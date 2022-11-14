#include<stdio.h>
#include<stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include "mytbf.h"

typedef void (*sighandler_t)(int);



static struct mytbf_st* job[MYTBF_MAX];

static int inited = 0;

//static sighandler_t alrm_handler_save;  // 恢复信号注册前的信号

static struct sigaction alrm_sa_save;

struct mytbf_st
{
	int cps;   // 工作效率
	int burst; // 令牌桶最大容量
	int token; // 当前令牌数量
	int pos;   // 令牌桶数组下标
};	

static int get_free_pos(void)  // 获取当前空闲的令牌桶下标
{
	int i;
	for(i =0 ; i < MYTBF_MAX; i++)
	{
		if(job[i] == NULL)
			return i;
	}
	return -1;
}
static void alrm_action(int s,siginfo_t *info,void *unused)
{
	int i;
//	alarm(1); // alarm链

	if(info->si_code != SI_KERNEL)
	{
		return ;// 不是kernel的信号，不响应
	}
	for(i = 0; i < MYTBF_MAX ; i++)
	{
		if(job[i] != NULL)
		{
			job[i]->token += job[i]->cps;
			if(job[i]->token > job[i]->burst)
				job[i]->token = job[i]->burst;
		}
	}
}
static void module_unload()
{
        int i;
  //      signal(SIGALRM,alrm_handler_save);
   //    alarm(0); // 关掉alarm，恢复模块出去之前的模样
   	
	struct itimerval itv = {{0,0},{0,0}};
	
	sigaction(SIGALRM,&alrm_sa_save,NULL);   
   	setitimer(ITIMER_REAL,&itv,NULL);


	for(i = 0; i < MYTBF_MAX; i++)
                free(job[i]);
}

static void module_load()
{
//	alrm_handler_save = signal(SIGALRM,alrm_handler);
//	alarm(1);
	struct sigaction sa;
	struct itimerval itv = {{1,0},{1,0}};
	sa.sa_sigaction = alrm_action;

	sa.sa_flags = SA_SIGINFO;

	
	sigaction(SIGALRM,&sa,&alrm_sa_save);
	/*if error*/
	setitimer(ITIMER_REAL,&itv,NULL);
	/*if error*/


	atexit(module_unload); 
	// load出问题或要结束时调用unload
}

mytbf_t *mytbf_init(int cps, int burst)  // 初始化令牌桶
{
	struct mytbf_st *me;
	int pos;
	if(!inited)
	{
		module_load();  // 加载模块 注册信号
		inited = 1;
	}
	pos = get_free_pos();
	if(pos < 0)
		return NULL;
	me = (struct mytbf_st *)malloc(sizeof(*me));
	if(me == NULL)
		return NULL;

	me->token = 0;
	me->cps = cps;
	me->burst = burst;
	me->pos = pos;
	job[pos] = me;
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
		pause();
	n = min(me->token,size);
	me->token -= n;
	return n;
}
int mytbf_returntoken(mytbf_t *ptr, int size)
{
	struct mytbf_st *me = ptr;
	if(size <= 0)
		return -EINVAL;

	me->token += size;

	if(me->token > me->burst)
		me->token = me->burst;

	return size;
}

void mytbf_destroy(mytbf_t *ptr )
{
	struct mytbf_st *me = ptr;
	job[me->pos] = NULL;
	free(ptr);

}
