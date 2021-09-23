/*
 * tp2854.c
 */
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>

#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/io.h>
//#include <asm/system.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/string.h>
#include <linux/list.h>
#include <asm/delay.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <uapi/linux/sched/types.h>
#include <linux/of_gpio.h>

#include "tp2856_def.h"
#include "tp2856.h"
#include "common.h"

extern u32 gpio_i2c_write(u8 chip, u8 reg, u8 val);
extern u8 gpio_i2c_read(u8 chip, u8 reg);
extern int reset_rx_video_mode(u8 chip, u8 video_mode);
extern int reset_rx_video_stream(void);
extern int get_rxdvp_index(void);

void tp2802_reset_init_reg(unsigned char pw_ctrl);
//static struct semaphore node_mutex;
struct mutex tp2802_mutex;

#define DEBUG             1  //printk debug information on/off

enum
{
	TP9930 = 0x3200,
	TP2855 = 0x5500,
	TP2856 = 0x5600,
};

//TP28xx audio
//both record and playback are master, 20ch,I2S mode, backend can use 16ch mode to capture.
#if 1 //Add by caihz
#define RX_MODE_I2S_SLAVE		(0)
#define RX_MODE_I2S_MASTER		(1)
#define RX_MODE					RX_MODE_I2S_MASTER		
#endif

#define I2S  				0
#define DSP  				1
#define AUDIO_FORMAT   		I2S

#define SAMPLE_8K    		0
#define SAMPLE_16K   		1
#define SAMPLE_RATE  		SAMPLE_8K

#define DATA_16BIT  		0
#define DATA_8BIT   		1
#define DATA_BIT    		DATA_16BIT

#define AUDIO_CHN   		8
#define DEFAULT_FORMAT      TP2802_1080P25

//static int audio_format = AUDIO_FORMAT;
//static int audio_channels = AUDIO_CHN;
static int mode = DEFAULT_FORMAT;
static int chips = 2;
static int output = MIPI_4CH4LANE_594M;

static unsigned int id[MAX_CHIPS];

void set_chips_cnt(int ad_cnt)
{
	chips = ad_cnt;
}

#define TP2856_I2C_A  		0x44
#define TP2856_I2C_B  		0x47

unsigned char tp2856_i2c_addr[] = 
{
    TP2856_I2C_A,
    TP2856_I2C_B,
};

typedef struct
{
    unsigned int			count[CHANNELS_PER_CHIP];
    unsigned int				  mode[CHANNELS_PER_CHIP];
    unsigned int               scan[CHANNELS_PER_CHIP];
    unsigned int				  gain[CHANNELS_PER_CHIP][4];
    unsigned int               std[CHANNELS_PER_CHIP];
    unsigned int                 state[CHANNELS_PER_CHIP];
    unsigned int                 force[CHANNELS_PER_CHIP];
    unsigned char addr;
} tp2802wd_info;

static const unsigned char SYS_MODE[5]= {0x01,0x02,0x04,0x08,0x0f}; //{ch1,ch2,ch3,ch4,ch_all}
static const unsigned char SYS_AND[5] = {0xfe,0xfd,0xfb,0xf7,0xf0};

//static void TP2854_PTZ_mode(unsigned char, unsigned char, unsigned char);
static void TP2854_RX_init(unsigned char chip, unsigned char mod);
static void TP2856_output(unsigned char chip,unsigned char port);
static int tp2802_set_video_mode(unsigned char addr, unsigned char mode, unsigned char ch, unsigned char std);
static void tp2802_set_reg_page(unsigned char addr, unsigned char ch);


unsigned char ReverseByte(unsigned char dat)
{

    static const unsigned char BitReverseTable256[] =
    {
    0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
    0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
    0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
    0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
    0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
    0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
    0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
    0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
    0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
    0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
    0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
    0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
    0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
    0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
    0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
    0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
    };
    return BitReverseTable256[dat];
}
/////////////////////////////////////////////////////////////////////
unsigned char PELCO_Decode(unsigned char *buf)
{
	unsigned char i, j;
	unsigned char cnt_high = 0;
	unsigned char cnt_low = 0;
	unsigned char dat = 0;
    unsigned char cnt_bit = 0;

	for(j = 0; j < 6; j++ )
	{
				for(i = 0; i <8; i++ )
				{
					if(0x80&buf[j])
					{
						cnt_high++;
						cnt_low = 0;
					}
					else
					{
						if(0 == cnt_low)
						{
                            if(cnt_bit < 8)
                            {
                                dat <<= 1;
                                if(cnt_high > 2) dat |= 0x01;
                                cnt_high = 0;
                                cnt_bit++;
                            }
                            else break;
						}
						cnt_low++;
					}
					buf[j] <<= 1;

				}
		}
		if( (0 == cnt_low) && (cnt_high > 2) && (cnt_bit < 8))
        {
                dat <<= 1;
                dat |= 0x01;
        }
		return dat;
}
///////////////////////////////////////////////////////
unsigned int ConvertACPV1Data(unsigned char dat)
{
    unsigned int i, tmp=0;
    for(i = 0; i < 8; i++)
    {
        tmp <<= 3;

        if(0x01 & dat) tmp |= 0x06;
        else tmp |= 0x04;

        dat >>= 1;
    }
    return tmp;
}

void tp28xx_byte_write(unsigned char chip, unsigned char reg_addr, unsigned char value)
{
	gpio_i2c_write(chip, reg_addr, value);
}

unsigned char tp28xx_byte_read(unsigned char chip, unsigned char reg_addr)
{
	return gpio_i2c_read(chip, reg_addr);
}

static void tp2802_write_table(unsigned char chip,
                               unsigned char addr, unsigned char *tbl_ptr, unsigned char tbl_cnt)
{
    unsigned char i = 0;
    for(i = 0; i < tbl_cnt; i ++)
    {
        tp28xx_byte_write(chip, (addr + i), *(tbl_ptr + i));
    }
}

void TP2854_StartTX(unsigned char chip, unsigned char ch)
{
	unsigned char tmp;
	int i;
	tp28xx_byte_write(chip, 0xB6, (0x01<<(ch))); //clear flag

	tmp = tp28xx_byte_read(chip, 0x6f);
    tmp |= 0x01;
    tp28xx_byte_write(chip, 0x6f, tmp); //TX enable
    tmp &= 0xfe;
    tp28xx_byte_write(chip, 0x6f, tmp); //TX disable

    i = 100;
    while(i--)
    {
        if( (0x01<<(ch)) & tp28xx_byte_read(chip, 0xb6)) break;
        //udelay(1000);
        msleep(2);
    }
}

void HDA_SetACPV2Data(unsigned char chip, unsigned char reg,unsigned char dat)
{
    unsigned int i;
	unsigned int PTZ_pelco=0;

    for(i = 0; i < 8; i++)
    {
        PTZ_pelco <<= 3;

        if(0x80 & dat) PTZ_pelco |= 0x06;
        else PTZ_pelco |= 0x04;

        dat <<= 1;
    }

    tp28xx_byte_write(chip, reg + 0 , (PTZ_pelco>>16)&0xff);
    tp28xx_byte_write(chip, reg + 1 , (PTZ_pelco>>8)&0xff);
    tp28xx_byte_write(chip, reg + 2 , (PTZ_pelco)&0xff);
}
void HDA_SetACPV1Data(unsigned char chip, unsigned char reg,unsigned char dat)
{

    unsigned int i;
	unsigned int PTZ_pelco=0;

    for(i = 0; i < 8; i++)
    {
        PTZ_pelco <<= 3;

        if(0x01 & dat) PTZ_pelco |= 0x06;
        else PTZ_pelco |= 0x04;

        dat >>= 1;
    }

    tp28xx_byte_write(chip, reg + 0 , (PTZ_pelco>>16)&0xff);
    tp28xx_byte_write(chip, reg + 1 , (PTZ_pelco>>8)&0xff);
    tp28xx_byte_write(chip, reg + 2 , (PTZ_pelco)&0xff);

}

int tp2802_open(struct inode * inode, struct file * file) 
{
#if 0
	if (!down_trylock(&node_mutex)) //尝试加锁，非阻塞，成功返回0，失败返回非0
	{
		return 0;
	}
#endif

	return 0; 
}
  
int tp2802_close(struct inode * inode, struct file * file)
{
//	up(&node_mutex);

    return 0;
}

static void tp2802_set_work_mode_1080p25(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_1080p25_raster, 9);
}

static void tp2802_set_work_mode_1080p30(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_1080p30_raster, 9);
}

static void tp2802_set_work_mode_720p25(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_720p25_raster, 9);
}

static void tp2802_set_work_mode_720p30(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_720p30_raster, 9);
}

static void tp2802_set_work_mode_720p50(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_720p50_raster, 9);
}

static void tp2802_set_work_mode_720p60(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_720p60_raster, 9);
}

static void tp2802_set_work_mode_PAL(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_PAL_raster, 9);
}

static void tp2802_set_work_mode_NTSC(unsigned chip)
{
    // Start address 0x15, Size = 9B
    tp2802_write_table(chip, 0x15, tbl_tp2802_NTSC_raster, 9);
}

//#define AMEND

