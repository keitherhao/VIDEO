#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
 
int main(int argc, char* argv[])
{
	int fd,i,lednum;
 
	fd = open("/dev/led",O_RDWR);
	if(fd<0)
	{
		perror("open fail \n");
		return ;
	}
	for(i=0;i<100;i++)
	{
		lednum=0;
		write(fd,&lednum,sizeof(int));
		lednum = i%4+1;
		write(fd,&lednum,sizeof(int));	
		sleep(1);
	}
	close(fd);
    return 0;
}