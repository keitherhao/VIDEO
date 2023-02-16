#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
// linux
#include <linux/videodev2.h>
// my
#include "linux/list_.h"
#include "log.h"
#include "camera.h"

int camera_init(CAMERA_INIT* _p)
{
    int ret = -1;
    DBG_PRINTF("// 1.打开设备\n");
	// _p->fd = open(_p->dev_naem, O_RDWR | O_NONBLOCK, 0);
	_p->fd = open(_p->dev_naem, O_RDWR);
	if(_p->fd < 0)
	{
		perror("打开设备失败");
		return -1;
	}
    
	DBG_PRINTF("// 获取驱动信息\n");
    struct v4l2_capability cap;
    ret = ioctl(_p->fd, VIDIOC_QUERYCAP, &cap);
    if (ret < 0) {
        printf("VIDIOC_QUERYCAP failed (%d)\n", ret);
        return ret;
    }
    printf("Capability Informations:\n");
    printf("\tdriver: %s\n", cap.driver);
    printf("\tcard: %s\n", cap.card);
    printf("\tbus_info: %s\n", cap.bus_info);
    printf("\tversion: %08X\n", cap.version);
    printf("\tcapabilities: %08X\n", cap.capabilities);

    DBG_PRINTF("// 2.获取摄像头支持的格式ioctl(文件描述符， 命令， 与命令对应的结构体)\n");
	struct v4l2_fmtdesc v4fmt;
	v4fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	int i = 0;
	while(1)
	{
		v4fmt.index = i++;  
		ret = ioctl(_p->fd, VIDIOC_ENUM_FMT, &v4fmt);
		if(ret < 0)
		{
			if (i == 1)
				perror("获取失败");
			break;
		}
		printf("supported formats -%d- :\n", i);
		printf("\tindex: %d\n", v4fmt.index);
		printf("\tflags: %d\n", v4fmt.flags);
		printf("\tdescription: %s\n", v4fmt.description);
		unsigned char *set_pf = (unsigned char *)&v4fmt.pixelformat;
		printf("\tpixelformat: %c%c%c%c\n", set_pf[0],set_pf[1],set_pf[2],set_pf[3]);
		printf("\treserved: %d\n", v4fmt.reserved[0]);
	}

    DBG_PRINTF("// 设置采集格式\n");
	ret = ioctl(_p->fd, VIDIOC_S_FMT, &_p->vfmt);
	if(ret < 0)
	{
		perror("设置格式失败");
		return ret;
	}
	DBG_PRINTF("// 读取当前的采集格式\n");
	memset(&_p->vfmt, 0, sizeof(_p->vfmt));
	_p->vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret  = ioctl(_p->fd, VIDIOC_G_FMT, &_p->vfmt);
	if(ret < 0)
	{
		perror("获取格式失败");
		return ret;
	}
	printf("Formatted :\n");
	printf("\twidth: %d\n", _p->vfmt.fmt.pix.width);
	printf("\theight: %d\n", _p->vfmt.fmt.pix.height);
	unsigned char *get_pf = (unsigned char *)&_p->vfmt.fmt.pix.pixelformat;
	printf("\tpixelformat: %c%c%c%c\n", get_pf[0],get_pf[1],get_pf[2],get_pf[3]);

    DBG_PRINTF("// 4.申请内核空间\n");
	ret  = ioctl(_p->fd, VIDIOC_REQBUFS, &_p->reqbuffer);
	if(ret < 0)
	{
		perror("申请队列空间失败");
		return ret;
	}

    DBG_PRINTF("// 5.映射\n");
	// 读取缓存
	ret = ioctl(_p->fd, VIDIOC_QUERYBUF, &_p->mapbuffer);//从内核空间中查询一个空间做映射
	if(ret < 0)
	{
		perror("查询内核空间队列失败");
		return ret;
	}
	DBG_PRINTF("mapbuffer.m.offset = %d\n", _p->mapbuffer.m.offset);
	// 转换成相对地址
	_p->mptr = (unsigned char *)mmap(NULL, _p->mapbuffer.length, PROT_READ|PROT_WRITE, MAP_SHARED, _p->fd, _p->mapbuffer.m.offset);
	_p->size = _p->mapbuffer.length;


	DBG_PRINTF("// 开始采集\n");
	ret = ioctl(_p->fd, VIDIOC_STREAMON, &_p->type);
	if(ret < 0)
	{
		perror("开启失败");
		return ret;
	}

    return 0;
}

int camera_get_a_frame(int* pfd, unsigned char** dest)
{
	int ret = -1;
	CAMERA_INIT* _p = container_of(pfd, CAMERA_INIT, fd);
	DBG_PRINTF("// 把一帧数据放入缓存队列\n");
	ret = ioctl(_p->fd, VIDIOC_QBUF, &_p->mapbuffer);
	if (ret < 0) 
	{
		perror("放入缓存队列失败");
		return ret;
	}
	DBG_PRINTF("// 从队列中提取一帧数据\n");
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(_p->fd, &fds);
	struct timeval tv = {0};
	tv.tv_sec = 1; 
	tv.tv_usec = 0;
	int r = select(_p->fd+1, &fds, NULL, NULL, &tv);
	if(-1 == r)
	{
		perror("Waiting for Frame");
	}
	// memset(&(_p->mapbuffer), 0x00, sizeof(struct v4l2_buffer));
	ret = ioctl(_p->fd, VIDIOC_DQBUF, &_p->mapbuffer);
	if (ret < 0)
	{
		perror("提取数据失败");
		return ret;
	}
	// if (_p->mapbuffer.length > 0)
		return _p->mapbuffer.length;
	// else 
		// return ret;
}

int camera_exit(int* pfd)
{
    int ret = -1;
    CAMERA_INIT* _p = container_of(pfd, CAMERA_INIT, fd);
    DBG_PRINTF("// 8. 停止采集\n");
    ret = ioctl(_p->fd, VIDIOC_STREAMOFF, &_p->type);

    DBG_PRINTF("// 9.释放映射\n");
	int i=0;
    munmap(_p->mptr, _p->size);

    DBG_PRINTF("// 10.关闭设备\n");
	close(_p->fd);
    return 0;
}