static int tp2856_audio_config_rmpos(unsigned chip, unsigned format, unsigned chn_num)
{
    int i = 0;

    //clear first
    for (i=0; i<20; i++)
    {
        tp28xx_byte_write(chip, i, 0);
    }

    switch(chn_num)
    {
    case 2:
        if (format)
        {
            tp28xx_byte_write(chip, 0x0, 1);
            tp28xx_byte_write(chip, 0x1, 2);
        }
        else
        {
            tp28xx_byte_write(chip, 0x0, 1);
            tp28xx_byte_write(chip, 0x8, 2);
        }

        break;
    case 4:
        if (format)
        {
            tp28xx_byte_write(chip, 0x0, 1);
            tp28xx_byte_write(chip, 0x1, 2);
            tp28xx_byte_write(chip, 0x2, 3);
            tp28xx_byte_write(chip, 0x3, 4);/**/
        }
        else
        {
            tp28xx_byte_write(chip, 0x0, 1);
            tp28xx_byte_write(chip, 0x1, 2);
            tp28xx_byte_write(chip, 0x8, 3);
            tp28xx_byte_write(chip, 0x9, 4);/**/
        }

        break;

    case 8:
        if (0 == chip%4)
        {
            if (format)
            {
                tp28xx_byte_write(chip, 0x0, 1);
                tp28xx_byte_write(chip, 0x1, 2);
                tp28xx_byte_write(chip, 0x2, 3);
                tp28xx_byte_write(chip, 0x3, 4);/**/
                tp28xx_byte_write(chip, 0x4, 5);
                tp28xx_byte_write(chip, 0x5, 6);
                tp28xx_byte_write(chip, 0x6, 7);
                tp28xx_byte_write(chip, 0x7, 8);/**/
            }
            else
            {
                tp28xx_byte_write(chip, 0x0, 1);
                tp28xx_byte_write(chip, 0x1, 2);
                tp28xx_byte_write(chip, 0x2, 3);
                tp28xx_byte_write(chip, 0x3, 4);/**/
                tp28xx_byte_write(chip, 0x8, 5);
                tp28xx_byte_write(chip, 0x9, 6);
                tp28xx_byte_write(chip, 0xa, 7);
                tp28xx_byte_write(chip, 0xb, 8);/**/
            }
        }
        else if (1 == chip%4)
        {
            if (format)
            {
                tp28xx_byte_write(chip, 0x0, 0);
                tp28xx_byte_write(chip, 0x1, 0);
                tp28xx_byte_write(chip, 0x2, 0);
                tp28xx_byte_write(chip, 0x3, 0);
                tp28xx_byte_write(chip, 0x4, 1);
                tp28xx_byte_write(chip, 0x5, 2);
                tp28xx_byte_write(chip, 0x6, 3);
                tp28xx_byte_write(chip, 0x7, 4);/**/
            }
            else
            {
                tp28xx_byte_write(chip, 0x0, 0);
                tp28xx_byte_write(chip, 0x1, 0);
                tp28xx_byte_write(chip, 0x2, 1);
                tp28xx_byte_write(chip, 0x3, 2);
                tp28xx_byte_write(chip, 0x8, 0);
                tp28xx_byte_write(chip, 0x9, 0);
                tp28xx_byte_write(chip, 0xa, 3);
                tp28xx_byte_write(chip, 0xb, 4);/**/
            }
        }


        break;

    case 16:
        if (0 == chip%4)
        {
            for (i=0; i<16; i++)
            {
                tp28xx_byte_write(chip, i, i+1);
            }
        }
        else if (1 == chip%4)
        {
            for (i=4; i<16; i++)
            {
                tp28xx_byte_write(chip, i, i+1 -4);
            }
        }
        else if	(2 == chip%4)
        {
            for (i=8; i<16; i++)
            {

                tp28xx_byte_write(chip, i, i+1 - 8);

            }
        }
        else
        {
            for (i=12; i<16; i++)
            {
                tp28xx_byte_write(chip, i, i+1 - 12);

            }
        }
        break;

    case 20:
        for (i=0; i<20; i++)
        {
            tp28xx_byte_write(chip, i, i+1);
        }
        break;

    default:
        for (i=0; i<20; i++)
        {
            tp28xx_byte_write(chip, i, i+1);
        }
        break;
    }

    mdelay(10);
    return 0;
}

#if 1 //Add by caihz
static int tp2856_set_audio_rm_format(unsigned chip, tp2833_audio_format *pstFormat)
{
    unsigned char temp = 0;

	if (NULL == pstFormat)
	{
		return FAILURE;
	}
	
    if (pstFormat->mode > 1 || pstFormat->format> 1 || pstFormat->bitrate> 1 || pstFormat->clkdir> 1 || pstFormat->precision > 1)
    {
        return FAILURE;
    }

    temp = tp28xx_byte_read(chip, 0x17);
    temp &= 0xf2;
    temp |= pstFormat->format;
    temp |= pstFormat->precision << 2;
    //dsp
    if (pstFormat->format)
    {
        temp |= 1 << 3;
    }
    tp28xx_byte_write(chip, 0x17, temp);

    temp = tp28xx_byte_read(chip, 0x18);
    temp &= 0xef;
    temp |= pstFormat->bitrate << 4;
    tp28xx_byte_write(chip, 0x18, temp);

    temp = tp28xx_byte_read(chip, 0x19);
    temp &= 0xdc;
    if (pstFormat->mode == 0)
    {
        //slave mode
        temp |= 1 << 5;
    }
    else
    {
        //master mode
        temp |= 0x3;
    }
    //Notice:
    temp |= pstFormat->bitrate << 4;
    tp28xx_byte_write(chip, 0x19, temp);

    tp2856_audio_config_rmpos(chip, pstFormat->format, pstFormat->chn);

    return SUCCESS;
}

static int tp2856_set_audio_pb_format(unsigned chip, tp2833_audio_format *pstFormat)
{
    unsigned char temp = 0;

	if (NULL == pstFormat)
	{
		return FAILURE;
	}
	
    if (pstFormat->mode > 1 || pstFormat->format> 1 || pstFormat->bitrate> 1 || pstFormat->clkdir> 1 || pstFormat->precision > 1)
    {
        return FAILURE;
    }

    temp = tp28xx_byte_read(chip, 0x1b);
    temp &= 0xb2;
    temp |= pstFormat->mode;
    temp |= pstFormat->format << 2;
    //dsp
    if (pstFormat->format)
    {
        temp |= 1 << 3;
    }
    temp |= pstFormat->precision << 6;

    tp28xx_byte_write(chip, 0x1b, temp);

    return SUCCESS;
}
#endif

