#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
// my
#include "linux/list_.h"
#include "log.h"
#include "camera.h"

int camera_init(CAMERA_INIT* parameter)
{
    int ret = -1;
    DBG_PRINTF("// 1.打开设备\n");
	// 1.打开设备
	parameter->fd = open("/dev/video0", O_RDWR);//14,15,21,22
	if(parameter->fd < 0)
	{
		perror("打开设备失败");
		return -1;
	}
    
    DBG_PRINTF("// 2.获取摄像头支持的格式ioctl(文件描述符， 命令， 与命令对应的结构体)\n");
	// 2.获取摄像头支持的格式ioctl(文件描述符， 命令， 与命令对应的结构体)
	parameter->v4fmt;
	parameter->v4fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	int i=0;
	while(1)
	{
		parameter->v4fmt.index = i++;  
		ret = ioctl(parameter->fd, VIDIOC_ENUM_FMT, &parameter->v4fmt);
		if(ret < 0)
		{
			if (i == 1)
				perror("获取失败");
			break;
		}
		printf("index=%d\n", parameter->v4fmt.index);
		printf("flags=%d\n", parameter->v4fmt.flags);
		printf("description=%s\n", parameter->v4fmt.description);
		unsigned char *p = (unsigned char *)&parameter->v4fmt.pixelformat;
		printf("pixelformat=%c%c%c%c\n", p[0],p[1],p[2],p[3]);
		printf("reserved=%d\n", parameter->v4fmt.reserved[0]);
	}

    DBG_PRINTF("// 3.设置采集格式\n");
    // 3.设置采集格式
	parameter->vfmt;
	parameter->vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;//摄像头采集
	parameter->vfmt.fmt.pix.width = 640;//设置宽（不能任意）
	parameter->vfmt.fmt.pix.height = 480;//设置高
	// vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;//设置视频采集格式
	parameter->vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	ret = ioctl(parameter->fd, VIDIOC_S_FMT, &parameter->vfmt);
	if(ret < 0)
	{
		perror("设置格式失败");
	}

	memset(&parameter->vfmt, 0, sizeof(parameter->vfmt));
	parameter->vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret  = ioctl(parameter->fd, VIDIOC_G_FMT, &parameter->vfmt);
	if(ret < 0)
	{
		perror("获取格式失败");
	}

	if(parameter->vfmt.fmt.pix.width == 640 && parameter->vfmt.fmt.pix.height == 480 && 
		(parameter->vfmt.fmt.pix.pixelformat == V4L2_PIX_FMT_YUYV || parameter->vfmt.fmt.pix.pixelformat == V4L2_PIX_FMT_MJPEG))
	{
		printf("设置成功\n");
	}else
	{
		printf("设置失败\n");
	}

    DBG_PRINTF("// 4.申请内核空间\n");
    // 4.申请内核空间
	parameter->reqbuffer;
	parameter->reqbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	parameter->reqbuffer.count = 4; //申请4个缓冲区
	parameter->reqbuffer.memory = V4L2_MEMORY_MMAP ;//映射方式
	ret  = ioctl(parameter->fd, VIDIOC_REQBUFS, &parameter->reqbuffer);
	if(ret < 0)
	{
		perror("申请队列空间失败");
	}

    DBG_PRINTF("// 5.映射\n");
    // 5.映射
	// parameter->*mptr[4];//保存映射后用户空间的首地址
    // parameter->size[4];
	parameter->mapbuffer;
	//初始化type, index
	parameter->mapbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	for(int i=0; i<4; i++)
	{
		parameter->mapbuffer.index = i;
		ret = ioctl(parameter->fd, VIDIOC_QUERYBUF, &parameter->mapbuffer);//从内核空间中查询一个空间做映射
		if(ret < 0)
		{
			perror("查询内核空间队列失败");
		}
		parameter->mptr[i] = (unsigned char *)mmap(NULL, parameter->mapbuffer.length, PROT_READ|PROT_WRITE, 
                                            MAP_SHARED, parameter->fd, parameter->mapbuffer.m.offset);
            parameter->size[i]=parameter->mapbuffer.length;

		//通知使用完毕--‘放回去’
		ret  = ioctl(parameter->fd, VIDIOC_QBUF, &parameter->mapbuffer);
		if(ret < 0)
		{
			perror("放回失败");
		}
	}

    return 0;
}


int camera_exit(int* pfd)
{
    int ret = -1;
    CAMERA_INIT* _p = container_of(pfd, CAMERA_INIT, fd);
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    DBG_PRINTF("// 8. 停止采集\n");
    // 8. 停止采集
    ret = ioctl(_p->fd, VIDIOC_STREAMOFF, &_p->type);

    DBG_PRINTF("// 9.释放映射\n");
    // 9.释放映射
    for(int i=0; i<4; i++)
    munmap(_p->mptr[i], _p->size[i]);

    DBG_PRINTF("// 10.关闭设备\n");
	//10.关闭设备
	close(_p->fd);
    return 0;
}