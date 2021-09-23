#ifndef __VIDEORX_COMMON_H__
#define __VIDEORX_COMMON_H__

#define IOC_VDEC_GET_VIDEO_LOSS     	(0x09)
#define IOC_VDEC_SET_BRIGHTNESS	    	(0x0E)
#define IOC_VDEC_SET_CONTRAST   		(0x0F)
#define IOC_VDEC_SET_HUE    			(0x10)
#define IOC_VDEC_SET_SATURATION  		(0x11)
#define IOC_VDEC_SET_SHARPNESS  		(0x12)

#define IOC_VDEC_DUMP_REG				(0xE0)
#define IOC_VDEC_RESET_REG				(0xE1)
#define IOC_VDEC_GET_FRMCNT				(0xE2)
#define IOC_VDEC_GET_SPT				(0xE3)
#define IOC_VDEC_SET_PB_VOL				(0xE4)
#define IOC_VDEC_SET_REC_VOL			(0xE5)

#define IOC_VDEC_GET_VIDEO_INFO			(0xEA)
#define IOC_VDEC_SET_VIDEO_OUT_DELAY	(0xEB)

#define IOC_VDEC_SET_VIDEO_MODE			(0xEC)
#define IOC_VDEC_GET_VIDEO_MODE			(0xED)

#define IOC_VDEC_SET_AUDIO_RM_FORMAT	(0xEE)
#define IOC_VDEC_SET_AUDIO_PB_FORMAT	(0xEF)
#define IOC_VDEC_SET_SAMPLE_RATE		(0xF0)
#define IOC_VDEC_SET_AUDIO_PLAYBACK		(0xF1)

#define IOC_VDEC_SET_TP9930_VIDEO_MODE	(0xF8)
#define TP9930_VIDEO_MODE_1080P			(0)
#define TP9930_VIDEO_MODE_720P			(1)

#define AHD5V_12CTRL					(1<<0)
#define AHD12V_34CTRL					(1<<1)

#define AHD_NOSUPPORT					(0)
#define AHD_SUPPORT						(1)

#define DMS_ADAS_CAMERA_POWER_GPIO_NUM	(53) //GPIO1-C5_D : 1 * 32 + 21
#define COMMON_CAMERA_POWER_GPIO_NUM	(54) //GPIO1-C6_D : 1 * 32 + 22 = 54

#define RX0_RESET_GPIO_NUM				(19) //GPIO0-C3_D : 0 *32 + 19 = 19
#define RX1_RESET_GPIO_NUM				(16) //GPIO0-C0_D : 0 * 32 + 16 = 16

typedef struct _videorx_video_mode //_nvp6158_video_mode
{
    unsigned int chip;
    unsigned int mode;
	unsigned char vformat[16];
	unsigned char chmode[16];
}videorx_video_mode; //nvp6158_video_mode

typedef struct _videorx_chn_mode //_nvp6158_chn_mode
{
    unsigned char ch;
	unsigned char vformat;
	unsigned char chmode;
}videorx_chn_mode; //nvp6158_chn_mode

typedef struct _videorx_opt_mode //_nvp6158_opt_mode
{
	unsigned char chipsel;
    unsigned char portsel;
	unsigned char portmode;
	unsigned char chid;
}videorx_opt_mode; //nvp6158_opt_mode

typedef struct _videorx_input_videofmt //_nvp6158_input_videofmt
{
    unsigned int inputvideofmt[16];
	unsigned int getvideofmt[16];
	unsigned int geteqstage[16];
	unsigned int getacpdata[16][8];
}videorx_input_videofmt; //nvp6158_input_videofmt

typedef struct _videorx_input_videofmt_ch //_nvp6158_input_videofmt_ch
{
	unsigned char ch;
	videorx_input_videofmt vfmt;
}videorx_input_videofmt_ch; //nvp6158_input_videofmt_ch

typedef struct _videorx_i2c_mode //_nvp6124_i2c_mode
{
	unsigned char flag;       // 0: read, 1 : write
	unsigned char slaveaddr;
	unsigned char bank;
	unsigned char address;
	unsigned char data;
}videorx_i2c_mode; //nvp6124_i2c_mode

typedef struct _videorx_video_adjust //_nvp6158_video_adjust
{
 	unsigned char ch;
	unsigned char value;
}videorx_video_adjust; //nvp6158_video_adjust

typedef struct _videorx_motion_area //_nvp6158_motion_area
{
    unsigned char ch;
    int m_info[12];
}videorx_motion_area; //nvp6158_motion_area

