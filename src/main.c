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
    camera_init(&camera_int);

    DBG_PRINTF("// 6.开始采集\n");
    // 6.开始采集
	camera_int.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(camera_int.fd, VIDIOC_STREAMON, &camera_int.type);
	if(ret < 0)
	{
		perror("开启失败");
	}

    DBG_PRINTF("// 7.从队列中提取一帧数据\n");
    // 7.从队列中提取一帧数据
	camera_int.readbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(camera_int.fd, VIDIOC_DQBUF, &camera_int.readbuffer);
	if(ret < 0)
	{
		perror("提取数据失败");
	}

	FILE *file=fopen("/mnt/my.jpg", "w");
	//mptr[readbuffer.index]
	fwrite(camera_int.mptr[camera_int.readbuffer.index], camera_int.readbuffer.length, 1, file);
	fclose(file);
	
	camera_exit(&camera_int.fd);
	return 0;
}