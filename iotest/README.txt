http://www.cnblogs.com/lidabo/p/4521123.html

#定义一个PHONY变量
PHONY := build
#开头说明build伪目标，使其成为Makefile.build的第一个目标
build :
#定义当前目录的目标变量，初始值为空
obj-y :=
#定义当前目录的子目录变量，初始值为空
subdir-y :=
#将当前目录的Makefile包含进来
include Makefile
#筛选出当前目录的目标变量中的子目录，并且去掉/
__subdir-y	:= $(patsubst %/,%,$(filter %/, $(obj-y)))
#将开始定义的subdir-y赋值为__subdir-y
subdir-y	+= $(__subdir-y)
#对于subdir-y里面的每一个值(目录),增加一个相应的目录/built-in.o的变量值
subdir_objs := $(foreach f,$(subdir-y),$(f)/built-in.o)
#得到obj-y中的.o文件
cur_objs := $(filter-out %/, $(obj-y))
#对于所有的.o文件，定义它的依赖文件名

工程必备:

顶层Makefile 顶层Makefile.build 子目录Makefile

编译过程:

    从顶层开始递归进入子目录，当进入到一个目录的最底层时，开始使用GCC编译，再将该层的所有.o文件打包成build-in.o，返回它的上一层目录再递归进入子目录，
    当编译完所有的子目录后，就开始编译顶层的.c文件，最后将顶层的.o文件和顶层每个子目录的build-in.o链接成我们的目标文件
	
问题1：
在共享文件下找不到静态库，需要放在linux环境下。

问题2：
在parse_jpeg.c中的函数int parse_jpeg(uint32 start_x, uint32 start_y, const char *p_pathname)
在解压图片时是按照逐行进行的，每解压一行，要把数据保存到一个缓冲区(p_jpegdata)中。

  while (cinfo.output_scanline < cinfo.output_height) {
    (void) jpeg_read_scanlines(&cinfo, buffer, 1);
    memcpy(p_jpegdata + (cinfo.output_scanline -1) * row_stride, buffer[0], row_stride);//Add by caihz
   // put_scanline_someplace(buffer[0], row_stride);
  }
 
问题3：
函数readdir(...)返回值出现DT_UNKNOWN
出现这种情况的原因是：文件没有被识别到，因为调试程序是在NFS文件系统下的，
                      导致文件类型的返回值d_type为DT_UNKNOWN，
					  在Btrfs, ext2, ext3,ext4文件系统下就不会有这种情况发生。
解决办法:
(1)将根文件系统制作成ext2格式烧录到开发板进行调试
(2)采用lstat函数解决：int lstat(const char *path, struct stat *buf);

