#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MB 1

#define DEF_MODE S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP
#define DEF_UMASK S_IROTH | S_IWOTH | S_IXOTH

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
	//打开一个读文件
	//没有必要指定文件权限位
	return Open(sourceFile,O_RDONLY,0);
}

int openDestFile(char  *destFile)
{
	//目的文件
	//       不存在则新建一个
	//       存在就截断
	//       以可读写的方式打开
	//
	return Open(destFile,O_CREAT | O_TRUNC | O_RDWR ,DEF_MODE);
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


void *Malloc(size_t size)
{
	if(!malloc(size))
	{
		unix_error("向堆请求字节数组出错");
	}
}


void doCopy(int srcFd,int destFd)
{
	ssize_t num = -1;
	int n = 1024 * MB;
	void *buf = Malloc(n);
	while((num = Read(srcFd,buf,n)) >  0)
	{
				
		Write(destFd,buf,(size_t)num);	
	}
	free(buf);
}

int main(int argc,char  *argv[])
{
	if(argc != 3)
	{
		printf("用法\n SimpleCopy 源文件路径 目的文件路径\n");
		exit(0);
	}	

	umask(DEF_UMASK);	
	int srcFd = openSourceFile(argv[1]);
	int destFd = openDestFile(argv[2]);

	doCopy(srcFd,destFd);

	closeFile(srcFd);
	closeFile(destFd);
}
