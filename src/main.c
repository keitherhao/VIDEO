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

#include "main.h"

int main(void)
{
    DBG_PRINTF("// 1.打开设备\n");
	// 1.打开设备
	int fd = open("/dev/video1", O_RDWR);//14,15,21,22
	if(fd < 0)
	{
		perror("打开设备失败");
		return -1;
	}
    
    DBG_PRINTF("// 2.获取摄像头支持的格式ioctl(文件描述符， 命令， 与命令对应的结构体)\n");
	// 2.获取摄像头支持的格式ioctl(文件描述符， 命令， 与命令对应的结构体)
	struct v4l2_fmtdesc v4fmt;
	v4fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	int i=0;
	while(1)
	{
		v4fmt.index = i++;  
		int ret = ioctl(fd, VIDIOC_ENUM_FMT, &v4fmt);
		if(ret < 0)
		{
			perror("获取失败");
			break;
		}
		printf("index=%d\n", v4fmt.index);
		printf("flags=%d\n", v4fmt.flags);
		printf("description=%s\n", v4fmt.description);
		unsigned char *p = (unsigned char *)&v4fmt.pixelformat;
		printf("pixelformat=%c%c%c%c\n", p[0],p[1],p[2],p[3]);
		printf("reserved=%d\n", v4fmt.reserved[0]);
	}

    DBG_PRINTF("// 3.设置采集格式\n");
    // 3.设置采集格式
	struct v4l2_format vfmt;
	vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;//摄像头采集
	vfmt.fmt.pix.width = 640;//设置宽（不能任意）
	vfmt.fmt.pix.height = 480;//设置高
	// vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;//设置视频采集格式
	vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	int ret = ioctl(fd, VIDIOC_S_FMT, &vfmt);
	if(ret < 0)
	{
		perror("设置格式失败");
	}

	memset(&vfmt, 0, sizeof(vfmt));
	vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret  = ioctl(fd, VIDIOC_G_FMT, &vfmt);
	if(ret < 0)
	{
		perror("获取格式失败");
	}

	if(vfmt.fmt.pix.width == 640 && vfmt.fmt.pix.height == 480 && 
	vfmt.fmt.pix.pixelformat == V4L2_PIX_FMT_YUYV)
	{
		printf("设置成功\n");
	}else
	{
		printf("设置失败\n");
	}

    DBG_PRINTF("// 4.申请内核空间\n");
    // 4.申请内核空间
	struct v4l2_requestbuffers reqbuffer;
	reqbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	reqbuffer.count = 4; //申请4个缓冲区
	reqbuffer.memory = V4L2_MEMORY_MMAP ;//映射方式
	ret  = ioctl(fd, VIDIOC_REQBUFS, &reqbuffer);
	if(ret < 0)
	{
		perror("申请队列空间失败");
	}

    DBG_PRINTF("// 5.映射\n");
    // 5.映射
	unsigned char *mptr[4];//保存映射后用户空间的首地址
    unsigned int  size[4];
	struct v4l2_buffer mapbuffer;
	//初始化type, index
	mapbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	for(int i=0; i<4; i++)
	{
		mapbuffer.index = i;
		ret = ioctl(fd, VIDIOC_QUERYBUF, &mapbuffer);//从内核空间中查询一个空间做映射
		if(ret < 0)
		{
			perror("查询内核空间队列失败");
		}
		mptr[i] = (unsigned char *)mmap(NULL, mapbuffer.length, PROT_READ|PROT_WRITE, 
                                            MAP_SHARED, fd, mapbuffer.m.offset);
            size[i]=mapbuffer.length;

		//通知使用完毕--‘放回去’
		ret  = ioctl(fd, VIDIOC_QBUF, &mapbuffer);
		if(ret < 0)
		{
			perror("放回失败");
		}
	}

    DBG_PRINTF("// 6.开始采集\n");
    // 6.开始采集
	int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(fd, VIDIOC_STREAMON, &type);
	if(ret < 0)
	{
		perror("开启失败");
	}

    DBG_PRINTF("// 7.从队列中提取一帧数据\n");
    // 7.从队列中提取一帧数据
	struct v4l2_buffer  readbuffer;
	readbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(fd, VIDIOC_DQBUF, &readbuffer);
	if(ret < 0)
	{
		perror("提取数据失败");
	}

	FILE *file=fopen("my.jpg", "w+");
	//mptr[readbuffer.index]
	fwrite(mptr[readbuffer.index], readbuffer.length, 1, file);
	fclose(file);
	
	//通知内核已经使用完毕
	ret = ioctl(fd, VIDIOC_QBUF, &readbuffer);
	if(ret < 0)
	{
		perror("放回队列失败");
	}

    DBG_PRINTF("// 8. 停止采集\n");
    // 8. 停止采集
    ret = ioctl(fd, VIDIOC_STREAMOFF, &type);

    DBG_PRINTF("// 9.释放映射\n");
    // 9.释放映射
    for(int i=0; i<4; i++)
    munmap(mptr[i], size[i]);

    DBG_PRINTF("// 10.关闭设备\n");
	//10.关闭设备
	close(fd);
	return 0;
}