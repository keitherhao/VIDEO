# uvcvideo-objs  := uvc_driver.o uvc_queue.o uvc_v4l2.o uvc_video.o uvc_ctrl.o \
# 		  uvc_status.o uvc_isight.o uvc_debugfs.o
# ifeq ($(CONFIG_MEDIA_CONTROLLER),y)
# uvcvideo-objs  += uvc_entity.o
# endif
# obj-$(CONFIG_USB_VIDEO_CLASS) += uvcvideo.o

ARCH = arm
CROSS_COMPILE = arm-linux-gnueabihf-
CC = $(CROSS_COMPILE)gcc
# 共享到sub-Makefile
export  ARCH  CROSS_COMPILE

# 内核编译后路径
KDIR := /home/keith/code/SSD202D/TAKOYAKI_DLC00V030/source_code/kernel
PWD := $(shell pwd)

# 注意:驱动目标不要和文件名相同
TARGET_DRV := uvc_device_driver
# 本次整个编译需要源文件 和 目录
$(TARGET_DRV)-y += uvc_driver.o uvc_queue.o uvc_v4l2.o uvc_video.o uvc_ctrl.o uvc_status.o uvc_isight.o uvc_debugfs.o
# obj-y：编译进kernel obj-m：编译为驱动 obj-n：不编译
obj-m := $(TARGET_DRV).o
# obj-m := $(TARGET).o 是告诉 makefile 最总的编译目标
# $(TARGET)-y 则是告诉 makefile 该总目标依赖哪些目标文件（固定格式，如总目标为 xxx.o，那么它依赖的源文件应该这样指定 xxx-y += ）

# 编译条件处理 如果有自定义头文件*.h的路径
# ccflags-y := -I$(PWD)/include

# -C 工作路径 M= 模块源码路径
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	rm -rf *.o *.symvers *.mod.c *.mod.o *.order .*.o.cmd .*.ko.cmd .*.o.d .tmp_versions
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
