#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>
/*
	使用信号量来解决一边计算一边打印的问题
	问题范畴：双向同步模型
	需要设置两个信号量
	一个作为空闲单元sem，一个作为数据单元ready
	打印与计算线程之间有明确的先后顺序，且只有一个存储数据的单元
	计算线程在计算之前需要wait操作来占有数据单元，计算之后释放sem
	打印进程需要占有当前sem执行打印，打印完毕后释放数据单元

*/
void *print(void* arg);
void *calculate(void* arg);

static int a = 0;
static sem_t  sem;
static sem_t ready;
int main()
{
	pthread_t p,c;
	
	sem_init(&sem,0,1);
	sem_init(&ready,0,1);
	
	
	pthread_create(&p,NULL,print,NULL);
	pthread_create(&c,NULL,calculate,NULL);
	
	pthread_join(p,NULL);
	pthread_join(c,NULL);

	sem_destroy(&sem);
	
	exit(0);
}
void *print(void* arg)
{
	for(int i=0 ; i< 100; i++)
	{
		sem_wait(&sem); // 有空闲单元
		printf("print: %d\n",a);
		//usleep(1);
		sem_post(&ready);
		usleep(1);
	}
}
void *calculate(void* arg)
{
	for(int i=0 ; i< 100; i++)
	{
		sem_wait(&ready);
		a++;
		//usleep(1);
		sem_post(&sem);
		usleep(1);
	}
}







