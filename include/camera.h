#ifndef __CAMERA_H__
#define __CAMERA_H__
#include <linux/videodev2.h>

typedef struct CAMERA_INIT{
    char* dev_naem; // 设备节点
    int fd;
    int type;
	struct v4l2_format vfmt; //采集格式
    // 内核
    struct v4l2_requestbuffers reqbuffer; // 申请内核空间
    // 用户空间 映射
    unsigned char *mptr;//保存映射后用户空间的首地址
    unsigned int  size;
	struct v4l2_buffer mapbuffer;
}CAMERA_INIT;

int camera_init(CAMERA_INIT* parameter);
int camera_get_a_frame(int* pfd, unsigned char** dest);
int camera_exit(int* pfd);
#endif