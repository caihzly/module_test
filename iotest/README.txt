http://www.cnblogs.com/lidabo/p/4521123.html

#����һ��PHONY����
PHONY := build
#��ͷ˵��buildαĿ�꣬ʹ���ΪMakefile.build�ĵ�һ��Ŀ��
build :
#���嵱ǰĿ¼��Ŀ���������ʼֵΪ��
obj-y :=
#���嵱ǰĿ¼����Ŀ¼��������ʼֵΪ��
subdir-y :=
#����ǰĿ¼��Makefile��������
include Makefile
#ɸѡ����ǰĿ¼��Ŀ������е���Ŀ¼������ȥ��/
__subdir-y	:= $(patsubst %/,%,$(filter %/, $(obj-y)))
#����ʼ�����subdir-y��ֵΪ__subdir-y
subdir-y	+= $(__subdir-y)
#����subdir-y�����ÿһ��ֵ(Ŀ¼),����һ����Ӧ��Ŀ¼/built-in.o�ı���ֵ
subdir_objs := $(foreach f,$(subdir-y),$(f)/built-in.o)
#�õ�obj-y�е�.o�ļ�
cur_objs := $(filter-out %/, $(obj-y))
#�������е�.o�ļ����������������ļ���

���̱ر�:

����Makefile ����Makefile.build ��Ŀ¼Makefile

�������:

    �Ӷ��㿪ʼ�ݹ������Ŀ¼�������뵽һ��Ŀ¼����ײ�ʱ����ʼʹ��GCC���룬�ٽ��ò������.o�ļ������build-in.o������������һ��Ŀ¼�ٵݹ������Ŀ¼��
    �����������е���Ŀ¼�󣬾Ϳ�ʼ���붥���.c�ļ�����󽫶����.o�ļ��Ͷ���ÿ����Ŀ¼��build-in.o���ӳ����ǵ�Ŀ���ļ�
	
����1��
�ڹ����ļ����Ҳ�����̬�⣬��Ҫ����linux�����¡�

����2��
��parse_jpeg.c�еĺ���int parse_jpeg(uint32 start_x, uint32 start_y, const char *p_pathname)
�ڽ�ѹͼƬʱ�ǰ������н��еģ�ÿ��ѹһ�У�Ҫ�����ݱ��浽һ��������(p_jpegdata)�С�

  while (cinfo.output_scanline < cinfo.output_height) {
    (void) jpeg_read_scanlines(&cinfo, buffer, 1);
    memcpy(p_jpegdata + (cinfo.output_scanline -1) * row_stride, buffer[0], row_stride);//Add by caihz
   // put_scanline_someplace(buffer[0], row_stride);
  }
 
����3��
����readdir(...)����ֵ����DT_UNKNOWN
�������������ԭ���ǣ��ļ�û�б�ʶ�𵽣���Ϊ���Գ�������NFS�ļ�ϵͳ�µģ�
                      �����ļ����͵ķ���ֵd_typeΪDT_UNKNOWN��
					  ��Btrfs, ext2, ext3,ext4�ļ�ϵͳ�¾Ͳ������������������
����취:
(1)�����ļ�ϵͳ������ext2��ʽ��¼����������е���
(2)����lstat���������int lstat(const char *path, struct stat *buf);

