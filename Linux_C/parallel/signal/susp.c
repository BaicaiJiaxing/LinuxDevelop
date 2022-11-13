#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>

/*
	结合sigset几个函数比较pause与suspend的用法
	pause无法完全做到信号驱动
	本题的所谓驱动：在行内打断多少次都只会输入一个感叹号，且只在下一行首出现
	suspend相当于多行注释的原子操作
	需注意sigset的参数之间，新旧关系。

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
    sigprocmask(SIG_UNBLOCK,&set,&saveset);
    sigprocmask(SIG_BLOCK,&set,&oset);
	while(1)
	{
	for(int i=0; i < 3; i++)
	{
		write(1,"*",1);
		sleep(1);
	}
	putchar('\n');
	sigsuspend(&oset); // 使用suspend
	// 相当于如下多行注释的原子操作
	
/*	sigset_t tempset;
	sigprocmask(SIG_SETMASK,&oset,&tempset); // shezhioset值到当前信号集，同时保存当前集合到tempset
	pause(); 
sigprocmask(SIG_SETMASK,&tempset,NULL);
//非原子操作，依旧不能达到中断驱动一行
*/
// 恢复oset之前的集合	
	}	
	sigprocmask(SIG_SETMASK,&saveset,NULL);
	
	exit(0);
}
