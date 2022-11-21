#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<signal.h>
#include<errno.h>
#include<pthread.h>

#include "mytbf.h"


#define CPS 10
#define BUFSIZE CPS
#define BURST 100 // 令牌桶的最大容量
/*

	cat命令的令牌桶的封装以及实现;

*/

int main(int argc,char **argv)
{

	int sfd,dfd = 1;
	int len;
	int size;
	char buf[BUFSIZE];
	int ret , pos;
	mytbf_t *tbf = NULL;

	if(argc < 2)
	{
		fprintf(stderr,"Usage: %s <src_file>\n ",argv[0]);
		exit(1);
	}	

	tbf = mytbf_init(CPS,BURST); // 初始化令牌桶
	if(tbf == NULL)
	{
		fprintf(stderr,"mytbf_init() failed\n");
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
	}while(sfd < 0);  // 打开失败
	
	while(1)
	{
		size = mytbf_fetchtoken(tbf,BUFSIZE);
		if(size < 0)  // 没有令牌了 出错停止？？？
		{
			fprintf(stderr,"%s\n",strerror(-size));
			exit(1);
		}
		while((len = read(sfd,buf,size)) < 0)
		{
			if(errno == EINTR)
				continue;
			perror("read()");
			break;
		}
		
		if(len == 0)
			break;
		if(size - len > 0)  // 读入len个 消耗len个令牌
				mytbf_returntoken(tbf,size-len);
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
	mytbf_destroy(tbf);

	exit(0);
}
