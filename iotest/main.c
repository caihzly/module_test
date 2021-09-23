#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#define CMD_GET_GPIO_V			(0xA0)
#define CMD_SET_GPIO_V			(0xA1)
#define CMD_FREE_GPIO 			(0xA2)

enum gpio_para_item
{
	GPIO_PARA_SATA_EN = 0, 			//GPIO0-A4 : 硬盘功能使能
	GPIO_PARA_LPHOUT_CS,			//GPIO0-A5 : 对讲功放使能
	GPIO_PARA_SD0_EN,				//GPIO0-C1 : SD卡0电源使能
	GPIO_PARA_RESET_HUB,			//GPIO0-C2 : HUB芯片复位
	GPIO_PARA_LED2,					//GPIO1-C0 : 前面板硬盘指示灯
	GPIO_PARA_WIFI_CRL,				//GPIO1-C1 : WIFI电源控制
	GPIO_PARA_ADS_DMS_VOICE_EN,		//GPIO1-C4 : 主动安全报警功放使能
	GPIO_PARA_RT4812_EN,			//GPIO1-C5 : ADAS/DSM摄像头电源控制
	GPIO_PARA_VCC_12V_EN,			//GPIO1-C6 : 3-8路摄像头电源使能
	GPIO_PARA_LED1,					//GPIO1-C7 : RV1126工作指示灯
	GPIO_PARA_SD1_EN,				//GPIO1-D1 : SD卡1电源使能
	GPIO_PARA_MC_DJ,				//GPIO1-D2 : 手唛检测
	GPIO_PARA_SPK_DJ_INT,			//GPIO1-D3 : 喇叭检测
	GPIO_PARA_ITEM_BUTT,
};

struct user_data
{
	enum gpio_para_item gpio_item;
//	int gpio_num;	//maybe unused
	int value;
};

int get_gpio_value(int fd, enum gpio_para_item user_gpio_item)
{
    struct user_data userdata;
	
    memset(&userdata, 0, sizeof(struct user_data));
	userdata.gpio_item = user_gpio_item;
	
    ioctl(fd, CMD_GET_GPIO_V, &userdata);
	
	printf("[%d] -> [%d]\n", userdata.gpio_item, userdata.value);
	
	return userdata.value;
}

int set_gpio_value(int fd, enum gpio_para_item user_gpio_item, int value) 
{
    int ret = -1;
    struct user_data userdata;
	
    memset(&userdata, 0, sizeof(struct user_data));
	userdata.gpio_item = user_gpio_item;
    userdata.value = value;

    ret = ioctl(fd, CMD_SET_GPIO_V, &userdata);
	
	printf("[%d] -> [%d]\n", userdata.gpio_item, userdata.value);

    return ret;
}

int release_gpio(int fd, enum gpio_para_item user_gpio_item)
{
    int ret = -1;
    struct user_data userdata;
	
    memset(&userdata, 0, sizeof(struct user_data));
	userdata.gpio_item = user_gpio_item;
	
    ret = ioctl(fd, CMD_FREE_GPIO, &userdata);
	
    return ret;
}

int main(int argc, char *argv[])
{	
	int fd_gpio_dev = -1;
	enum gpio_para_item user_gpio_item = GPIO_PARA_ITEM_BUTT;
	int value = 0;
	
	fd_gpio_dev = open("/dev/gpio_para", O_RDWR);
	if (fd_gpio_dev < 0)
	{
		printf("line : %d, open /dev/gpio_para fail\n", __LINE__);
		return -1;
	}
	
	if (argc == 2)
	{
		user_gpio_item = atoi(argv[1]);
		value = get_gpio_value(fd_gpio_dev, user_gpio_item);
	}
	else if (argc == 3)
	{
		user_gpio_item = atoi(argv[1]);
		value = atoi(argv[2]);
		set_gpio_value(fd_gpio_dev, user_gpio_item, value);
	}
	else if (argc == 4)
	{
		user_gpio_item = atoi(argv[1]);
		if (0 == atoi(argv[2]) && 0 == strncmp("free", argv[3], 4))
		{
			release_gpio(fd_gpio_dev, user_gpio_item);
		}
	}
	else
	{
		printf("usage:\n");
		printf("get gpio value: ./iotest gpio_para_item\n");
		printf("set gpio value: ./iotest gpio_para_item value\n");
		printf("free gpio:      ./iotest gpio_para_item 0 free\n");
	}
	
	close(fd_gpio_dev);
	
	return 0;
}