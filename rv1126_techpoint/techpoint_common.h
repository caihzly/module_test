/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Rockchip CIF Driver
 *
 * Copyright (C) 2021 Rockchip Electronics Co., Ltd.
 */

#ifndef _TECHPOINT_COMMON_H
#define _TECHPOINT_COMMON_H

// #define DEBUG
#include <linux/clk.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/pm_runtime.h>
#include <linux/regulator/consumer.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/rk-camera-module.h>
#include <media/media-entity.h>
#include <media/v4l2-async.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-subdev.h>
#include <linux/pinctrl/consumer.h>
#include <linux/rk-preisp.h>
#include <linux/sched.h>
#include <linux/kthread.h>

#define DRIVER_VERSION  KERNEL_VERSION(0, 0x01, 0x0)

#define TECHPOINT_CAMERA_XVCLK_FREQ		"xvclk_freq"
#define TECHPOINT_CHANNEL_NUMS			"channel_nums"

#define TECHPOINT_TEST_PATTERN	0
#define DEF_1080P		1
#define TECHPOINT_SHARING_POWER	1

#define INPUT_STATUS_REG	0x01
#define PAGE_REG			0x40
#define CHIP_ID_H_REG		0xFE
#define CHIP_ID_L_REG		0xFF

#define INPUT_STATUS_MASK	0x80

enum rx_video_mode{
#if DEF_1080P
	VIDEO_MODE_1080P_25FPS = 0,
	VIDEO_MODE_1080P_30FPS,
#endif

	VIDEO_MODE_720P_25FPS,
	VIDEO_MODE_720P_30FPS,
};

enum techpoint_chips {
	CHIP_UNKNOWN = 0,
	CHIP_TP2855,
	CHIP_TP2815,
	CHIP_TP9930,
};

enum techpoint_input_type {
	TECHPOINT_MIPI = 0,
	TECHPOINT_DVP_BT1120,
	TECHPOINT_DVP_BT565,
};

enum techpoint_support_std{
    STD_HDA = 0, //AHD
    STD_TVI,
};

enum techpoint_support_reso {
	TECHPOINT_S_RESO_720P_25 = 0,
	TECHPOINT_S_RESO_1080P_25,
	TECHPOINT_S_RESO_720P_30,
	TECHPOINT_S_RESO_1080P_30,
#if 1 //Add by caihz
	TECHPOINT_S_RESO_SD,
	TECHPOINT_S_RESO_NO_SUPPORT,
#endif
	TECHPOINT_S_RESO_NUMS,
};

enum techpoint_max_pad {
	PAD0 = 0,
	PAD1,
	PAD2,
	PAD3,
	PAD_MAX,
};

struct techpoint_video_modes {
	u32 bus_fmt;
	u32 width;
	u32 height;
	struct v4l2_fract max_fps;
	u32 bpp;
	u32 lane;
	s64 link_freq_value;
	u32 vc[PAD_MAX];
	const struct regval *common_reg_list;
	int common_reg_size;
	enum techpoint_support_reso channel_reso[PAD_MAX];
};

/* Audio output port formats */
enum techpoint_audfmts {
	AUDFMT_I2S = 0,
	AUDFMT_DSP,
};

struct techpoint_audio {
	enum techpoint_audfmts audfmt;
	int mclk_fs;
	int cascade_num;
	int cascade_order;
	int slave_num;
	struct techpoint	*slave_tp[3];
};

struct techpoint {
	struct device dev;
	struct i2c_client *client;
	struct clk *xvclk;
	struct gpio_desc *reset_gpio;
	struct gpio_desc *power_gpio;
	struct pinctrl *pinctrl;
	struct pinctrl_state *pins_sleep;
	struct pinctrl_state *pins_default;
	struct media_pad pad[PAD_MAX];
	struct v4l2_subdev subdev;
	struct v4l2_ctrl *pixel_rate_ctrl;
	struct v4l2_ctrl *link_freq_ctrl;
	struct v4l2_ctrl_handler ctrl_handler;
	struct mutex mutex;
	struct regulator_bulk_data *supplies;
	u32 xvclk_freq_value;

	struct techpoint_audio *audio_in;
	struct techpoint_audio *audio_out;
	int i2c_idx;

	u32	channel_nums;

	enum techpoint_chips chip_id;
	struct techpoint_video_modes *video_modes;
	const struct techpoint_video_modes *cur_video_mode;
	u32 video_modes_num;
	enum techpoint_input_type input_type;

	u32 module_index;
	const char *module_facing;
	const char *module_name;
	const char *len_name;
	bool power_on;
	bool streaming;
	bool do_reset;

	u8 detect_status[PAD_MAX];
	struct task_struct *detect_thread;
};

enum rx_video_mode get_rx_tp9930_video_mode(void);

#endif // _TECHPOINT_COMMON_H