long tp2802_ioctl(struct file* file, unsigned int cmd, unsigned long arg)
{
	struct _videorx_video_adjust *video_para = NULL;
	struct backend_frmcnt *backend_frmcnt_para = NULL;
	struct videorx_support_input *support_input = NULL;
	struct _VIDEORX_INFORMATION_S *video_info = NULL;
	struct videorx_video_out_delay *video_out_delay = NULL;
	struct _tp2802_video_mode *video_mode = NULL;
	struct _tp2833_audio_format *audio_format = NULL;
	int index = 0;
	unsigned char tmp = 0;
	unsigned char chip = 0;
	unsigned char ch_index = 0;  
	unsigned char ch_cnt = 0;
	long ret = 0;
	
	mutex_lock(&tp2802_mutex);

	if ((cmd == IOC_VDEC_SET_PB_VOL) || (cmd == IOC_VDEC_SET_REC_VOL) || (cmd == IOC_VDEC_SET_BRIGHTNESS) || (cmd == IOC_VDEC_SET_CONTRAST)\
	|| (cmd == IOC_VDEC_SET_HUE) || (cmd == IOC_VDEC_SET_SATURATION) || (cmd == IOC_VDEC_SET_SHARPNESS))
	{
		video_para = kzalloc(sizeof(struct _videorx_video_adjust), GFP_KERNEL);
		if (NULL == video_para)
		{
			return -EFAULT;
		}

		memset(video_para, 0, sizeof(struct _videorx_video_adjust));

		if (copy_from_user(video_para, (struct msg __user *)arg, sizeof(struct _videorx_video_adjust)))
		{
			ret = -EFAULT;
			goto out;
		}

		chip = video_para->ch / CHANNELS_PER_CHIP;
		ch_index = video_para->ch % CHANNELS_PER_CHIP;

		if (chip >= chips)
		{
			ret = -EFAULT;
			goto out;
		}
	}
	else if (cmd == IOC_VDEC_GET_FRMCNT)
	{
		backend_frmcnt_para = kzalloc(sizeof(struct backend_frmcnt), GFP_KERNEL);
		if (NULL == backend_frmcnt_para)
		{
			return -EFAULT;
		}

		memset(backend_frmcnt_para, 0, sizeof(struct backend_frmcnt));

		if (copy_from_user(backend_frmcnt_para, (struct msg __user *)arg, sizeof(struct backend_frmcnt)))
		{
			ret = -EFAULT;
			goto out;
		}
	}
	else if (cmd == IOC_VDEC_GET_SPT)
	{
		support_input = kzalloc(sizeof(struct videorx_support_input), GFP_KERNEL);
		if (NULL == support_input)
		{
			return -EFAULT;
		}

		memset(support_input, 0, sizeof(struct videorx_support_input));

		if (copy_from_user(support_input, (struct msg __user *)arg, sizeof(struct videorx_support_input)))
		{
			ret = -EFAULT;
			goto out;
		}
	}
	else if (cmd == IOC_VDEC_GET_VIDEO_INFO)
	{
		video_info = kzalloc(sizeof(struct _VIDEORX_INFORMATION_S), GFP_KERNEL);
		if (NULL == video_info)
		{
			return -EFAULT;
		}

		memset(video_info, 0, sizeof(struct _VIDEORX_INFORMATION_S));

		if (copy_from_user(video_info, (struct msg __user *)arg, sizeof(struct _VIDEORX_INFORMATION_S)))
		{
			ret = -EFAULT;
			goto out;
		}

		ch_cnt = video_info->ch;
	}
	else if (cmd == IOC_VDEC_SET_VIDEO_OUT_DELAY)
	{
		video_out_delay = kzalloc(sizeof(struct videorx_video_out_delay), GFP_KERNEL);
		if (NULL == video_out_delay)
		{
			return -EFAULT;
		}

		memset(video_out_delay, 0, sizeof(struct videorx_video_out_delay));

		if (copy_from_user(video_out_delay, (struct msg __user *)arg, sizeof(struct videorx_video_out_delay)))
		{
			ret = -EFAULT;
			goto out;
		}
	}
	else if (cmd == IOC_VDEC_SET_VIDEO_MODE)
	{
		video_mode = kzalloc(sizeof(struct _tp2802_video_mode), GFP_KERNEL);
		if (NULL == video_mode)
		{
			return -EFAULT;
		}

		memset(video_mode, 0, sizeof(struct _tp2802_video_mode));

		if (copy_from_user(video_mode, (struct msg __user *)arg, sizeof(struct _tp2802_video_mode)))
		{
			ret = -EFAULT;
			goto out;
		}
	}
	else if (cmd == IOC_VDEC_GET_VIDEO_MODE)
	{
		video_mode = kzalloc(sizeof(struct _tp2802_video_mode), GFP_KERNEL);
		if (NULL == video_mode)
		{
			return -EFAULT;
		}

		memset(video_mode, 0, sizeof(struct _tp2802_video_mode));

		if (copy_from_user(video_mode, (struct msg __user *)arg, sizeof(struct _tp2802_video_mode)))
		{
			ret = -EFAULT;
			goto out;
		}
	}
	else if (cmd == IOC_VDEC_SET_AUDIO_RM_FORMAT || cmd == IOC_VDEC_SET_AUDIO_PB_FORMAT)
	{
		audio_format = kzalloc(sizeof(struct _tp2833_audio_format), GFP_KERNEL);
		if (NULL == audio_format)
		{
			return -EFAULT;
		}

		if (copy_from_user(audio_format, (struct msg __user *)arg, sizeof(struct _tp2833_audio_format)))
		{ 
			ret = -EFAULT;
			goto out;
		}

		if (audio_format->chip >= chips || audio_format->chip < 0)
		{
			ret = -EFAULT;
			goto out;
		}
	}
	else
	{}

	switch(cmd)
	{		
		case IOC_VDEC_SET_PB_VOL:
		{
            if (video_para->value > 15 || video_para->value < 0)
            {
				video_para->value = 8;
			}
			
            tp2802_set_reg_page(chip, AUDIO_PAGE); //audio page

		    tmp = tp28xx_byte_read(chip, 0x1f);
		    tmp &= 0xf0;
		    tmp |= video_para->value;
			
            tp28xx_byte_write(chip, 0x1f, tmp); //AOUT_GAIN

			break;
		}
		case IOC_VDEC_SET_REC_VOL:
		{
			if (video_para->value > 15 || video_para->value < 0)
			{
				video_para->value = 8;
			}

			tp2802_set_reg_page(chip, AUDIO_PAGE); //audio page

			tmp = tp28xx_byte_read(chip, 0x38);
		    tmp &= 0xf0;
		    tmp |= video_para->value;

			tp28xx_byte_write(chip, 0x38, tmp); //DAOGAIN

			break;
		}
		case IOC_VDEC_SET_BRIGHTNESS:
		{
			tp2802_set_reg_page(chip, ch_index);  
			
			if (video_para->value > MAX_BRIGHTNESS_V)  
			{
				video_para->value = D_BRIGHTNESS_V;
			}
			
			tp28xx_byte_write(chip, TP_BRIGHTNESS, video_para->value);
			
			break;
		}
		case IOC_VDEC_SET_CONTRAST:
		{
			tp2802_set_reg_page(chip, ch_index);

			if (video_para->value > MAX_CONTRAST_V)
			{
				video_para->value = D_CONTRAST_V;				
			}
			
			tp28xx_byte_write(chip, TP_CONTRAST, video_para->value);
			
			break;
		}
		case IOC_VDEC_SET_SATURATION:
		{
			tp2802_set_reg_page(chip, ch_index);

			if (video_para->value > MAX_SATURATION_V)
			{
				video_para->value = D_SATURATION_V;			
			}
			
			tp28xx_byte_write(chip, TP_SATURATION, video_para->value);
			
			break;
		}
		case IOC_VDEC_SET_HUE:
		{
			tp2802_set_reg_page(chip, ch_index);

			if (video_para->value > MAX_HUE_V)
			{
				video_para->value = D_HUE_V;
			}
			
			tp28xx_byte_write(chip, TP_HUE, video_para->value);
			
			break;
		}
		case IOC_VDEC_SET_SHARPNESS:
		{
			tp2802_set_reg_page(chip, ch_index);
			
			if (video_para->value > MAX_SHARPNESS_V)
			{
				video_para->value = D_SHARPNESS_V;
			}
			
		    tmp = tp28xx_byte_read(chip, TP_SHARPNESS);
		    tmp &= 0xe0;
		    tmp |= (video_para->value & 0x1F);
		    tp28xx_byte_write(chip, TP_SHARPNESS, tmp);
			
			break;
		}
		case IOC_VDEC_GET_VIDEO_LOSS:
		{
			unsigned char video_input_status = 0;
			unsigned char video_loss = 0;
			unsigned char vin_status = 0;

			ch_cnt = chips * CHANNELS_PER_CHIP;

			for (index = 0; index < ch_cnt; index++)
			{
				tp2802_set_reg_page(index / CHANNELS_PER_CHIP, index % CHANNELS_PER_CHIP);
				video_input_status = tp28xx_byte_read(index / CHANNELS_PER_CHIP, 0x01);
				if (video_input_status & 0x80)
				{
					msleep(100);
					tp2802_set_reg_page(index / CHANNELS_PER_CHIP, index % CHANNELS_PER_CHIP);
					video_input_status = tp28xx_byte_read(index / CHANNELS_PER_CHIP, 0x01);
				}

				video_loss = (video_input_status & 0x80) >> 7;
				vin_status |= (video_loss << index);
			}

			//printk(KERN_INFO "vin_status = 0x%x\n", vin_status);

			if (copy_to_user((struct msg __user *)arg, &vin_status, sizeof(unsigned char)))
			{
				ret = -EFAULT;
			}
		
			break;
		}
		case IOC_VDEC_RESET_REG: 
		{
			unsigned char pw_ctrl = 0;
			
			if (copy_from_user(&pw_ctrl, (struct msg __user *)arg, sizeof(unsigned char)))
			{
				ret = -EFAULT;
				break;
			}

			printk("IOC_VDEC_RESET_REG cmd pw_ctrl=0x%x\n", pw_ctrl);

			tp2802_reset_init_reg(pw_ctrl);

			break;  
		}
		case IOC_VDEC_GET_FRMCNT:
		{
			break;
		}
		case IOC_VDEC_GET_SPT:
		{
			support_input->ch_num = chips * CHANNELS_PER_CHIP;

			for (index = 0; index < support_input->ch_num; index++)
			{
				tp2802_set_reg_page(index / CHANNELS_PER_CHIP, index % CHANNELS_PER_CHIP);
				tmp = tp28xx_byte_read(index / CHANNELS_PER_CHIP, 0x03);
				tmp &= 0x07;

				if (tmp == INVALID_FORMAT)
				{
					msleep(100);

					tp2802_set_reg_page(index / CHANNELS_PER_CHIP, index % CHANNELS_PER_CHIP);
					tmp = tp28xx_byte_read(index / CHANNELS_PER_CHIP, 0x03);
					tmp &= 0x07;
				}

				support_input->ch_spt[index] = AHD_SUPPORT;
				if (TP2802_SD == tmp || INVALID_FORMAT == tmp)
				{
					support_input->ch_spt[index] = AHD_NOSUPPORT;
				}

				support_input->ch_reso[index] = tmp;
			}

			if (copy_to_user((struct msg __user *)arg, support_input, sizeof(struct videorx_support_input)))  
			{
				ret = -EFAULT;
			}

			break;
		}
		case IOC_VDEC_SET_VIDEO_MODE:
		{
			if(video_mode->ch >= CHANNELS_PER_CHIP)
			{
				return FAILURE;
			}
				
			//output = video_mode->output;

			ret = tp2802_set_video_mode(video_mode->chip, video_mode->mode, video_mode->ch, video_mode->std);
			TP2856_output(video_mode->chip, MIPI_PAGE);

			break;
		}
		case IOC_VDEC_GET_VIDEO_MODE:
		{
			if(video_mode->ch >= CHANNELS_PER_CHIP)
			{
				return FAILURE;
			}
				
			chip = video_mode->chip;
			tp2802_set_reg_page(chip, video_mode->ch);

			tmp = tp28xx_byte_read(chip, 0x03);
			tmp &= 0x7; /* [2:0] - CVSTD */
			video_mode->mode = tmp;

			if (copy_to_user((struct msg __user *)arg, video_mode, sizeof(struct _tp2802_video_mode)))  
			{
				ret = -EFAULT;
			}

			break;
		}
		case IOC_VDEC_SET_AUDIO_RM_FORMAT:
		{			
            for (index = 0; index < chips; index++)
            {
                tp2802_set_reg_page(index, AUDIO_PAGE);

                ret = tp2856_set_audio_rm_format(index, audio_format);
            }

            break;
		}
		case IOC_VDEC_SET_AUDIO_PB_FORMAT:
		{
			tp2802_set_reg_page(audio_format->chip, AUDIO_PAGE);
			
			ret = tp2856_set_audio_pb_format(audio_format->chip, audio_format);
		}
		case IOC_VDEC_SET_SAMPLE_RATE:
		{
			enum _tp2802_audio_samplerate samplerate;

			if (copy_from_user(&samplerate, (struct msg __user *)arg, sizeof(samplerate)))
			{
				ret = -EFAULT;
				goto out;
			}

			for (index = 0; index < chips; index++)
			{
				tp2802_set_reg_page(index, AUDIO_PAGE); //audio page
				tmp = tp28xx_byte_read(index, 0x18);
				tmp &= 0xf8;

				if(SAMPLE_RATE_16000 == samplerate) 
				{
					tmp |= 0x01;
				}

				tp28xx_byte_write(index, 0x18, tmp);
				tp28xx_byte_write(index, 0x3d, 0x01); //audio reset
			}

			break;
		}
		case IOC_VDEC_SET_AUDIO_PLAYBACK:
		{
			struct _tp2802_audio_playback audio_playback;
			if (copy_from_user(&audio_playback, (struct msg __user *)arg, sizeof(struct _tp2802_audio_playback)))
			{
				ret = -EFAULT;
				goto out;
			}

			if(audio_playback.chn > 0x19 || audio_playback.chn < 0x01)
			{
				ret = -EFAULT;
				goto out;
			}

			tp2802_set_reg_page(audio_playback.chip, AUDIO_PAGE); //audio page

			tp28xx_byte_write(audio_playback.chip, 0x1a, audio_playback.chn);

			break;
		}
		case IOC_VDEC_SET_TP9930_VIDEO_MODE:
		{
			unsigned char rxdvp_video_mode = 0; // 0: 1080p, 1: 720p
			int rxdvp_index = 0;
			
			if (copy_from_user(&rxdvp_video_mode, (struct msg __user *)arg, sizeof(unsigned char)))
			{
				ret = -EFAULT;
				break;
			}

			rxdvp_index = get_rxdvp_index();

			reset_rx_video_mode(rxdvp_index, !!rxdvp_video_mode);
			
			break;
		}
		default:
		{
			break;
		}
	}
	
out:
	if (video_para != NULL)
	{
		kfree(video_para);
	}

	if (backend_frmcnt_para != NULL)
	{
		kfree(backend_frmcnt_para);
	}

	if (support_input != NULL)
	{
		kfree(support_input);
	}

	if (video_info != NULL)
	{
		kfree(video_info);
	}

	if (video_out_delay != NULL)
	{
		kfree(video_out_delay);
	}

	if (video_mode != NULL)
	{
		kfree(video_mode);
	}

	if (audio_format != NULL)
	{
		kfree(audio_format);
	}

	mutex_unlock(&tp2802_mutex);

	return ret;
}

