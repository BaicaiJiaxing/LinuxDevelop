#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<signal.h>
#include<errno.h>

#define CPS 10
#define BUFSIZE CPS

/*

	cat命令的漏桶实现;

*/

static volatile int loop = 0;
static void alrm_handler(int s)
{
	alarm(1);// (!!!) 保持每秒一个alarm
	loop = 1;
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
	alarm(1); // !!!
	
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
		while((len = read(sfd,buf,BUFSIZE)) < 0);
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
			while(loop == 0) // loop 0 时 不写入
				pause(); // 去掉盲等，出让cpu
			loop = 0;  // !!!
			
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
