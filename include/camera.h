#ifndef __CAMERA_H__
#define __CAMERA_H__
#include <linux/videodev2.h>

typedef struct CAMERA_INIT{
    int fd;
    struct v4l2_fmtdesc v4fmt; //获取摄像头支持的格式
    int type;
	struct v4l2_format vfmt; //采集格式
    // 内核
    struct v4l2_requestbuffers reqbuffer; // 申请内核空间
    // 用户空间 映射
    unsigned char *mptr[4];//保存映射后用户空间的首地址
    unsigned int  size[4];
	struct v4l2_buffer mapbuffer;
    // 从队列中提取一帧数据
	struct v4l2_buffer  readbuffer;
}CAMERA_INIT;

int camera_init(CAMERA_INIT* parameter);
int camera_exit(int* pfd);
#endif