//////////////////////////////////////////////////////////////////////////////
static void TP2856_Audio_DataSet(unsigned char chip)
{

    unsigned int bank;

    bank = tp28xx_byte_read(chip, 0x40);
    tp28xx_byte_write(chip, 0x40, 0x40);

    tp2856_audio_config_rmpos(chip, AUDIO_FORMAT , AUDIO_CHN);

    tp28xx_byte_write(chip, 0x17, 0x00|(DATA_BIT<<2));
    tp28xx_byte_write(chip, 0x1B, 0x01|(DATA_BIT<<6));

#if(AUDIO_CHN == 20)
    tp28xx_byte_write(chip, 0x18, 0xd0|(SAMPLE_RATE));
#else
    tp28xx_byte_write(chip, 0x18, 0xc0|(SAMPLE_RATE));
#endif

#if(AUDIO_CHN >= 8)
    tp28xx_byte_write(chip, 0x19, 0x1F);
#else
    tp28xx_byte_write(chip, 0x19, 0x0F);
#endif

    tp28xx_byte_write(chip, 0x1A, 0x15);

    tp28xx_byte_write(chip, 0x37, 0x20);
    tp28xx_byte_write(chip, 0x38, 0x38);
    tp28xx_byte_write(chip, 0x3E, 0x00);
    tp28xx_byte_write(chip, 0x7a, 0x25);

    tp28xx_byte_write(chip, 0x3d, 0x01);//audio reset

    tp28xx_byte_write(chip, 0x40, bank);

}
static void TP2854_RX_init(unsigned char chip, unsigned char mod)
{

    int i, index=0;
    unsigned char regA7=0x00;
    unsigned char regA8=0x00;

    //regC9~D7
    static const unsigned char PTZ_RX_dat[][15]=
    {
        {0x00,0x00,0x07,0x08,0x00,0x00,0x04,0x00,0x00,0x60,0x10,0x06,0xbe,0x39,0x27}, //TVI command
        {0x00,0x00,0x07,0x08,0x09,0x0a,0x04,0x00,0x00,0x60,0x10,0x06,0xbe,0x39,0x27}, //TVI burst
        {0x00,0x00,0x06,0x07,0x08,0x09,0x05,0xbf,0x11,0x60,0x0b,0x04,0xf0,0xd8,0x2f}, //ACP1 0.525
        {0x00,0x00,0x06,0x07,0x08,0x09,0x02,0xdf,0x88,0x60,0x10,0x04,0xf0,0xd8,0x17}, //ACP2 0.6
        //{0x00,0x00,0x06,0x07,0x08,0x09,0x04,0xec,0xe9,0x60,0x10,0x04,0xf0,0xd8,0x17}, //ACP3 0.35
        {0x00,0x00,0x07,0x08,0x09,0x0a,0x09,0xd9,0xd3,0x60,0x08,0x04,0xf0,0xd8,0x2f}, //ACP3 0.35
        {0x00,0x00,0x06,0x07,0x08,0x09,0x03,0x52,0x53,0x60,0x10,0x04,0xf0,0xd8,0x17}  //ACP1 0.525
    };

        if(PTZ_RX_TVI_CMD == mod)
        {
            index = 0;
            regA7 = 0x03;
            regA8 = 0x00;
        }
        else if(PTZ_RX_TVI_BURST == mod)
        {
            index = 1;
            regA7 = 0x03;
            regA8 = 0x00;
        }
        else if(PTZ_RX_ACP1 == mod)
        {
            index = 2;
            regA7 = 0x03;
            regA8 = 0x00;
        }
        else if(PTZ_RX_ACP2 == mod)
        {
            index = 3;
            regA7 = 0x27;
            regA8 = 0x0f;
        }
        else if(PTZ_RX_ACP3 == mod)
        {
            index = 4;
            regA7 = 0x03;
            regA8 = 0x00;
        }
        else if(PTZ_RX_TEST == mod)
        {
            index = 5;
            regA7 = 0x03;
            regA8 = 0x00;
        }

        for(i = 0; i < 15; i++)
        {
            tp28xx_byte_write(chip, 0xc9+i, PTZ_RX_dat[index][i]);
            tp28xx_byte_write(chip, 0xa8, regA8);
            tp28xx_byte_write(chip, 0xa7, regA7);
        }

}

static void TP2856_reset_default(unsigned char chip, unsigned char ch)
{
    unsigned int tmp;

    tp28xx_byte_write(chip, 0x07, 0xC0);
    tp28xx_byte_write(chip, 0x0B, 0xC0);
    tmp = tp28xx_byte_read(chip, 0x26);
    tmp &= 0xfe;
    tp28xx_byte_write(chip, 0x26, tmp);
    tmp = tp28xx_byte_read(chip, 0xa7);
    tmp &= 0xfe;
    tp28xx_byte_write(chip, 0xa7, tmp);
    tmp = tp28xx_byte_read(chip, 0x06);
    tmp &= 0xfb;
    tp28xx_byte_write(chip, 0x06, tmp);
    tp28xx_byte_write(chip, 0x45, 0x01);
    tp28xx_byte_write(chip, 0x73, 0x26);
    tp28xx_byte_write(chip, 0x80, 0x50);
    tp28xx_byte_write(chip, 0x85, 0x07);
    tp28xx_byte_write(chip, 0x86, 0x0f);
    tp28xx_byte_write(chip, 0x87, 0xa5);
}
//////////////////////////////////////////////////////////////
static void TP2854_NTSC_DataSet(unsigned char chip)
{
    unsigned char tmp;
    //tp28xx_byte_write(chip, 0x07, 0xc0);
    tp28xx_byte_write(chip, 0x0b, 0xc0);
    tp28xx_byte_write(chip, 0x0c, 0x13);
    tp28xx_byte_write(chip, 0x0d, 0x50);

    tp28xx_byte_write(chip, 0x20, 0x40);
    tp28xx_byte_write(chip, 0x21, 0x84);
    tp28xx_byte_write(chip, 0x22, 0x36);
    tp28xx_byte_write(chip, 0x23, 0x3c);

    tp28xx_byte_write(chip, 0x25, 0xff);
    tp28xx_byte_write(chip, 0x26, 0x05);
    tp28xx_byte_write(chip, 0x27, 0x2d);
    tp28xx_byte_write(chip, 0x28, 0x00);

    tp28xx_byte_write(chip, 0x2b, 0x70);
    tp28xx_byte_write(chip, 0x2c, 0x2a);
    tp28xx_byte_write(chip, 0x2d, 0x68);
    tp28xx_byte_write(chip, 0x2e, 0x57);

    tp28xx_byte_write(chip, 0x30, 0x62);
    tp28xx_byte_write(chip, 0x31, 0xbb);
    tp28xx_byte_write(chip, 0x32, 0x96);
    tp28xx_byte_write(chip, 0x33, 0xc0);
    //tp28xx_byte_write(chip, 0x35, 0x25);
    tp28xx_byte_write(chip, 0x38, 0x00);
    tp28xx_byte_write(chip, 0x39, 0x04);
    tp28xx_byte_write(chip, 0x3a, 0x32);
    tp28xx_byte_write(chip, 0x3B, 0x26);

    tp28xx_byte_write(chip, 0x18, 0x12);

    tp28xx_byte_write(chip, 0x13, 0x00);
    tmp = tp28xx_byte_read(chip, 0x14);
    tmp &= 0x9f;
    tp28xx_byte_write(chip, 0x14, tmp);
}
static void TP2854_PAL_DataSet(unsigned char chip)
{
    unsigned char tmp;
    //tp28xx_byte_write(chip, 0x07, 0xc0);
    tp28xx_byte_write(chip, 0x0b, 0xc0);
    tp28xx_byte_write(chip, 0x0c, 0x13);
    tp28xx_byte_write(chip, 0x0d, 0x51);

    tp28xx_byte_write(chip, 0x20, 0x48);
    tp28xx_byte_write(chip, 0x21, 0x84);
    tp28xx_byte_write(chip, 0x22, 0x37);
    tp28xx_byte_write(chip, 0x23, 0x3f);

    tp28xx_byte_write(chip, 0x25, 0xff);
    tp28xx_byte_write(chip, 0x26, 0x05);
    tp28xx_byte_write(chip, 0x27, 0x2d);
    tp28xx_byte_write(chip, 0x28, 0x00);

    tp28xx_byte_write(chip, 0x2b, 0x70);
    tp28xx_byte_write(chip, 0x2c, 0x2a);
    tp28xx_byte_write(chip, 0x2d, 0x64);
    tp28xx_byte_write(chip, 0x2e, 0x56);

    tp28xx_byte_write(chip, 0x30, 0x7a);
    tp28xx_byte_write(chip, 0x31, 0x4a);
    tp28xx_byte_write(chip, 0x32, 0x4d);
    tp28xx_byte_write(chip, 0x33, 0xf0);
    //tp28xx_byte_write(chip, 0x35, 0x25);
    tp28xx_byte_write(chip, 0x38, 0x00);
    tp28xx_byte_write(chip, 0x39, 0x04);
    tp28xx_byte_write(chip, 0x3a, 0x32);
    tp28xx_byte_write(chip, 0x3B, 0x26);

    tp28xx_byte_write(chip, 0x18, 0x17);

    tp28xx_byte_write(chip, 0x13, 0x00);
    tmp = tp28xx_byte_read(chip, 0x14);
    tmp &= 0x9f;
    tp28xx_byte_write(chip, 0x14, tmp);
}
static void TP2854_V1_DataSet(unsigned char chip)
{
    unsigned char tmp;
    tp28xx_byte_write(chip, 0x13, 0x00);
    tmp = tp28xx_byte_read(chip, 0x14);
    tmp &= 0x9f;
    tp28xx_byte_write(chip, 0x14, tmp);

    //tp28xx_byte_write(chip, 0x07, 0xc0);
    tp28xx_byte_write(chip, 0x0b, 0xc0);
    tp28xx_byte_write(chip, 0x0c, 0x03);
    tp28xx_byte_write(chip, 0x0d, 0x50);

    tp28xx_byte_write(chip, 0x20, 0x30);
    tp28xx_byte_write(chip, 0x21, 0x84);
    tp28xx_byte_write(chip, 0x22, 0x36);
    tp28xx_byte_write(chip, 0x23, 0x3c);

    tp28xx_byte_write(chip, 0x25, 0xff);
    tp28xx_byte_write(chip, 0x26, 0x05);
    tp28xx_byte_write(chip, 0x27, 0x2d);
    tp28xx_byte_write(chip, 0x28, 0x00);

    tp28xx_byte_write(chip, 0x2b, 0x60);
    tp28xx_byte_write(chip, 0x2c, 0x0a);
    tp28xx_byte_write(chip, 0x2d, 0x30);
    tp28xx_byte_write(chip, 0x2e, 0x70);

    tp28xx_byte_write(chip, 0x30, 0x48);
    tp28xx_byte_write(chip, 0x31, 0xbb);
    tp28xx_byte_write(chip, 0x32, 0x2e);
    tp28xx_byte_write(chip, 0x33, 0x90);
    //tp28xx_byte_write(chip, 0x35, 0x05);
    tp28xx_byte_write(chip, 0x38, 0x00);
    tp28xx_byte_write(chip, 0x39, 0x1c);
    tp28xx_byte_write(chip, 0x3a, 0x32);
    tp28xx_byte_write(chip, 0x3B, 0x26);

    tp28xx_byte_write(chip, 0x80, 0x52);
    tp28xx_byte_write(chip, 0x81, 0x10);
    tp28xx_byte_write(chip, 0x82, 0x18);
    tp28xx_byte_write(chip, 0x83, 0x48);
    tp28xx_byte_write(chip, 0x84, 0x14);
    tp28xx_byte_write(chip, 0x88, 0x58);

    TP2854_RX_init(chip, PTZ_RX_TVI_CMD);
}
static void TP2854_V2_DataSet(unsigned char chip)
{
    unsigned char tmp;
    tp28xx_byte_write(chip, 0x13, 0x00);
    tmp = tp28xx_byte_read(chip, 0x14);
    tmp &= 0x9f;
    tp28xx_byte_write(chip, 0x14, tmp);

    //tp28xx_byte_write(chip, 0x07, 0xc0);
    tp28xx_byte_write(chip, 0x0b, 0xc0);
    tp28xx_byte_write(chip, 0x0c, 0x13);
    tp28xx_byte_write(chip, 0x0d, 0x50);

    tp28xx_byte_write(chip, 0x20, 0x30);
    tp28xx_byte_write(chip, 0x21, 0x84);
    tp28xx_byte_write(chip, 0x22, 0x36);
    tp28xx_byte_write(chip, 0x23, 0x3c);

    tp28xx_byte_write(chip, 0x25, 0xff);
    tp28xx_byte_write(chip, 0x26, 0x05);
    tp28xx_byte_write(chip, 0x27, 0x2d);
    tp28xx_byte_write(chip, 0x28, 0x00);

    tp28xx_byte_write(chip, 0x2b, 0x60);
    tp28xx_byte_write(chip, 0x2c, 0x0a);
    tp28xx_byte_write(chip, 0x2d, 0x30);
    tp28xx_byte_write(chip, 0x2e, 0x70);

    tp28xx_byte_write(chip, 0x30, 0x48);
    tp28xx_byte_write(chip, 0x31, 0xbb);
    tp28xx_byte_write(chip, 0x32, 0x2e);
    tp28xx_byte_write(chip, 0x33, 0x90);
    //tp28xx_byte_write(chip, 0x35, 0x25);
    tp28xx_byte_write(chip, 0x38, 0x00);
    tp28xx_byte_write(chip, 0x39, 0x18);
    tp28xx_byte_write(chip, 0x3a, 0x32);
    tp28xx_byte_write(chip, 0x3B, 0x26);

    TP2854_RX_init(chip, PTZ_RX_TVI_CMD);
}

