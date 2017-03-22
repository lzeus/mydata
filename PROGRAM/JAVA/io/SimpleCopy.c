#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MB 1

void unix_error(char *msg)
{	
	printf("%s: %s\n",msg,strerror(errno));
	exit(0);
}

int Open(char *pathname,int flag,mode_t mode)
{
	int fd = open(pathname,flag,mode);
	
	if(fd < 0)
	{
		unix_error("打开文件错误");
	}
	return fd;
}

int openSourceFile(char  *sourceFile)
{
	return Open(sourceFile,O_RDONLY,0);
}

int openDestFile(char  *destFile)
{
	return Open(destFile,O_CREAT | O_TRUNC | O_RDWR ,0);
}

ssize_t Read(int fd,void *buf,size_t n)
{
	ssize_t num = read(fd,buf,n);	
	if(num < 0)
	{
		unix_error("读文件出错");
	}
	return num;
}

ssize_t Write(int fd,void *buf,size_t n)
{
	ssize_t num =  write(fd,buf,n);
	if(num < 0)
	{
		unix_error("写文件出错");
	}
	return num;
}


void Close(int fd)
{
	if(close(fd) < 0)
	{
		unix_error("关闭文件出错");
	}	
}

void closeFile(int fd)
{
	Close(fd);
}



void doCopy(int srcFd,int destFd)
{
	ssize_t num = -1;
	int n = 1024 * MB;
	void *buf = malloc(n);
	while((num = Read(srcFd,buf,n)) >  0)
	{
				
		Write(destFd,buf,(size_t)num);	
	}
}





int main(int argc,char  *argv[])
{
	if(argc != 3)
	{
		printf("用法\n SimpleCopy 源文件路径 目的文件路径\n");
		exit(0);
	}	
	
	int srcFd = openSourceFile(argv[1]);
	int destFd = openDestFile(argv[2]);

	doCopy(srcFd,destFd);

	closeFile(srcFd);
	closeFile(destFd);
}
