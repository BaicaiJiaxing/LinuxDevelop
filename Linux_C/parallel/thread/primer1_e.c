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
 *
 * 解决强制转换导致的警告问题
 * 为什么需要丑陋的强转？->如果传入i的地址会导致200个线程对同一个地址进行访问导致竞争
 * 故选择传入值，200个进程对不同的值进行访问避免竞争
 *
 * 优化：创建结构体存放i，传入地址。
 * 200个进程对200个不同的地址进行访问避免竞争，同时避免强转
 *
 */
struct thr_arg_st
{
	int n;
};

static void* thr_prime(void *p);
static void* thr_prime(void *p)
{
	int i,j,mark = 1;
	i = ((struct thr_arg_st *)p)->n;
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
	

	pthread_exit(p); // 设置结束的返回值为p，便于在同一个函数中使用malloc并free
}



int main()
{

	int i;
	struct thr_arg_st *p;
	void *ptr;
	pthread_t tid[THRNUM];
	for(i = LEFT; i <= RIGHT; i++)
	{

		p = malloc(sizeof(*p));
		if(p == NULL)
		{
			perror("malloc()");
			exit(1);
		}
		p->n = i;
		int err = pthread_create(tid+i-LEFT,NULL,thr_prime,p);
		if(err)
		{
			fprintf(stdout,"pthread_create():%s\n",strerror(err));
			exit(1);
		}
	}

	for(i = LEFT; i <= RIGHT; i++)
	{
		pthread_join(tid[i-LEFT],&ptr);//!!!
		free(ptr);
	}
	exit(0);
}	