///////////////////////////////////////////////////////////////////
static void TP2854_A720P30_DataSet(unsigned char chip)
{
    unsigned char tmp;
    tmp = tp28xx_byte_read(chip, 0x02);
    tmp |= 0x04;
    tp28xx_byte_write(chip, 0x02, tmp);

    tp28xx_byte_write(chip, 0x0d, 0x70);

    tp28xx_byte_write(chip, 0x20, 0x40);
    tp28xx_byte_write(chip, 0x21, 0x46);
    tp28xx_byte_write(chip, 0x22, 0x36);
    tp28xx_byte_write(chip, 0x23, 0x3c);

    tp28xx_byte_write(chip, 0x25, 0xfe);
    tp28xx_byte_write(chip, 0x26, 0x01);
    tp28xx_byte_write(chip, 0x27, 0x2d);
    tp28xx_byte_write(chip, 0x28, 0x00);

    tp28xx_byte_write(chip, 0x2b, 0x60);
    tp28xx_byte_write(chip, 0x2c, 0x3a);
    tp28xx_byte_write(chip, 0x2d, 0x5a);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0x9d);
    tp28xx_byte_write(chip, 0x31, 0xca);
    tp28xx_byte_write(chip, 0x32, 0x01);
    tp28xx_byte_write(chip, 0x33, 0xd0);

    tp28xx_byte_write(chip, 0x38, 0x00);
    tp28xx_byte_write(chip, 0x39, 0x18);

    TP2854_RX_init(chip, PTZ_RX_ACP1);
}
static void TP2854_A720P25_DataSet(unsigned char chip)
{
    unsigned char tmp;
    tmp = tp28xx_byte_read(chip, 0x02);
    tmp |= 0x04;
    tp28xx_byte_write(chip, 0x02, tmp);

    tp28xx_byte_write(chip, 0x0d, 0x71);

    tp28xx_byte_write(chip, 0x20, 0x40);
    tp28xx_byte_write(chip, 0x21, 0x46);
    tp28xx_byte_write(chip, 0x22, 0x36);
    tp28xx_byte_write(chip, 0x23, 0x3c);

    tp28xx_byte_write(chip, 0x25, 0xfe);
    tp28xx_byte_write(chip, 0x26, 0x01);
    tp28xx_byte_write(chip, 0x27, 0x2d);
    tp28xx_byte_write(chip, 0x28, 0x00);

    tp28xx_byte_write(chip, 0x2b, 0x60);
    tp28xx_byte_write(chip, 0x2c, 0x3a);
    tp28xx_byte_write(chip, 0x2d, 0x5a);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0x9e);
    tp28xx_byte_write(chip, 0x31, 0x20);
    tp28xx_byte_write(chip, 0x32, 0x10);
    tp28xx_byte_write(chip, 0x33, 0x90);

    tp28xx_byte_write(chip, 0x38, 0x00);
    tp28xx_byte_write(chip, 0x39, 0x18);

    TP2854_RX_init(chip, PTZ_RX_ACP1);
}
static void TP2854_A1080P30_DataSet(unsigned char chip)
{
    unsigned char tmp;
    tmp = tp28xx_byte_read(chip, 0x02);
    tmp |= 0x04;
    tp28xx_byte_write(chip, 0x02, tmp);

    tp28xx_byte_write(chip, 0x15, 0x01);
    tp28xx_byte_write(chip, 0x16, 0xf0);

    tp28xx_byte_write(chip, 0x0d, 0x72);

    tp28xx_byte_write(chip, 0x20, 0x38);
    tp28xx_byte_write(chip, 0x21, 0x46);
    tp28xx_byte_write(chip, 0x22, 0x36);
    tp28xx_byte_write(chip, 0x23, 0x3c);

    tp28xx_byte_write(chip, 0x25, 0xfe);
    tp28xx_byte_write(chip, 0x26, 0x01);
    tp28xx_byte_write(chip, 0x27, 0x2d);
    tp28xx_byte_write(chip, 0x28, 0x00);

    tp28xx_byte_write(chip, 0x2b, 0x60);
    tp28xx_byte_write(chip, 0x2c, 0x3a);
    tp28xx_byte_write(chip, 0x2d, 0x54);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0xa5);
    tp28xx_byte_write(chip, 0x31, 0x95);
    tp28xx_byte_write(chip, 0x32, 0xe0);
    tp28xx_byte_write(chip, 0x33, 0x60);

    tp28xx_byte_write(chip, 0x38, 0x00);
    tp28xx_byte_write(chip, 0x39, 0x1c);

    TP2854_RX_init(chip, PTZ_RX_ACP1);
}
static void TP2854_A1080P25_DataSet(unsigned char chip)
{
    unsigned char tmp;
    tmp = tp28xx_byte_read(chip, 0x02);
    tmp |= 0x04;
    tp28xx_byte_write(chip, 0x02, tmp);

    tp28xx_byte_write(chip, 0x15, 0x01);
    tp28xx_byte_write(chip, 0x16, 0xf0);

    tp28xx_byte_write(chip, 0x0d, 0x73);

    tp28xx_byte_write(chip, 0x20, 0x3c);
    tp28xx_byte_write(chip, 0x21, 0x46);
    tp28xx_byte_write(chip, 0x22, 0x36);
    tp28xx_byte_write(chip, 0x23, 0x3c);

    tp28xx_byte_write(chip, 0x25, 0xfe);
    tp28xx_byte_write(chip, 0x26, 0x05);
    tp28xx_byte_write(chip, 0x27, 0x2d);
    tp28xx_byte_write(chip, 0x28, 0x00);

    tp28xx_byte_write(chip, 0x2b, 0x60);
    tp28xx_byte_write(chip, 0x2c, 0x3a);
    tp28xx_byte_write(chip, 0x2d, 0x54);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0xa5);
    tp28xx_byte_write(chip, 0x31, 0x86);
    tp28xx_byte_write(chip, 0x32, 0xfb);
    tp28xx_byte_write(chip, 0x33, 0x60);

    tp28xx_byte_write(chip, 0x38, 0x00);
    tp28xx_byte_write(chip, 0x39, 0x1c);

    TP2854_RX_init(chip, PTZ_RX_ACP1);
}

static void TP2854_A720P60_DataSet(unsigned char chip)
{
    unsigned char tmp;

    tmp = tp28xx_byte_read(chip, 0x14);
    tmp |= 0x60;
    tp28xx_byte_write(chip, 0x14, tmp);

    tp28xx_byte_write(chip, 0x20, 0x38);
    tp28xx_byte_write(chip, 0x21, 0x46);

    tp28xx_byte_write(chip, 0x25, 0xfe);
    tp28xx_byte_write(chip, 0x26, 0x01);

    tp28xx_byte_write(chip, 0x2d, 0x44);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0x29);
    tp28xx_byte_write(chip, 0x31, 0x62);
    tp28xx_byte_write(chip, 0x32, 0x78);
    tp28xx_byte_write(chip, 0x33, 0x10);

    //tp28xx_byte_write(chip, 0x3B, 0x05);
}