typedef struct _videorx_audio_playback //_nvp6158_audio_playback
{
    unsigned char chip;
    unsigned char ch;
}videorx_audio_playback; //nvp6158_audio_playback

typedef struct _videorx_audio_da_mute //_nvp6158_audio_da_mute
{
    unsigned char chip;
}videorx_audio_da_mute; //nvp6158_audio_da_mute

typedef struct _videorx_audio_da_volume //_nvp6158_audio_da_volume
{
    unsigned char chip;
    unsigned char volume;
}videorx_audio_da_volume; //nvp6158_audio_da_volume

typedef struct _videorx_audio_format //_nvp6158_audio_format
{
	unsigned char format;   /* 0:i2s; 1:dsp */
    unsigned char mode;   /* 0:slave 1:master*/
	unsigned char dspformat; /*0:dsp;1:ssp*/
    unsigned char clkdir;  /*0:inverted;1:non-inverted*/
	unsigned char chn_num; /*2,4,8,16*/
	unsigned char bitrate; /*0:256fs 1:384fs invalid for nvp6114 2:320fs*/
	unsigned char precision;/*0:16bit;1:8bit*/
	unsigned char samplerate;/*0:8kHZ;1:16kHZ; 2:32kHZ*/
} videorx_audio_format; //nvp6158_audio_format

enum __DETECTION_TYPE_INFORMATION__
{
	DETECTION_TYPE_AUTO=0,
	DETECTION_TYPE_AHD,
	DETECTION_TYPE_CHD,
	DETECTION_TYPE_THD,
	DETECTION_TYPE_CVBS,

	DETECTION_TYPE_MAX
};

#define VIDEORX_IOC_MAGIC            'n' //NVP6158_IOC_MAGIC

#define VIDEORX_SET_AUDIO_PLAYBACK   		_IOW(VIDEORX_IOC_MAGIC, 0x21, videorx_audio_playback) 
#define VIDEORX_SET_AUDIO_DA_MUTE    		_IOW(VIDEORX_IOC_MAGIC, 0x22, videorx_audio_da_mute)
#define VIDEORX_SET_AUDIO_DA_VOLUME  		_IOW(VIDEORX_IOC_MAGIC, 0x23, videorx_audio_da_volume)
/*set record format*/
#define VIDEORX_SET_AUDIO_R_FORMAT     		_IOW(VIDEORX_IOC_MAGIC, 0x24, videorx_audio_format)
/*set playback format*/
#define VIDEORX_SET_AUDIO_PB_FORMAT     	_IOW(VIDEORX_IOC_MAGIC, 0x25, videorx_audio_format)

#define _SET_BIT(data,bit) ((data)|=(1<<(bit)))
#define _CLE_BIT(data,bit) ((data)&=(~(1<<(bit))))
//////////////////////////////////////
typedef enum _VIDEORX_VIDEO_ONOFF //_NC_VIDEO_ONOFF
{
	VIDEO_LOSS_ON = 0,
	VIDEO_LOSS_OFF = 1,

} VIDEORX_VIDEO_ONOFF; //NC_VIDEO_ONOFF

typedef struct _decoder_dev_ch_info_s
{
	unsigned char ch;
	unsigned char devnum;
	unsigned char fmt_def;
}decoder_dev_ch_info_s;

