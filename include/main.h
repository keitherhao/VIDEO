#ifndef __MAIN_H__
#define __MAIN_H__

// #define V4L2_CAP_VIDEO_CAPTURE 0x00000001

#define DBG_PRINTF(fmt, args...)  \
do\
{\
    printf("<<File:%s  Line:%d  Function:%s>> ", __FILE__, __LINE__, __FUNCTION__);\
    printf(fmt, ##args);\
}while(0)


// struct v4l2_capability
// {
//     unsigned char driver[16];       // 驱动名字
//     unsigned char card[32];         // 设备名字
//     unsigned char bus_info[32];     // 设备在系统中的位置
//     unsigned int version;         // 驱动版本号
//     unsigned int capabilities;    // 设备支持的操作
//     unsigned int reserved[4];     // 保留字段
// };


#endif