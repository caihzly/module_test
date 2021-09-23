#指定交叉编译工具链

#IMX6
CROSS_COMPILE = /media/disk1/zhengqichezai/chz/rv1126_sdk/prebuilts/gcc/linux-x86/arm/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-


AS		= $(CROSS_COMPILE)as
#链接工具
LD		= $(CROSS_COMPILE)ld
#编译工具
CC		= $(CROSS_COMPILE)gcc-8.3.0
CPP		= $(CC) -E
#打包工具
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
#优化工具
STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump

#将定义的变量导出，方便其他Makefile使用
export AS LD CC CPP AR NM
export STRIP OBJCOPY OBJDUMP

TOPDIR := $(shell pwd)
export TOPDIR

#指定编译器参数  Wall: 提示所有警告  O2: 优化等级  g: 调试
CFLAGS := -Wall -O2 -g -pthread
#指定编译时包含头文件
CFLAGS += -I $(TOPDIR)/include


#指定编译器动态链接库
LDFLAGS := -pthread

#将定义的变量导出，方便其他Makefile使用
export CFLAGS LDFLAGS


#指定链接后的文件名
TARGET := iotest

#指定根文件系统调试路径
#TARGET_DIR := /share/imx6q-c/filesystem/qt4/rootfs/driver-test

#顶层要生成的.o文件以及顶层文件夹
obj-y += main.o

all : 
#进入当前目录，使用顶层的makefile.build进行编译
	make -C ./ -f $(TOPDIR)/Makefile.build
#将编译好的built-in.o文件链接生成我们的目标文件
	$(CC) $(LDFLAGS) -o $(TARGET) built-in.o

#顶层的清除规则
clean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.d")
	rm -f $(TARGET)

#拷贝文件到根文件系统调试路径
install:
	cp -f $(TARGET) $(DES_PATH)
	
.PHONY:all clean intall
