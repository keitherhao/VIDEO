#ifndef __LOG_H__
#define __LOG_H__

#ifndef MY_LOG_ON
#define MY_LOG_ON	1
#endif

#if MY_LOG_ON
#define DBG_PRINTF(fmt, args...)  \
do\
{\
    printf("<<File:%s  Line:%d  Function:%s>> ", __FILE__, __LINE__, __FUNCTION__);\
    printf(fmt, ##args);\
}while(0)
#else
#define DBG_PRINTF(fmt, args...)	do{}while(0)
#endif

#endif