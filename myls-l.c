#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<errno.h>
#include<grp.h>
#include<pwd.h>
#include<time.h>
#include<string.h>
#define BUFSIZE 1024

// st_mode 转 字符串
void modetoStr(int mode,char *buf)
{
	strncpy(buf,"",1); // 初始化buf
	// 首先判断文件类型
	if(S_ISREG(mode))
		strncat(buf,"-",2);
	if(S_ISDIR(mode))
		strncat(buf,"d",2);
	if(S_ISCHR(mode))
		strncat(buf,"c",2);
	if(S_ISBLK(mode))
		strncat(buf,"b",2);
	if(S_ISFIFO(mode))
		strncat(buf,"f",2);
	if(S_ISLNK(mode))
		strncat(buf,"l",2);
	if(S_ISSOCK(mode))
		strncat(buf,"s",2);
		
	// 其次判断user group others 的读写执行权限
	if(mode & S_IRUSR)
		strncat(buf,"r",2);
	else
		strncat(buf,"-",2);
	if(mode & S_IWUSR)
		strncat(buf,"w",2);
	else
		strncat(buf,"-",2);
	if(mode & S_IXUSR)
		strncat(buf,"x",2);
	else
		strncat(buf,"-",2);
		
	if(mode & S_IRGRP)
		strncat(buf,"r",2);
	else
		strncat(buf,"-",2);
		
	if(mode & S_IWGRP)
		strncat(buf,"w",2);
	else
		strncat(buf,"-",2);
	if(mode & S_IXGRP)
		strncat(buf,"x",2);
	else
		strncat(buf,"-",2);
	if(mode & S_IROTH)
		strncat(buf,"r",2);
	else
		strncat(buf,"-",2);
		
	if(mode & S_IWOTH)
		strncat(buf,"w",2);
	else
		strncat(buf,"-",2);
	if(mode & S_IXOTH)
		strncat(buf,"x",2);
	else
		strncat(buf,"-",2);
}

int main(int argc,char **argv)
{
	DIR* dp;
	struct dirent *cur;
	struct stat statbuf[BUFSIZE];
	struct group *groupres;
	struct passwd *usrres;
	struct tm* tmres;
	char timebuf[BUFSIZE];
	char strmode[BUFSIZE];
	char cwd[200];
	
	if(argc == 1)  // print current dir
	{
		getcwd(cwd,200);
		dp = opendir(cwd);
		if(dp == NULL)
		{
			perror("opendir");
			exit(1);
		}
	}
	if(argc == 2) 
	{
		dp = opendir(argv[2]);
	}
	while((cur = readdir(dp)) != NULL)
	{
		if(lstat(cur->d_name,statbuf))
		{
			perror("lstat()");
			exit(1);
		}	
		if(strcmp(cur->d_name,".")!=0 && strcmp(cur->d_name,"..") != 0)// 上级目录不输出
			
		{// use getgrgid to fetch groupname 
		groupres = getgrgid(statbuf->st_gid);
		// use getpwuid to fetch username
		usrres = getpwuid(statbuf->st_uid);
		// use gmtime to transfer time_t to *tm
		tmres = gmtime(&statbuf->st_atime);
		strftime(timebuf,BUFSIZE,"%m月 %d %H:%M",tmres);
		// use my defined function modetoStr 
		modetoStr(statbuf->st_mode,strmode);
		fprintf(stdout,"%s\t",strmode);
		fprintf(stdout,"%ld\t",statbuf->st_nlink);
		fprintf(stdout,"%s\t",groupres->gr_name);
		fprintf(stdout,"%s\t",usrres->pw_name);
		fprintf(stdout,"%ld\t",statbuf->st_size);
		fprintf(stdout,"%s\t",timebuf);
		fprintf(stdout,"%s\t",cur->d_name);
		putchar('\n');
		}
	}
	closedir(dp);
	exit(0);
}
