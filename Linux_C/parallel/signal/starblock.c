#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>

/*
	结合本例，联系了sigset的使用
	尤其是setprocmake的三个参数
	SIG_BLOCK --》阻塞
	SIG_UNBLOCK --》解除阻塞
	SIG_SETMASK --》设置为某个信号集的状态


*/
void int_handeller(int s)
{
	write(1,"!",1);
}
int main()
{
	signal(SIGINT,int_handeller);
//	signal(SIGINT,SIG_IGN); // 无视终止符
	sigset_t set,oset,saveset;
	sigemptyset(&set);
	sigaddset(&set,SIGINT);
	// 考虑宏观编程思想，进入main模块前与出来之后状态需保持一致，需要保存现场且恢复
	// 保存现场
	sigprocmask(SIG_UNBLOCK,&set,&saveset);
	// 为什么是unblock？因为我的目的是block
	while(1)
	{
	sigprocmask(SIG_BLOCK,&set,&oset); // 置信号为阻塞，不响应
	for(int i=0; i < 3; i++)
	{
		write(1,"*",1);
		sleep(1);
	}
	putchar('\n');
	sigprocmask(SIG_UNBLOCK,&oset,NULL);
	// 非阻塞，响应 
	// 在阻塞状态下的中断，一旦解除阻塞会立马响应
	}	
	sigprocmask(SIG_SETMASK,&saveset,NULL);
	// 恢复之前的值
	exit(0);
}