typedef enum VIDEORX_VIVO_CH_FORMATDEF //NC_VIVO_CH_FORMATDEF
{
	VIDEORX_VIVO_CH_FORMATDEF_UNKNOWN = 0,
	VIDEORX_VIVO_CH_FORMATDEF_AUTO,

	AHD20_SD_H960_NT,   //960h*480i					
	AHD20_SD_H960_PAL,  //960h*576i					
	AHD20_SD_SH720_NT,  //960h*576i				
	AHD20_SD_SH720_PAL, //960h*576i				
	AHD20_SD_H1280_NT,				
	AHD20_SD_H1280_PAL,				
	AHD20_SD_H1440_NT,				
	AHD20_SD_H1440_PAL,				
	AHD20_SD_H960_EX_NT,  //1920h*480i						
	AHD20_SD_H960_EX_PAL, //1920h*576i					
	AHD20_SD_H960_2EX_NT, 			
	AHD20_SD_H960_2EX_PAL,		
	AHD20_SD_H960_2EX_Btype_NT,	 //3840h*480i		
	AHD20_SD_H960_2EX_Btype_PAL, //3840h*576i			

	AHD20_1080P_60P, // For Test			
	AHD20_1080P_50P, // For Test			

	AHD20_1080P_30P,					
	AHD20_1080P_25P,                			
	AHD20_720P_60P,                			
	AHD20_720P_50P,                 			
	AHD20_720P_30P,                 			
	AHD20_720P_25P,                 			
	AHD20_720P_30P_EX,              		
	AHD20_720P_25P_EX,              		
	AHD20_720P_30P_EX_Btype,  //2560*720P       		
	AHD20_720P_25P_EX_Btype,  //2560*720P     		

	AHD30_4M_30P,						
	AHD30_4M_25P,                   			
	AHD30_4M_15P,                   			
	AHD30_3M_30P,                   			
	AHD30_3M_25P,                   			
	AHD30_3M_18P,                   			//2048 x 1536
	AHD30_5M_12_5P,					//2592 x 1944
	AHD30_5M_20P,                  			//2592 x 1944
                                  
                                  
	AHD30_5_3M_20P,	
	AHD30_6M_18P,         
	AHD30_6M_20P,         
	AHD30_8M_X_30P,     
	AHD30_8M_X_25P,     
	AHD30_8M_7_5P,       
	AHD30_8M_12_5P,     
	AHD30_8M_15P,         
                                  
	TVI_FHD_30P,            
	TVI_FHD_25P,		
	TVI_HD_60P,             
	TVI_HD_50P,             
	TVI_HD_30P,             
	TVI_HD_25P,             
	TVI_HD_30P_EX,		
	TVI_HD_25P_EX,      
	TVI_HD_B_30P,        
	TVI_HD_B_25P,        
	TVI_HD_B_30P_EX,  
	TVI_HD_B_25P_EX,
	TVI_3M_18P,       					//1920 x 1536    
	TVI_5M_12_5P,        
	TVI_5M_20P,           
	TVI_4M_30P,           
	TVI_4M_25P,           
	TVI_4M_15P,           
	TVI_8M_15P, 
	TVI_8M_12_5P,
                                  
	CVI_FHD_30P,          
	CVI_FHD_25P,          
	CVI_HD_60P,		
	CVI_HD_50P,           
	CVI_HD_30P,           
	CVI_HD_25P,		
	CVI_HD_30P_EX,      
	CVI_HD_25P_EX,      
	CVI_4M_30P,           
	CVI_4M_25P,           
	CVI_8M_15P,           
	CVI_8M_12_5P,        

	VIDEORX_VIVO_CH_FORMATDEF_MAX,

} VIDEORX_VIVO_CH_FORMATDEF; //NC_VIVO_CH_FORMATDEF

typedef struct VDEC_DEV_INFORM_S{

	unsigned char chip_id[4];
	unsigned char chip_rev[4];
	unsigned char chip_addr[4];

	unsigned char Total_Port_Num;
	unsigned char Total_Chip_Cnt;

}VDEC_DEV_INFORM_S;

typedef struct _VIDEORX_INFORMATION_S //_NVP6158_INFORMATION_S
{
	unsigned char			ch;
	unsigned char 			ch_switch_dis;//bit表示：0:允许切换，1:不允许切换
	VIDEORX_VIVO_CH_FORMATDEF 	curvideofmt[ 16 ];
	VIDEORX_VIVO_CH_FORMATDEF 	prevideofmt[ 16 ];
	unsigned char 		 	curvideoloss[ 16 ];
	unsigned char			vfc[16];
	unsigned char			debounce[16][5];
	unsigned char			debounceidx[16];
	VDEC_DEV_INFORM_S 		chipinform;

} VIDEORX_INFORMATION_S; //NVP6158_INFORMATION_S


struct backend_frmcnt //t7_frmcnt
{
	unsigned char ch_num;	
	unsigned long ch_frmcnt[8];	
};

enum rx_ch_reso {
	RX_CH_RESO_720P_60 = 0,
	RX_CH_RESO_720P_50,
	RX_CH_RESO_1080P_30,
	RX_CH_RESO_1080P_25,
	RX_CH_RESO_720P_30,
	RX_CH_RESO_720P_25,
	RX_CH_RESO_SD,
	RX_CH_RESO_INVALID,
};

struct videorx_support_input //nvp_support_input
{
	unsigned char ch_num;	
	unsigned char ch_spt[8];

	enum rx_ch_reso ch_reso[8];
};

typedef struct videorx_video_out_delay //nvp_video_out_delay
{
	unsigned char ch;
	unsigned char h_delay;
	unsigned char v_delay;
}VIDEORX_VIDEO_OUT_DELAY;//NVP_VIDEO_OUT_DELAY

#endif /*__VIDEORX_COMMON_H__*/