static void TP2854_C1080P30_DataSet(unsigned char chip);
static void TP2854_C1080P25_DataSet(unsigned char chip);
static void TP2854_C720P30_DataSet(unsigned char chip);
static void TP2854_C720P25_DataSet(unsigned char chip);
static void TP2854_C720P60_DataSet(unsigned char chip);
static void TP2854_C720P50_DataSet(unsigned char chip);
static void TP2854_SYSCLK_V1(unsigned char chip, unsigned char ch);
static void TP2854_SYSCLK_V2(unsigned char chip, unsigned char ch);
//static void TP2854_SYSCLK_V3(unsigned char chip, unsigned char ch);
void TP28xx_reset_default(int chip, unsigned char ch);
static int tp2802_set_video_mode(unsigned char chip, unsigned char mode, unsigned char ch, unsigned char std)
{
    int err=0;
    unsigned int tmp;

    if(STD_HDA_DEFAULT == std) std = STD_HDA;

#if (DEBUG)
    printk("chip %d  mode %d ch %d std %d\r\n", chip, mode, ch, std);
#endif

    // Set Page Register to the appropriate Channel
    tp2802_set_reg_page(chip, ch);

    switch(mode)
    //switch(mode&(~FLAG_HALF_MODE))
    {
    case TP2802_1080P25:
        tp28xx_byte_write(chip, 0x35, 0x05);
        tp2802_set_work_mode_1080p25(chip);
        tp28xx_byte_write(chip, 0x02, 0xc0);
        TP2854_V1_DataSet(chip);

        if( STD_HDA == std)
        {
        	TP2854_A1080P25_DataSet(chip);
        }
        else if(STD_HDC == std || STD_HDC_DEFAULT == std)
        {
			TP2854_C1080P25_DataSet(chip);

            if(STD_HDC == std) //HDC 1080p25 position adjust
               {
                    {
                        tp28xx_byte_write(chip, 0x15, 0x13);
                        tp28xx_byte_write(chip, 0x16, 0x60);
                        tp28xx_byte_write(chip, 0x17, 0x80);
                        tp28xx_byte_write(chip, 0x18, 0x29);
                        tp28xx_byte_write(chip, 0x19, 0x38);
                        tp28xx_byte_write(chip, 0x1A, 0x47);
                        tp28xx_byte_write(chip, 0x1C, 0x09);
                        tp28xx_byte_write(chip, 0x1D, 0x60);
                    }
               }
        }

        TP2854_SYSCLK_V1(chip, ch);
        break;

    case TP2802_1080P30:
        tp28xx_byte_write(chip, 0x35, 0x05);
        tp2802_set_work_mode_1080p30(chip);
        tp28xx_byte_write(chip, 0x02, 0xc0);
        TP2854_V1_DataSet(chip);

        if( STD_HDA == std)
        {
            {
                TP2854_A1080P30_DataSet(chip);
            }
        }
        else if(STD_HDC == std || STD_HDC_DEFAULT == std)
        {
            {
                TP2854_C1080P30_DataSet(chip);
            }
            if(STD_HDC == std) //HDC 1080p30 position adjust
               {
                    {
                        tp28xx_byte_write(chip, 0x15, 0x13);
                        tp28xx_byte_write(chip, 0x16, 0x60);
                        tp28xx_byte_write(chip, 0x17, 0x80);
                        tp28xx_byte_write(chip, 0x18, 0x29);
                        tp28xx_byte_write(chip, 0x19, 0x38);
                        tp28xx_byte_write(chip, 0x1A, 0x47);
                        tp28xx_byte_write(chip, 0x1C, 0x09);
                        tp28xx_byte_write(chip, 0x1D, 0x60);
                    }
               }
        }

        TP2854_SYSCLK_V1(chip, ch);
        break;

    case TP2802_720P25:
        tp28xx_byte_write(chip, 0x35, 0x05);
        tp2802_set_work_mode_720p25(chip);
        tp28xx_byte_write(chip, 0x02, 0xc2);
        TP2854_V1_DataSet(chip);

        TP2854_SYSCLK_V1(chip, ch);
        break;

    case TP2802_720P30:
        tp28xx_byte_write(chip, 0x35, 0x05);
        tp2802_set_work_mode_720p30(chip);
        tp28xx_byte_write(chip, 0x02, 0xc2);
        TP2854_V1_DataSet(chip);

        TP2854_SYSCLK_V1(chip, ch);
        break;

    case TP2802_720P50:
        tp28xx_byte_write(chip, 0x35, 0x05);
        tp2802_set_work_mode_720p50(chip);
        tp28xx_byte_write(chip, 0x02, 0xc2);
        TP2854_V1_DataSet(chip);

        if(STD_HDA == std)
        {

        }
        else if(STD_HDC == std || STD_HDC_DEFAULT == std)
        {
            {
                TP2854_C720P50_DataSet(chip);
            }
            if(STD_HDC == std) //HDC 720p50 position adjust
               {

                    {
                        tp28xx_byte_write(chip, 0x15, 0x13);
                        tp28xx_byte_write(chip, 0x16, 0x0a);
                        tp28xx_byte_write(chip, 0x17, 0x00);
                        tp28xx_byte_write(chip, 0x18, 0x19);
                        tp28xx_byte_write(chip, 0x19, 0xd0);
                        tp28xx_byte_write(chip, 0x1A, 0x25);
                        tp28xx_byte_write(chip, 0x1C, 0x06);
                        tp28xx_byte_write(chip, 0x1D, 0x7a);
                    }
               }
        }

        TP2854_SYSCLK_V1(chip, ch);
        break;

    case TP2802_720P60:
        tp28xx_byte_write(chip, 0x35, 0x05);
        tp2802_set_work_mode_720p60(chip);
        tp28xx_byte_write(chip, 0x02, 0xc2);
        TP2854_V1_DataSet(chip);

        if(STD_HDA == std)
        {
            {
                TP2854_A720P60_DataSet(chip);
            }
        }
        else if(STD_HDC == std || STD_HDC_DEFAULT == std)
        {

            {
                TP2854_C720P60_DataSet(chip);
            }
            if(STD_HDC == std) //HDC 720p60 position adjust
               {
                    {
                        tp28xx_byte_write(chip, 0x15, 0x13);
                        tp28xx_byte_write(chip, 0x16, 0x08);
                        tp28xx_byte_write(chip, 0x17, 0x00);
                        tp28xx_byte_write(chip, 0x18, 0x19);
                        tp28xx_byte_write(chip, 0x19, 0xd0);
                        tp28xx_byte_write(chip, 0x1A, 0x25);
                        tp28xx_byte_write(chip, 0x1C, 0x06);
                        tp28xx_byte_write(chip, 0x1D, 0x72);
                    }
               }

        }

        TP2854_SYSCLK_V1(chip, ch);
        break;

    case TP2802_720P30V2:
        tp28xx_byte_write(chip, 0x35, 0x25);
        tp2802_set_work_mode_720p60(chip);
        tp28xx_byte_write(chip, 0x02, 0xc2);
        TP2854_V2_DataSet(chip);

        if(STD_HDA == std)
        {
            {
                TP2854_A720P30_DataSet(chip);
            }
        }
        else if(STD_HDC == std || STD_HDC_DEFAULT == std)
        {
            {
                TP2854_C720P30_DataSet(chip);
            }
            if(STD_HDC == std) //HDC 720p30 position adjust
               {
                    {
                        tp28xx_byte_write(chip, 0x15, 0x13);
                        tp28xx_byte_write(chip, 0x16, 0x08);
                        tp28xx_byte_write(chip, 0x17, 0x00);
                        tp28xx_byte_write(chip, 0x18, 0x19);
                        tp28xx_byte_write(chip, 0x19, 0xd0);
                        tp28xx_byte_write(chip, 0x1A, 0x25);
                        tp28xx_byte_write(chip, 0x1C, 0x06);
                        tp28xx_byte_write(chip, 0x1D, 0x72);
                    }
               }
        }

        TP2854_SYSCLK_V2(chip, ch);
        break;

    case TP2802_720P25V2:
        tp28xx_byte_write(chip, 0x35, 0x25);
        tp2802_set_work_mode_720p50(chip);
        tp28xx_byte_write(chip, 0x02, 0xc2);
        TP2854_V2_DataSet(chip);

        if(STD_HDA == std)
        {
            {
                TP2854_A720P25_DataSet(chip);
            }
        }
        else if(STD_HDC == std || STD_HDC_DEFAULT == std)
        {
            {
                TP2854_C720P25_DataSet(chip);
            }
            if(STD_HDC == std) //HDC 720p25 position adjust
               {
                    {
                        tp28xx_byte_write(chip, 0x15, 0x13);
                        tp28xx_byte_write(chip, 0x16, 0x0a);
                        tp28xx_byte_write(chip, 0x17, 0x00);
                        tp28xx_byte_write(chip, 0x18, 0x19);
                        tp28xx_byte_write(chip, 0x19, 0xd0);
                        tp28xx_byte_write(chip, 0x1A, 0x25);
                        tp28xx_byte_write(chip, 0x1C, 0x06);
                        tp28xx_byte_write(chip, 0x1D, 0x7a);
                    }
               }

        }

        TP2854_SYSCLK_V2(chip, ch);
        break;

    case TP2802_PAL:
    	//if( TP2856 == id[chip])
    	{
        	tp28xx_byte_write(chip, 0x35, 0x65);
    	}
        tp2802_set_work_mode_PAL(chip);
        tp28xx_byte_write(chip, 0x02, 0xc7);
        TP2854_PAL_DataSet(chip);

        TP2854_SYSCLK_V2(chip, ch);
        break;

    case TP2802_NTSC:
    	//if( TP2856 == id[chip])
    	{
        	tp28xx_byte_write(chip, 0x35, 0x65);
    	}
        tp2802_set_work_mode_NTSC(chip);
        tp28xx_byte_write(chip, 0x02, 0xc7);
        TP2854_NTSC_DataSet(chip);

        TP2854_SYSCLK_V2(chip, ch);
        break;
    default:
        err = -1;
        break;
    }


    {
        tmp = tp28xx_byte_read(chip, 0x02);
        tmp &= 0x7f;
        tp28xx_byte_write(chip, 0x02, tmp);
    }

    return err;
}


static void tp2802_set_reg_page(unsigned char chip, unsigned char ch)
{
    switch(ch)
    {
    case CH_1:
        tp28xx_byte_write(chip, 0x40, 0x00);
        break;  //
    case CH_2:
        tp28xx_byte_write(chip, 0x40, 0x01);
        break;  //
    case CH_3:
        tp28xx_byte_write(chip, 0x40, 0x02);
        break;  //
    case CH_4:
        tp28xx_byte_write(chip, 0x40, 0x03);
        break;  //
    case CH_ALL:
        tp28xx_byte_write(chip, 0x40, 0x04);
        break;  //
    case AUDIO_PAGE:
        tp28xx_byte_write(chip, 0x40, 0x40);
        break;  // Audio
    case MIPI_PAGE:
        tp28xx_byte_write(chip, 0x40, 0x08);
        break;  //
    case MIPI_PAGE2:
        tp28xx_byte_write(chip, 0x40, 0x09);
        break;  //
    }
}
/*
const char SYSTEM_AND[5]={0xfe,0xfd,0xfb,0xf7,0xf0};
const char SYSTEM_MODE[5]={0x01,0x02,0x04,0x08,0x0f};
*/
static void TP2854_SYSCLK_V2(unsigned char chip, unsigned char ch)
{
/*
    unsigned char tmp;

	if( TP2854 == id[chip])
	{
    		tmp = tp28xx_byte_read(chip, 0x40);
		tp28xx_byte_write(chip, 0x40, 0x08);
		tp28xx_byte_write(chip, 0x23, 0x02);
		tp28xx_byte_write(chip, 0x13, 0x24);
		tp28xx_byte_write(chip, 0x14, 0x04);
		tp28xx_byte_write(chip, 0x15, 0x00);

		tp28xx_byte_write(chip, 0x25, 0x08);
		tp28xx_byte_write(chip, 0x26, 0x02);
		tp28xx_byte_write(chip, 0x27, 0x0b);

		tp28xx_byte_write(chip, 0x10, 0x88);
		tp28xx_byte_write(chip, 0x10, 0x08);
		tp28xx_byte_write(chip, 0x23, 0x00);
		tp28xx_byte_write(chip, 0x40, tmp);
	}
	else
*/
	{
		tp28xx_byte_write(chip, 0xf5, tp28xx_byte_read(chip, 0xf5) | SYS_MODE[ch]);

	}


}
static void TP2854_SYSCLK_V1(unsigned char chip, unsigned char ch)
{
/*
    unsigned char tmp;

	if( TP2854 == id[chip])
	{
		tmp = tp28xx_byte_read(chip, 0x40);
		tp28xx_byte_write(chip, 0x40, 0x08);
		tp28xx_byte_write(chip, 0x23, 0x02);
		tp28xx_byte_write(chip, 0x13, 0x04);
		tp28xx_byte_write(chip, 0x14, 0x04);
		tp28xx_byte_write(chip, 0x15, 0x00);

		tp28xx_byte_write(chip, 0x25, 0x10);
		tp28xx_byte_write(chip, 0x26, 0x02);
		tp28xx_byte_write(chip, 0x27, 0x16);

		tp28xx_byte_write(chip, 0x10, 0x88);
		tp28xx_byte_write(chip, 0x10, 0x08);
		tp28xx_byte_write(chip, 0x23, 0x00);
		tp28xx_byte_write(chip, 0x40, tmp);
	}
	else
*/
	{
		tp28xx_byte_write(chip, 0xf5, tp28xx_byte_read(chip, 0xf5) & SYS_AND[ch]);

		if( MIPI_4CH4LANE_297M == output || MIPI_2CH2LANE_297M == output || MIPI_4CH2LANE_594M == output )
		{
			tp28xx_byte_write(chip, 0x35, tp28xx_byte_read(chip, 0x35) | 0x40 );
		}

	}


}

