#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include<pthread.h>
#include <errno.h>
#include <unistd.h>
#include "mytbf.h"

//typedef void (*sighandler_t)(int);

static pthread_mutex_t  mut_job = PTHREAD_MUTEX_INITIALIZER;
static pthread_t tid_alrm;
static pthread_once_t init_once = PTHREAD_ONCE_INIT;
static struct mytbf_st* job[MYTBF_MAX];

static int inited = 0;

//static sighandler_t alrm_handler_save;  // 恢复信号注册前的信号

struct mytbf_st
{
	int cps;   // 工作效率
	int burst; // 令牌桶最大容量
	int token; // 当前令牌数量
	pthread_mutex_t mut; // 更改token的互斥量
	pthread_cond_t cond; // 与mut联合使用
	int pos;   // 令牌桶数组下标
};	

static int get_free_pos_unlocked(void)  // 获取当前空闲的令牌桶下标
{ // ——unlocked版 先加锁再调用
	int i;
	for(i =0 ; i < MYTBF_MAX; i++)
	{
		if(job[i] == NULL)
			return i;
	}
	return -1;
}
static void *thr_alrm(void *s)
{
	int i;
	
	while(1)
	{
		pthread_mutex_lock(&mut_job);
		for(i = 0; i < MYTBF_MAX ; i++)
	{
		if(job[i] != NULL)
		{
			pthread_mutex_lock(&job[i]->mut);
			job[i]->token += job[i]->cps;
			if(job[i]->token > job[i]->burst)
				job[i]->token = job[i]->burst;

			pthread_cond_broadcast(&job[i]->cond);
			// cond成立时，唤醒所有正在等待的进程
			pthread_mutex_unlock(&job[i]->mut);
		}
	}
		pthread_mutex_unlock(&mut_job);
		sleep(1);
	}

}
static void module_unload()
{
        int i;
     //   signal(SIGALRM,alrm_handler_save);
     // alarm(0); // 关掉alarm，恢复模块出去之前的模样
    
	pthread_cancel(tid_alrm);
	// 结合join 对一个死循环的线程进行收尸
	pthread_join(tid_alrm,NULL);
    	for(i = 0; i < MYTBF_MAX; i++)
	{
		if(job[i] != NULL)
		{

			mytbf_destroy(job[i]);
		}

	}
	pthread_mutex_destroy(&mut_job);
}

static void module_load()
{
//	alrm_handler_save = signal(SIGALRM,alrm_handler);
//	alarm(1);

//	pthread_t tid_alrm;
	int err;
	err = pthread_create(&tid_alrm,NULL,thr_alrm,NULL);
	if(err)
	{
		fprintf(stderr,"pthread_create():%s\n",strerror(err));
		exit(1);
	}

	atexit(module_unload); 
	// load出问题或要结束时调用unload
}

mytbf_t *mytbf_init(int cps, int burst)  // 初始化令牌桶
{
	struct mytbf_st *me;
	int pos;
/*
 	lock();
	if(!inited)
	{
		module_load();  // 加载模块 注册信号
		inited = 1;
	}
	unlock();
*/
	pthread_once(&init_once,module_load);
	//该模块只加载一次 引入该函数
	me = (struct mytbf_st *)malloc(sizeof(*me));
	me->token = 0;
	me->cps = cps;
	me->burst = burst;
	pthread_mutex_init(&me->mut,NULL);
	pthread_cond_init(&me->cond,NULL);

	if(me == NULL)
                return NULL;

	pthread_mutex_lock(&mut_job);
	pos = get_free_pos_unlocked();// 此函数也是临界区的跳转语句
	if(pos < 0) 
	{
		pthread_mutex_unlock(&mut_job);
		free(me);
		return NULL;//临界区跳转语句
	}

	me->pos = pos;

	job[pos] = me;
	pthread_mutex_unlock(&mut_job);
	return me;

}

int min(int a, int b)
{
	return a < b ? a : b;
}
int mytbf_fetchtoken(mytbf_t *ptr , int size)
{
	struct mytbf_st *me = ptr;
	int n;
	if(size <= 0)
		return -EINVAL;// 参数非法

	while(me->token <= 0)
	{
	/*	pthread_mutex_unlock(&me->mut);
		sched_yield();
		pthread_mutex_lock(&me->mut);
	*/
		pthread_cond_wait(&me->cond,&me->mut);
		// 如果条件成立，就解锁
	}

	n = min(me->token,size);
	me->token -= n;
	pthread_mutex_unlock(&me->mut);
	return n;
}
int mytbf_returntoken(mytbf_t *ptr, int size)
{
	struct mytbf_st *me = ptr;
	if(size <= 0)
		return -EINVAL;
	pthread_mutex_lock(&me->mut);
	me->token += size;

	if(me->token > me->burst)
		me->token = me->burst;

	pthread_cond_broadcast(&me->cond);
	pthread_mutex_unlock(&me->mut);
	return size;
}

void mytbf_destroy(mytbf_t *ptr )
{
	struct mytbf_st *me = ptr;
	pthread_mutex_lock(&mut_job);
	job[me->pos] = NULL;
	pthread_mutex_unlock(&mut_job);

	pthread_mutex_destroy(&me->mut);
	pthread_cond_destroy(&me->cond);
	free(ptr);

}
