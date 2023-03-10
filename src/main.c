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
#include "encode_video.h"

int take_photo(void)
{
	int ret = -1;
	CAMERA_INIT camera_int;
	camera_int.dev_naem = "/dev/video1"; //14,15,21,22
	camera_int.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	// vfmt 设置视频采集格式
	camera_int.vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;//摄像头采集
	// 240*320 640*480 1280*720 1920*1080
	camera_int.vfmt.fmt.pix.width = 1920;//设置宽（不能任意）
	camera_int.vfmt.fmt.pix.height = 1080;//设置高
	// camera_int.vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	camera_int.vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	// reqbuffer 内核空间
	camera_int.reqbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	camera_int.reqbuffer.count = 1; //申请4个缓冲区
	camera_int.reqbuffer.memory = V4L2_MEMORY_MMAP ;//映射方式
	// map 用户空间
	camera_int.mapbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	camera_int.mapbuffer.index = 0;
    camera_init(&camera_int);

	char filename[128];
	unsigned char *_data;
	for (int i = 0; i<4; i++)
	{
		ret = camera_get_a_frame(&camera_int.fd, &_data);
		if (ret < 0)
		{
			perror("get_a_frame error");
			// continue;
			printf("[ %s ] line #%d\tret = %d\n", __FUNCTION__, __LINE__, ret);
			camera_exit(&camera_int.fd);
			while(camera_init(&camera_int) < 0)
			{
				sleep(3);
				printf("[ %s ] line #%d\n", __FUNCTION__, __LINE__);
			}
		}
		sprintf(filename, "./my_%d.yuv", i); // 小心越界
		FILE *fp=fopen(filename, "w");
		fwrite(_data, ret, 1, fp);
		fclose(fp);
	    fprintf(stdout, ".");
	}
	fprintf(stderr, "\n");
	sync();

	camera_exit(&camera_int.fd);
	return 0;
}

int main(int argc, char* argv[])
{
	take_photo();
	encode_video("output.jpg", "libx264");
	return 0;
}