void TP28xx_reset_default(int chip, unsigned char ch)
{
    //if(TP2854 == id[chip] )
    {
        TP2856_reset_default(chip, ch);
    }
}


static void TP2854_C1080P25_DataSet(unsigned char chip)
{

    tp28xx_byte_write(chip, 0x13, 0x40);

    tp28xx_byte_write(chip, 0x20, 0x50);

    tp28xx_byte_write(chip, 0x26, 0x01);
    tp28xx_byte_write(chip, 0x27, 0x5a);
    tp28xx_byte_write(chip, 0x28, 0x04);

    tp28xx_byte_write(chip, 0x2b, 0x60);

    tp28xx_byte_write(chip, 0x2d, 0x54);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0x41);
    tp28xx_byte_write(chip, 0x31, 0x82);
    tp28xx_byte_write(chip, 0x32, 0x27);
    tp28xx_byte_write(chip, 0x33, 0xa2);

}
static void TP2854_C720P25_DataSet(unsigned char chip)
{

    tp28xx_byte_write(chip, 0x13, 0x40);

    tp28xx_byte_write(chip, 0x20, 0x3a);

    tp28xx_byte_write(chip, 0x26, 0x01);
    tp28xx_byte_write(chip, 0x27, 0x5a);
    tp28xx_byte_write(chip, 0x28, 0x04);

    tp28xx_byte_write(chip, 0x2b, 0x60);
    tp28xx_byte_write(chip, 0x2d, 0x36);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0x48);
    tp28xx_byte_write(chip, 0x31, 0x67);
    tp28xx_byte_write(chip, 0x32, 0x6f);
    tp28xx_byte_write(chip, 0x33, 0x33);

}
static void TP2854_C720P50_DataSet(unsigned char chip)
{

    tp28xx_byte_write(chip, 0x13, 0x40);

    tp28xx_byte_write(chip, 0x20, 0x3a);

    tp28xx_byte_write(chip, 0x26, 0x01);
    tp28xx_byte_write(chip, 0x27, 0x5a);
    tp28xx_byte_write(chip, 0x28, 0x04);

    tp28xx_byte_write(chip, 0x2b, 0x60);

    tp28xx_byte_write(chip, 0x2d, 0x42);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0x41);
    tp28xx_byte_write(chip, 0x31, 0x82);
    tp28xx_byte_write(chip, 0x32, 0x27);
    tp28xx_byte_write(chip, 0x33, 0xa3);

}
static void TP2854_C1080P30_DataSet(unsigned char chip)
{

    tp28xx_byte_write(chip, 0x13, 0x40);

    tp28xx_byte_write(chip, 0x20, 0x3c);

    tp28xx_byte_write(chip, 0x26, 0x01);
    tp28xx_byte_write(chip, 0x27, 0x5a);
    tp28xx_byte_write(chip, 0x28, 0x04);

    tp28xx_byte_write(chip, 0x2b, 0x60);

    tp28xx_byte_write(chip, 0x2d, 0x4c);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0x41);
    tp28xx_byte_write(chip, 0x31, 0x82);
    tp28xx_byte_write(chip, 0x32, 0x27);
    tp28xx_byte_write(chip, 0x33, 0xa4);

}
static void TP2854_C720P30_DataSet(unsigned char chip)
{

    tp28xx_byte_write(chip, 0x13, 0x40);

    tp28xx_byte_write(chip, 0x20, 0x30);

    tp28xx_byte_write(chip, 0x26, 0x01);
    tp28xx_byte_write(chip, 0x27, 0x5a);
    tp28xx_byte_write(chip, 0x28, 0x04);

    tp28xx_byte_write(chip, 0x2b, 0x60);

    tp28xx_byte_write(chip, 0x2d, 0x37);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0x48);
    tp28xx_byte_write(chip, 0x31, 0x67);
    tp28xx_byte_write(chip, 0x32, 0x6f);
    tp28xx_byte_write(chip, 0x33, 0x30);

}
static void TP2854_C720P60_DataSet(unsigned char chip)
{

    tp28xx_byte_write(chip, 0x13, 0x40);

    tp28xx_byte_write(chip, 0x20, 0x30);

    tp28xx_byte_write(chip, 0x26, 0x01);
    tp28xx_byte_write(chip, 0x27, 0x5a);
    tp28xx_byte_write(chip, 0x28, 0x04);

    tp28xx_byte_write(chip, 0x2b, 0x60);

    tp28xx_byte_write(chip, 0x2d, 0x37);
    tp28xx_byte_write(chip, 0x2e, 0x40);

    tp28xx_byte_write(chip, 0x30, 0x41);
    tp28xx_byte_write(chip, 0x31, 0x82);
    tp28xx_byte_write(chip, 0x32, 0x27);
    tp28xx_byte_write(chip, 0x33, 0xa0);

}
///////////////////////////////////////////////////////////////

static void TP2856_output(unsigned char chip, unsigned char port)
{

	if(MIPI_PAGE == port || MIPI_PAGE2 == port )
	{
        tp28xx_byte_write(chip, 0x40, port);
        tp28xx_byte_write(chip, 0x01, 0xf0);
        tp28xx_byte_write(chip, 0x02, 0x01);
        tp28xx_byte_write(chip, 0x08, 0x0f);


        if (output == MIPI_4CH4LANE_594M)
        {
            tp28xx_byte_write(chip, 0x20, 0x44);
            tp28xx_byte_write(chip, 0x34, 0xe4);
            tp28xx_byte_write(chip, 0x15, 0x0c);
            tp28xx_byte_write(chip, 0x25, 0x08);
            tp28xx_byte_write(chip, 0x26, 0x06);
            tp28xx_byte_write(chip, 0x27, 0x11);
            tp28xx_byte_write(chip, 0x29, 0x0a);
            tp28xx_byte_write(chip, 0x33, 0x07);
            tp28xx_byte_write(chip, 0x33, 0x00);
            tp28xx_byte_write(chip, 0x14, 0x33);
            tp28xx_byte_write(chip, 0x14, 0xb3);
            tp28xx_byte_write(chip, 0x14, 0x33);
            tp28xx_byte_write(chip, 0x23, 0x03);
            tp28xx_byte_write(chip, 0x23, 0x00);
        }
        else if (output == MIPI_2CH4LANE_594M) {
            tp28xx_byte_write(chip, 0x20, 0x24);
            tp28xx_byte_write(chip, 0x34, 0x10); //output vin1&vin2
            tp28xx_byte_write(chip, 0x15, 0x0C);
            tp28xx_byte_write(chip, 0x25, 0x08);
            tp28xx_byte_write(chip, 0x26, 0x06);
            tp28xx_byte_write(chip, 0x27, 0x11);
            tp28xx_byte_write(chip, 0x29, 0x0a);
            tp28xx_byte_write(chip, 0x33, 0x07);
            tp28xx_byte_write(chip, 0x33, 0x00);
            tp28xx_byte_write(chip, 0x14, 0x33);
            tp28xx_byte_write(chip, 0x14, 0xb3);
            tp28xx_byte_write(chip, 0x14, 0x33);
            tp28xx_byte_write(chip, 0x23, 0x03);
            tp28xx_byte_write(chip, 0x23, 0x00);
        }
        else if (output == MIPI_4CH4LANE_297M) {
            tp28xx_byte_write(chip, 0x20, 0x44);
            tp28xx_byte_write(chip, 0x34, 0xe4);
            tp28xx_byte_write(chip, 0x14, 0x44);
            tp28xx_byte_write(chip, 0x15, 0x0d);
            tp28xx_byte_write(chip, 0x25, 0x04);
            tp28xx_byte_write(chip, 0x26, 0x03);
            tp28xx_byte_write(chip, 0x27, 0x09);
            tp28xx_byte_write(chip, 0x29, 0x02);
            tp28xx_byte_write(chip, 0x33, 0x07);
            tp28xx_byte_write(chip, 0x33, 0x00);
            tp28xx_byte_write(chip, 0x14, 0xc4);
            tp28xx_byte_write(chip, 0x14, 0x44);
            tp28xx_byte_write(chip, 0x23, 0x03);
            tp28xx_byte_write(chip, 0x23, 0x00);
        } else if (output == MIPI_2CH4LANE_297M) {
            tp28xx_byte_write(chip, 0x20, 0x24);
            tp28xx_byte_write(chip, 0x34, 0x10); //output vin1&vin2
            tp28xx_byte_write(chip, 0x14, 0x44);
            tp28xx_byte_write(chip, 0x15, 0x0d);
            tp28xx_byte_write(chip, 0x25, 0x04);
            tp28xx_byte_write(chip, 0x26, 0x03);
            tp28xx_byte_write(chip, 0x27, 0x09);
            tp28xx_byte_write(chip, 0x29, 0x02);
            tp28xx_byte_write(chip, 0x33, 0x07);
            tp28xx_byte_write(chip, 0x33, 0x00);
            tp28xx_byte_write(chip, 0x14, 0xc4);
            tp28xx_byte_write(chip, 0x14, 0x44);
            tp28xx_byte_write(chip, 0x23, 0x03);
            tp28xx_byte_write(chip, 0x23, 0x00);
        } else if (output == MIPI_1CH4LANE_297M) {
            tp28xx_byte_write(chip, 0x20, 0x14);
            tp28xx_byte_write(chip, 0x34, 0xe4);
            tp28xx_byte_write(chip, 0x14, 0x44);
            tp28xx_byte_write(chip, 0x15, 0x0d);
            tp28xx_byte_write(chip, 0x25, 0x04);
            tp28xx_byte_write(chip, 0x26, 0x03);
            tp28xx_byte_write(chip, 0x27, 0x09);
            tp28xx_byte_write(chip, 0x29, 0x02);
            tp28xx_byte_write(chip, 0x33, 0x07);
            tp28xx_byte_write(chip, 0x33, 0x00);
            tp28xx_byte_write(chip, 0x14, 0xc4);
            tp28xx_byte_write(chip, 0x14, 0x44);
            tp28xx_byte_write(chip, 0x23, 0x03);
            tp28xx_byte_write(chip, 0x23, 0x00);
        }
	else if (output == MIPI_4CH2LANE_594M) {
            tp28xx_byte_write(chip, 0x20, 0x42);
            tp28xx_byte_write(chip, 0x34, 0xe4);
            tp28xx_byte_write(chip, 0x15, 0x0c);
            tp28xx_byte_write(chip, 0x25, 0x08);
            tp28xx_byte_write(chip, 0x26, 0x06);
            tp28xx_byte_write(chip, 0x27, 0x11);
            tp28xx_byte_write(chip, 0x29, 0x0a);
            tp28xx_byte_write(chip, 0x33, 0x07);
            tp28xx_byte_write(chip, 0x33, 0x00);
            tp28xx_byte_write(chip, 0x14, 0x43);
            tp28xx_byte_write(chip, 0x14, 0xc3);
            tp28xx_byte_write(chip, 0x14, 0x43);
            tp28xx_byte_write(chip, 0x23, 0x03);
            tp28xx_byte_write(chip, 0x23, 0x00);
        } else if (output == MIPI_2CH2LANE_594M) {
            tp28xx_byte_write(chip, 0x20, 0x22);
            tp28xx_byte_write(chip, 0x34, 0x10); //output vin1&vin2
            tp28xx_byte_write(chip, 0x15, 0x0c);
            tp28xx_byte_write(chip, 0x25, 0x08);
            tp28xx_byte_write(chip, 0x26, 0x06);
            tp28xx_byte_write(chip, 0x27, 0x11);
            tp28xx_byte_write(chip, 0x29, 0x0a);
            tp28xx_byte_write(chip, 0x33, 0x07);
            tp28xx_byte_write(chip, 0x33, 0x00);
            tp28xx_byte_write(chip, 0x14, 0x43);
            tp28xx_byte_write(chip, 0x14, 0xc3);
            tp28xx_byte_write(chip, 0x14, 0x43);
            tp28xx_byte_write(chip, 0x23, 0x03);
            tp28xx_byte_write(chip, 0x23, 0x00);
        }else if (output == MIPI_1CH2LANE_594M) {
            tp28xx_byte_write(chip, 0x20, 0x12);
            tp28xx_byte_write(chip, 0x34, 0xe4);
            tp28xx_byte_write(chip, 0x15, 0x0c);
            tp28xx_byte_write(chip, 0x25, 0x08);
            tp28xx_byte_write(chip, 0x26, 0x06);
            tp28xx_byte_write(chip, 0x27, 0x11);
            tp28xx_byte_write(chip, 0x29, 0x0a);
            tp28xx_byte_write(chip, 0x33, 0x07);
            tp28xx_byte_write(chip, 0x33, 0x00);
            tp28xx_byte_write(chip, 0x14, 0x43);
            tp28xx_byte_write(chip, 0x14, 0xc3);
            tp28xx_byte_write(chip, 0x14, 0x43);
            tp28xx_byte_write(chip, 0x23, 0x03);
            tp28xx_byte_write(chip, 0x23, 0x00);
        }
	else if (output == MIPI_1CH2LANE_297M) {
            tp28xx_byte_write(chip, 0x20, 0x12);
            tp28xx_byte_write(chip, 0x34, 0xe4);
            tp28xx_byte_write(chip, 0x14, 0x54);
            tp28xx_byte_write(chip, 0x15, 0x0d);
            tp28xx_byte_write(chip, 0x25, 0x04);
            tp28xx_byte_write(chip, 0x26, 0x03);
            tp28xx_byte_write(chip, 0x27, 0x09);
            tp28xx_byte_write(chip, 0x29, 0x02);
            tp28xx_byte_write(chip, 0x33, 0x07);
            tp28xx_byte_write(chip, 0x33, 0x00);
            tp28xx_byte_write(chip, 0x14, 0xd4);
            tp28xx_byte_write(chip, 0x14, 0x54);
            tp28xx_byte_write(chip, 0x23, 0x03);
            tp28xx_byte_write(chip, 0x23, 0x00);
        }
	else if (output == MIPI_2CH2LANE_297M) {
            tp28xx_byte_write(chip, 0x20, 0x22);
            tp28xx_byte_write(chip, 0x34, 0x10); //output vin1&vin2
            tp28xx_byte_write(chip, 0x14, 0x54);
            tp28xx_byte_write(chip, 0x15, 0x0d);
            tp28xx_byte_write(chip, 0x25, 0x04);
            tp28xx_byte_write(chip, 0x26, 0x03);
            tp28xx_byte_write(chip, 0x27, 0x09);
            tp28xx_byte_write(chip, 0x29, 0x02);
            tp28xx_byte_write(chip, 0x33, 0x07);
            tp28xx_byte_write(chip, 0x33, 0x00);
            tp28xx_byte_write(chip, 0x14, 0xd4);
            tp28xx_byte_write(chip, 0x14, 0x54);
            tp28xx_byte_write(chip, 0x23, 0x03);
            tp28xx_byte_write(chip, 0x23, 0x00);
        }

        tp28xx_byte_write(chip, 0x40, 0x04);
	}

}

