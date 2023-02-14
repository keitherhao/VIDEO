ifneq ($(KERNELRELEASE),)
obj-m := hello.o
# 其他依赖的*.c
# demo-objs := *.o
# obj-m += *.o
ARCH := arm
CROSS_COMPILE := arm-linux-gnueabihf-
# CC := arm-linux-gnueabihf-gcc
else
# $(info "2nd")
obj-m:=hello.o
KDIR := /home/keith/code/SSD202D/TAKOYAKI_DLC00V030/source_code/kernel
PWD:=$(shell pwd)
all:
	# $(info "1st")
	make -C $(KDIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
	arm-linux-gnueabihf-gcc main.c
	# sudo cp hello.ko a.out /rootfs/test/
clean:
	rm -f *.ko *.o *.symvers *.mod.c *.mod.o *.order
endif