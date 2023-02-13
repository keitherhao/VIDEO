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
#include "log.h"
#include "camera.h"

int main(void)
{
	int ret = -1;
	CAMERA_INIT camera_int;
	camera_int.dev_naem = "/dev/video0"; //14,15,21,22
	camera_int.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	// vfmt 设置视频采集格式
	camera_int.vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;//摄像头采集
	camera_int.vfmt.fmt.pix.width = 640;//设置宽（不能任意）
	camera_int.vfmt.fmt.pix.height = 480;//设置高
	camera_int.vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	// reqbuffer 内核空间
	camera_int.reqbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	camera_int.reqbuffer.count = 1; //申请4个缓冲区
	camera_int.reqbuffer.memory = V4L2_MEMORY_MMAP ;//映射方式
	// map 用户空间
	camera_int.mapbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	camera_int.mapbuffer.index = 0;
    camera_init(&camera_int);

	char filename[16];
	int i = 0;
	for (i = 0; i<4; i++)
	{
		DBG_PRINTF("// 把一帧数据放入缓存队列\n");
		ret = ioctl(camera_int.fd, VIDIOC_QBUF, &camera_int.mapbuffer);
		if (ret < 0) 
		{
			perror("放入缓存队列失败");
		}
		DBG_PRINTF("// 从队列中提取一帧数据\n");
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(camera_int.fd, &fds);
		struct timeval tv = {0};
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		int r = select(camera_int.fd+1, &fds, NULL, NULL, &tv);
		if(-1 == r)
		{
			perror("Waiting for Frame");
			continue;;
		}
		ret = ioctl(camera_int.fd, VIDIOC_DQBUF, &camera_int.mapbuffer);
		if (ret < 0)
		{
			perror("提取数据失败");
		}

		sprintf(filename, "/mnt/my_%d.jpg", i);
		FILE *fp=fopen(filename, "w");
		fwrite(camera_int.mptr, camera_int.mapbuffer.length, 1, fp);
		// fdatasync(fp);
		fclose(fp);
	}
	

	camera_exit(&camera_int.fd);
	return 0;
}