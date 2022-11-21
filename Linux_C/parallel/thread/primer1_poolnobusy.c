#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<pthread.h>
#include<string.h>


#define LEFT  30000000
#define RIGHT 30000200
#define THRNUM 4
/*
 *使用线程池对四个线程分配两百个筛质数任务
 *
 *注意mutex的反复使用 对临界区慎之又慎
 *
 *引用条件变量及其函数实现非盲等优化
 * 产生盲等的根本原因：mutex_num反复循环查询num值是否变化
 * 引入条件变量cond,cond外的循环条件一旦不成立即cond成立，此时唤醒mutex_num
 * 同时配合signal以及broadcast通知其他正在等待的进程
 *	signal、broadcast通常位于在cond条件的成立的语句下
 *
 */



static int num = 0;
static pthread_mutex_t mut_num = PTHREAD_MUTEX_INITIALIZER; // 锁住num
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;//观察num有无变化

static void* thr_prime(void *p);
static void* thr_prime(void *p)
{
	int i,j,mark = 1;
	while(1)
	{	
		pthread_mutex_lock(&mut_num);
		while(num == 0)
		{
/*			pthread_mutex_unlock(&mut_num);
			sched_yield();
			pthread_mutex_lock(&mut_num);
			// 等待任务投放
*/
			pthread_cond_wait(&cond,&mut_num);
		}
		if(num == -1)
		{
			pthread_mutex_unlock(&mut_num);// !直接break会导致死锁
			break;
		}
		i = num;
		num = 0;
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mut_num);
		mark = 1;	
		for(j = 2; j < i/2 ; j++)
		{
			if(i % j == 0)
			{
				mark = 0;
				break;
			}
		}
		if(mark == 1)
			printf("[%d]%d is a primer\n",(int)p,i);
	}

	pthread_exit(NULL);
}



int main()
{

	int i;
	pthread_t tid[THRNUM];
	for(i = 0; i < THRNUM ; i++)
	{

		int err = pthread_create(tid+i,NULL,thr_prime,(void *)i);
		//i !!!
		if(err)
		{
			fprintf(stdout,"pthread_create():%s\n",strerror(err));
			exit(1);
		}
	}


	for(i = LEFT; i <= RIGHT; i++)
	{
		pthread_mutex_lock(&mut_num);
		while(num != 0)
		{
/*			pthread_mutex_unlock(&mut_num);
			sched_yield(); // 出让调度器给别的线程，让它有机会拿到任务
			pthread_mutex_lock(&mut_num);
*/
			pthread_cond_wait(&cond,&mut_num);

		} // 等待num被某个线程取走
		num = i;
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mut_num);
	}
	pthread_mutex_lock(&mut_num);
	while(num != 0)
	{
/*		pthread_mutex_unlock(&mut_num);
		sched_yield();
		pthread_mutex_lock(&mut_num);
*/
		pthread_cond_wait(&cond,&mut_num);

	}// 确保最后一个任务被取走
	num = -1; // 任务分配完毕
	pthread_cond_broadcast(&cond);

	pthread_mutex_unlock(&mut_num);
	for(i = 0; i < THRNUM ; i++)
	{
		pthread_join(tid[i],NULL);
	}
	exit(0);

	pthread_mutex_destroy(&mut_num);
	pthread_cond_destroy(&cond);
}	
