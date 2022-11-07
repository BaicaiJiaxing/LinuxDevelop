#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>

/*
	使用共享变量实现一边计算1到100的累加，一边打印数据
	function：pthread_mutex_init lock unlock destroy
		  pthread_create pthread_join
	summary:
		需要明白什么是临界区;
		临界区内的代码和临界区外的代码的区别：在此题应该是对a的使用
		使用sleep的深意
		为什么要锁在循环内部：在循环外部会锁住整个线程直到线程结束
		lock用于使用临界区资源
		unlock用于归还临界区资源

*/
void *print(void* arg);
void *calculate(void* arg);

static int a = 0;
static pthread_mutex_t mutex;

int main()
{
	pthread_t p,c;
	
	pthread_mutex_init(&mutex,NULL);
	
	pthread_create(&p,NULL,print,NULL);
	pthread_create(&c,NULL,calculate,NULL);
	
	pthread_join(p,NULL);
	pthread_join(c,NULL);

	pthread_mutex_destroy(&mutex);
	exit(0);
}
void *print(void* arg)
{
	for(int i=0 ; i< 100; i++)
	{
		pthread_mutex_lock(&mutex);
		printf("print: %d\n",a);
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}
}
void *calculate(void* arg)
{
	for(int i=0 ; i< 100; i++)
	{
		pthread_mutex_lock(&mutex);
		a++;
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}
}