static void tp2802_video_init(int chip)
{
    tp28xx_byte_write(chip, 0x10, 0x70); //0x70 -> 0x10
    tp28xx_byte_write(chip, 0x11, 0x45); //0x45 -> 0x30
    tp28xx_byte_write(chip, 0x12, 0x50); //0x50 -> 0x1a
    tp28xx_byte_write(chip, 0x13, 0xf5); //0xf5 -> 0x00
    tp28xx_byte_write(chip, 0x14, 0x3f); //0x3f -> 0x12
}
///////////////////////////////////////////////////////////////
static void tp2802_comm_init( int chip)
{
    //unsigned int val;

    tp2802_set_reg_page(chip, CH_ALL);

    TP2856_reset_default(chip, CH_ALL);

    tp2802_set_video_mode(chip, mode, CH_ALL, STD_HDA);

	TP2856_Audio_DataSet(chip);

    TP2854_RX_init(chip, PTZ_RX_ACP1);

    tp2802_video_init(chip);

    TP2856_output(chip,MIPI_PAGE);
	
	if(TP2856 == id[chip]) 
	{
		TP2856_output(chip,MIPI_PAGE2);
	}
}

void tp2802_reset_init_reg(unsigned char pw_ctrl)
{
	if(pw_ctrl & AHD5V_12CTRL)
		gpio_set_value(DMS_ADAS_CAMERA_POWER_GPIO_NUM, 0);

	if(pw_ctrl & AHD12V_34CTRL)
		gpio_set_value(COMMON_CAMERA_POWER_GPIO_NUM, 0);

	msleep(1000);

	if(pw_ctrl & AHD5V_12CTRL)
		gpio_set_value(DMS_ADAS_CAMERA_POWER_GPIO_NUM, 1);	

	if(pw_ctrl & AHD12V_34CTRL)
		gpio_set_value(COMMON_CAMERA_POWER_GPIO_NUM, 1);	

	msleep(1000);
}

static struct file_operations tp2802_fops =
{
    .owner      = THIS_MODULE,
    .unlocked_ioctl  = tp2802_ioctl,
    .open       = tp2802_open,
    .release    = tp2802_close,
};

static struct miscdevice tp2802_dev =
{
    .minor		= MISC_DYNAMIC_MINOR,
    .name		= "video_rx",
    .fops  		= &tp2802_fops,
};

int do_tp2802_misc_register(void)
{	
	mutex_init(&tp2802_mutex);

	if(misc_register(&tp2802_dev)) 
	{
		printk(KERN_INFO "misc_register tp2802_dev error\n");
		return -1;
	}

	return 0;
}

int do_tp2802_misc_deregister(void)
{
	misc_deregister(&tp2802_dev);

	return 0;
}

int tp2802_module_init(void)
{
	int i = 0;
	unsigned char val = 0;

#if (DEBUG)
    printk("TP2856 driver version %d.%d.%d loaded\n",
           (TP2802_VERSION_CODE >> 16) & 0xff,
           (TP2802_VERSION_CODE >>  8) & 0xff,
           TP2802_VERSION_CODE & 0xff);
#endif
	
    if (chips <= 0 || chips > MAX_CHIPS)  
    {
        printk("TP2856 module param 'chips' invalid value:%d\n", chips);
        return FAILURE;
    }

    /* initize each tp2802*/
    for (i = 0; i < chips; i++)
    {
		//page reset
		id[i] = 0x0000;
		tp28xx_byte_write(i, 0x40, 0x00);

		val = tp28xx_byte_read(i, 0xfe);
		if(0x28 == val)
		{
			//printk("Detected TP28xx \n");
		}
		else
		{
			id[i] = 0xffff;
			printk("Invalid chip %2x\n", val);
			chips = i;
			break;
		}

		id[i] = tp28xx_byte_read(i, 0xff);
		id[i] <<=8;
		id[i] +=tp28xx_byte_read(i, 0xfd);


		if (TP2855 == id[i])
		{
			tp2802_comm_init(i);
		}
    }

	if (0 == chips)
	{
#if (DEBUG)
		printk(KERN_INFO "No Detected Chip !!!\n");
#endif

		return FAILURE;
	}

#if (DEBUG)
    printk(KERN_INFO "TP2855 Driver Init Successful!\n");
#endif

    return SUCCESS;
}

void tp2802_module_exit(void)
{
    printk("TP2856 Driver Exit Successful!\n");
}

void tp283x_egain(unsigned int chip, unsigned int CGAIN_STD)
{
    unsigned int tmp, cgain;
    unsigned int retry = 30;

    tp28xx_byte_write(chip, 0x2f, 0x06);
    cgain = tp28xx_byte_read(chip, 0x04);
#if (DEBUG)
    printk("Cgain=0x%02x \r\n", cgain );
#endif

    if(cgain < CGAIN_STD )
    {
        while(retry)
        {
            retry--;

            tmp = tp28xx_byte_read(chip, 0x07);
            tmp &=0x3f;
            while(abs(CGAIN_STD-cgain))
            {
                if(tmp) tmp--;
                else break;
                cgain++;
            }

            tp28xx_byte_write(chip, 0x07, 0x80|tmp);
            if(0 == tmp) break;
            msleep(40);
            tp28xx_byte_write(chip, 0x2f, 0x06);
            cgain = tp28xx_byte_read(chip, 0x04);
#if (DEBUG)
            printk("Cgain=0x%02x \r\n", cgain );
#endif
            if(cgain > (CGAIN_STD+1))
            {
                tmp = tp28xx_byte_read(chip, 0x07);
                tmp &=0x3f;
                tmp +=0x02;
                if(tmp > 0x3f) tmp = 0x3f;
                tp28xx_byte_write(chip, 0x07, 0x80|tmp);
                if(0x3f == tmp) break;
                msleep(40);
                cgain = tp28xx_byte_read(chip, 0x04);
#if (DEBUG)
                printk("Cgain=0x%02x \r\n", cgain);
#endif
            }
            if(abs(cgain-CGAIN_STD) < 2)  break;
        }

    }
}
/////////////////////////////////////////////////////////////////
unsigned char tp28xx_read_egain(unsigned char chip)
{
    unsigned char gain;

            tp28xx_byte_write(chip, 0x2f, 0x00);
            gain = tp28xx_byte_read(chip, 0x04);
            return gain;

}
