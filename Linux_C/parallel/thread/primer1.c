#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<pthread.h>
#include<string.h>


#define LEFT  30000000
#define RIGHT 30000200
#define THRNUM (RIGHT - LEFT +1)
/*
 * 最直接最丑陋的解决此处竞争的问题
 * 对i进行值传递而非地址传递
 * 会产生警告，但是非错误，可以执行
 * 结果正确
 *
 */
static void* thr_prime(void *p);
static void* thr_prime(void *p)
{
	int i,j,mark = 1;
	i = (int) p; //!!!
	for(j = 2; j < i/2 ; j++)
	{
		if(i % j == 0)
		{
			mark = 0;
			break;
		}
	}
	if(mark == 1)
		printf("%d is a primer\n",i);
	

	pthread_exit(NULL);
}



int main()
{

	int i;
	pthread_t tid[THRNUM];
	for(i = LEFT; i <= RIGHT; i++)
	{

		int err = pthread_create(tid+i-LEFT,NULL,thr_prime,(void *)i);
		//i !!!
		if(err)
		{
			fprintf(stdout,"pthread_create():%s\n",strerror(err));
			exit(1);
		}
	}

	for(i = LEFT; i <= RIGHT; i++)
	{
		pthread_join(tid[i-LEFT],NULL);
	}
	exit(0);
}	
