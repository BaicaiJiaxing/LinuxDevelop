#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/time.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<signal.h>
#include<errno.h>

#define CPS 10
#define BUFSIZE CPS
#define BURST 100 // 令牌桶的最大容量
/*

	cat命令的令牌桶实现;
	tips: 使用sig_atomic_t替换掉int，是为了防止小概率的alrm_handler中的token++ 和 token-- 同时运行
	使用setitimer 替换掉alarm
	应尽量使用setitimer

*/

static volatile sig_atomic_t token = 0; // 令牌
static struct itimerval timerval = {{1,0},{1,0}};  // 给new itimerval赋值

static void alrm_handler(int s)
{
	//alarm(1);// (!!!) 保持每秒一个alarm
	setitimer(ITIMER_REAL,&timerval,NULL);
	token++;  // 每个中断 token++
	if(token > BURST)
		token = BURST;
}	
int main(int argc,char **argv)
{

	int sfd,dfd = 1;
	int len;
	char buf[BUFSIZE];
	int ret , pos;
	
	if(argc < 2)
	{
		fprintf(stderr,"Usage: %s <src_file> ",argv[0]);
		exit(1);
	}
	
	signal(SIGALRM,alrm_handler); // !!!
	//alarm(1); // !!!
	// 改进：使用setitimer替代alarm
	
	if(!setitimer(ITIMER_REAL,&timerval,NULL))
	{
		perror("setitimer");
		exit(1);
	}
	
	do
	{
	sfd = open(argv[1],O_RDONLY);
	if( sfd < 0)
	{
		perror("open()");
		exit(1);
	}
	}while(sfd < 0);
	
	while(1)
	{
		while(token <= 0)  // 如果没有令牌就暂停
			pause();
		token --; // 还有令牌 继续读
		// ！！！ 此处token--为非原子性操作，有小概率和alrm_handler中token++同时运行，故更改token的数据类型为 sig_atomic_t
		while((len = read(sfd,buf,BUFSIZE)) < 0)
		{
			if(errno == EINTR)
				continue;
			perror("read()");
			break;
		}
		
		if(len == 0)
			break;
		
		pos = 0;
		while(len > 0)
		{
			ret = write(dfd,buf+pos,len);
			if(ret < 0)
			{
				if(errno == EINTR)
					continue;
				perror("write()");
				exit(1);
			}
			len -= ret;
			pos += ret;
		}
	}

	close(sfd);

	exit(0);
}
