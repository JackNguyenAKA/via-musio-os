/*
 * Copyright (C) 2010-2015 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/*!
 * @defgroup Framebuffer Framebuffer Driver for tc358778xbg.
 */

/*!
 * @file mxsfb_tc358778xbg.c
 *
 * @brief  Frame buffer driver for tc358778xbg
 *
 * @ingroup Framebuffer
 */

/*!
 * Include files
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/console.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/i2c.h>
#include <linux/fsl_devices.h>
#include <linux/interrupt.h>
#include <linux/reset.h>
#include <asm/mach-types.h>
#include <video/mxc_edid.h>
#include <linux/switch.h>
#include "mxcfb_tc358778xbg.h"
#include <linux/gpio.h>
#include <linux/proc_fs.h>

#undef DEBUG
//#define DEBUG
// #define DEBUG_DETAIL
//#define CONFIG_VIA_PROC

#define DRV_NAME "tc358778xbg"

struct tc358778xbg_data {
	struct i2c_client *client;
	struct fb_info *fbi;
	bool dft_mode_set;
	const char *mode_str;
	int bits_per_pixel;
	u32 yres_virtual;
	int init_flag;
} tc358778xbg;

static void tc358778xbg_dispon(void);
static void tc358778xbg_dispoff(void);
static void tc358778xbg_init(void);
static void tc358778xbg_suspend(void);
static void tc358778xbg_dump_regs(int index, int len, int ulen);
static void tc358778xbg_read_word(int index, u16 *val);
static void tc358778xbg_write_word(int index, u16 val);
static void tc358778xbg_init(void);
void tc358778xbg_init2(void);
static void txdt550uzpa_init(void);

static void tc358778xbg_read_word(int index, u16 *val)
{
	u8 au8RegBuf[2] = {0};
	u8 *pu8Rd;

	au8RegBuf[0] = index >> 8;
	au8RegBuf[1] = index & 0xff;

	if (2 != i2c_master_send(tc358778xbg.client, au8RegBuf, 2)) {
		printk("*E* %s(0x%x,0x%x)\n", __func__, index, *val);
		return;
	}

	if (2 != i2c_master_recv(tc358778xbg.client,(char *) val, 2)) {
		printk("*E* %s(0x%x,0x%x)\n", __func__, index, *val);
		return;
	}
	pu8Rd = (u8 *) val;
#ifdef DEBUG_DETAIL
	printk("[sam] read word(0x%x,0x%x),%x %x\n", index, *val,
					pu8Rd[0], pu8Rd[1]);
#endif
	swap(pu8Rd[0],pu8Rd[1]);
}

static void tc358778xbg_write_word(int index, u16 val)
{
	u8 au8Buf[4] = {0};
	int ret;

	au8Buf[0] = index >> 8;
	au8Buf[1] = index & 0xff;
	au8Buf[2] = val >> 8;	
	au8Buf[3] = val & 0xff;

	ret = i2c_master_send(tc358778xbg.client, au8Buf, 4);
	if (ret < 0) {
		printk("*E* %s(0x%x,0x%x),ret %d\n", __func__, index, val, ret);
		return;
	}

#ifdef DEBUG_DETAIL
	printk("i2cset -f -y 2 0x0E 0x%02X 0x%02X 0x%02X 0x%02X i\n",
		au8Buf[0], au8Buf[1], au8Buf[2], au8Buf[3]);
#endif
	return;
}

static void tc358778xbg_read_dword(int index, u32 *val)
{
	u8 au8RegBuf[2] = {0};
	u8 *pu8Rd;

	au8RegBuf[0] = index >> 8;
	au8RegBuf[1] = index & 0xff;

	if (2 != i2c_master_send(tc358778xbg.client, au8RegBuf, 2)) {
		printk("*E* %s(0x%x,0x%x)\n", __func__, index, *val);
		return;
	}

	if (4 != i2c_master_recv(tc358778xbg.client, (char *) val, 4)) {
		printk("*E* %s(0x%x,0x%x)\n", __func__, index, *val);
		return;
	}
	
	pu8Rd = (u8 *) val;
#ifdef DEBUG_DETAIL
	printk("[sam] read dword(0x%x,0x%x),%x %x %x %x\n", index, *val,
				pu8Rd[0], pu8Rd[1], pu8Rd[2], pu8Rd[3]);
#endif
	swap(pu8Rd[0],pu8Rd[1]);
	swap(pu8Rd[2],pu8Rd[3]);
}

static void tc358778xbg_write_dword(int index, u32 val)
{
	u8 au8Buf[6] = {0};
	int ret;

	au8Buf[0] = index >> 8;
	au8Buf[1] = index & 0xff;
	au8Buf[2] = (val & 0xff00) >> 8;	
	au8Buf[3] = val & 0xff;
	au8Buf[4] = (val & 0xff000000) >> 24;
	au8Buf[5] = (val & 0xff0000) >> 16;

	ret = i2c_master_send(tc358778xbg.client, au8Buf, 6);
	if (ret < 0) {
		printk("*E* %s(0x%x,0x%x),ret %d\n", __func__, index, val, ret);
		return;
	}
#ifdef DEBUG_DETAIL
	printk("[sam] write dword(0x%x,0x%x),%x %x %x %x %x %x\n",
		index, val, au8Buf[0], au8Buf[1], au8Buf[2],
		au8Buf[3], au8Buf[4], au8Buf[5]);
#endif
	return;
}

#if 0
static void tc358778xbg_read_regs(int index, u8 *buf, int len)
{
	int ret;

	ret = i2c_smbus_read_i2c_block_data(tc358778xbg.client, index, len, buf);
	if (ret < 0) {
		printk("*E* %s(0x%x,%d),ret %d\n", __func__, index, len, ret);
	}
}

static void tc358778xbg_write_regs(int index, u8 *buf, int len)
{
	int ret;

	i2c_smbus_write_i2c_block_data(tc358778xbg.client, index, len, buf);
	if (ret) {
		printk("*E* %s(0x%x,%d),ret %d\n", __func__, index, len, ret);
	}
}
#endif

static void tc358778xbg_dump_regs(int index, int len, int ulen)
{
	u8 buf[256];
	int i;
	int offset;
	
	for (i = 0; i < len/ulen; i++) {
		offset = i * ulen;
		if ((offset % 16) == 0)
			printk("\n 0x%04x :", index + offset);
		else if ((offset % 8) == 0)
			printk(" -");
		switch(ulen) {
		case 2:
			tc358778xbg_read_word(index + offset, (u16 *) &buf[offset]);
			printk(" %02x %02x", buf[offset], buf[offset + 1]);
			break;
		case 4:
			tc358778xbg_read_dword(index + offset, (u32 *) &buf[offset]);
			printk(" %02x %02x %02x %02x", buf[offset], buf[offset + 1],
				buf[offset + 2], buf[offset + 3]);
			break;
		default:
			break;
		}
	}
	printk("\n");
}

int tc358778xbg_check_ID(void)
{
	u8 buf[2];

	tc358778xbg_read_word(REG_CHIPID, (u16 *) buf);
#ifdef DEBUG
	printk("[sam] %s %d(0x%x,0x%x)\n", __func__, __LINE__, buf[0], buf[1]);
#endif
	if (buf[0] != 0x01)
		return -1;
	if (buf[1] != 0x44)
		return -1;
	return 0;
}

#if 0
void tc358778xbg_reset(void)
{
	tc358778xbg_write_word(REG_SYSCTL, 1);
	mdelay(1);
	tc358778xbg_write_word(REG_SYSCTL, 0);
}
#endif

void tc358778xbg_write_cmd(u8 cmd, u8 *buf, int len)
{
	u16 data;
#ifdef DEBUG
	printk("[sam] %s (cmd 0x%x,len %d, 0x%x)\n", __func__, cmd, len, buf[0]);
#endif

	/* REG_DSICMD_TYPE */
	data = (len) ? 0x4000 : 0x1000; /* 0x10:DSI short packet,0x40:DSI long packet */
	data |= cmd;
	tc358778xbg_write_word(REG_DSICMD_TYPE, data);

	/* REG_DSICMD_WC */
	data = len;
	tc358778xbg_write_word(REG_DSICMD_WC, data);	

	if (len) {
		int i, cnt;

		cnt = len / 2;
		cnt += (len % 2) ? 1 : 0;
		for(i = 0; i < cnt; i++) {
			data = buf[2 * i] + (buf[2 * i + 1] << 8);
			tc358778xbg_write_word(REG_DSICMD_WD0 + 2 * i, data);
#ifdef DEBUG_DETAIL			
		printk("[sam] wr cmd(%d) 0x%x = 0x%x\n", i,
			REG_DSICMD_WD0 + 2 * i, data);
#endif
		}
	} else {
		data = buf[0] + (buf[1] << 8);
		tc358778xbg_write_word(REG_DSICMD_WD0, data);
#ifdef DEBUG_DETAIL
		printk("[sam] wr cmd 0x%x = 0x%x\n", REG_DSICMD_WD0, data);
#endif
	}

	/* Start transfer */
	data = 0x1;
	tc358778xbg_write_word(REG_DSICMD_TX, data);
}

#if 0
void tc58778xbg_read_cmd(u8 cmd, u8 *buf, int len, int rx_len)
{
	u16 data, data2;
	int i;

	/* Reset RX-FIFO */
	tc358778xbg_write_word(REG_DSI_RESET, 0x0010);
	tc358778xbg_write_word(0x506, 0x0000);

	/* REG_DSICMD_TYPE */
	data = (len) ? 0x4000 : 0x1000; /* 0x10:DSI short pkt,0x40:DSI long pkt */
	data |= cmd;
	tc358778xbg_write_word(REG_DSICMD_TYPE, data);

	/* REG_DSICMD_WC */
	data = len;
	tc358778xbg_write_word(REG_DSICMD_WC, data);	

	data = buf[0] + (buf[1] << 8);
	tc358778xbg_write_word(REG_DSICMD_WD0, data);

	/* Start transfer */
	data = 0x1;
	tc358778xbg_write_word(REG_DSICMD_TX, data);

	udelay(100);

	do {
		tc358778xbg_read_word(REG_DSI_STATUS, &data);
		tc358778xbg_read_word(REG_DSI_STATUS + 2, &data2);
	} while ((data & 0x20) == 0);

	tc358778xbg_read_word(REG_DSI_DSICMD_RXFIFO, &data);
	tc358778xbg_read_word(REG_DSI_DSICMD_RXFIFO + 2, &data2);
	
	buf[0] = data & 0xFF;
	buf[1] = (data & 0xFF00) >> 8;
	buf[2] = data2 & 0xFF;
	buf[3] = (data2 & 0xFF00) >> 8;

	if ((data == 0) && (data2 == 0)) {
		tc358778xbg_read_word(REG_DSI_DSI_ACKERR, &data);
		tc358778xbg_read_word(REG_DSI_DSI_ACKERR + 2, &data2);
		printk("*E* %s(0x%x,0x%x)\n", __func__, data, data2);
		return;
	}

	len = rx_len / 4;
	if (rx_len % 4)
		len += 1;

	for (i = 0; i < len; i++) {
		tc358778xbg_read_word(REG_DSI_DSICMD_RXFIFO,
			(u16 *) &buf[4 * i + 4]);
		tc358778xbg_read_word(REG_DSI_DSICMD_RXFIFO + 2,
			(u16 *) &buf[4 * i + 6]);
	}
}

static void tc358778xbg_setup(struct fb_info *fbi)
{
	u8 data[4];
	u32 refresh;
	int temp;
	u8 *tmp;

	printk("[sam] %s %d\n", __func__, __LINE__);

	dev_dbg(&tc358778xbg.client->dev, "tc358778xbg: setup..\n");

	/* Configures registers */
	/* Enable Parallel input port */

	/* REG_SYSCTL */
	data[0] = 0x44; /* index inc, DE hi, Vsync low, Parallel enable */
	data[1] = 0x00; /* Data Fmt Mode 0 */
	tc358778xbg_write_regs(REG_SYSCTL, data, 2);

	/* REG_DATAFMT */
	data[0] = 0x3; /* Data Type ID, TX enable */
	data[1] = 0x3; /* RGB888 */
	tc358778xbg_write_regs(REG_DATAFMT, data, 2);

	/* PLL */
	data[0] = 0x3; /* xFBD */
	data[1] = 0x0; /* /PRD */
	tc358778xbg_write_regs(REG_PLLCTL0, data, 2);

	data[0] = 0x1; /* PLL enable */
	data[1] = 0x8; /* freq range */
	tc358778xbg_write_regs(REG_PLLCTL1, data, 2);

	/* REG_DSI_EVENT */
	data[0] = 0x0; /* 0:Pulse mode, 1:Event mode */
	data[1] = 0x0;
	tc358778xbg_write_regs(REG_DSI_EVENT, data, 2);

	/* REG_DSI_VSW */
	temp = fbi->var.vsync_len;
	data[0] = temp & 0xFF;
	data[1] = (temp & 0xFF00) >> 8;
	tc358778xbg_write_regs(REG_DSI_EVENT, data, 2);
	
	/* REG_DSI_VBPR */
	temp = fbi->var.upper_margin;
	data[0] = temp & 0xFF;
	data[1] = (temp & 0xFF00) >> 8;
	tc358778xbg_write_regs(REG_DSI_VBPR, data, 2);

	/* REG_DSI_VACT */
	temp = fbi->var.height;
	data[0] = temp & 0xFF;
	data[1] = (temp & 0xFF00) >> 8;
	tc358778xbg_write_regs(REG_DSI_VACT, data, 2);
	
	/* REG_DSI_HSW */
	temp = fbi->var.hsync_len;
	data[0] = temp & 0xFF;
	data[1] = (temp & 0xFF00) >> 8;
	tc358778xbg_write_regs(REG_DSI_HSW, data, 2);

	/* REG_DSI_HBPR */
	temp = fbi->var.left_margin;
	data[0] = temp & 0xFF;
	data[1] = (temp & 0xFF00) >> 8;
	tc358778xbg_write_regs(REG_DSI_HBPR, data, 2);
	
	/* REG_DSI_HACT */
	temp = fbi->var.width;
	data[0] = temp & 0xFF;
	data[1] = (temp & 0xFF00) >> 8;
	tc358778xbg_write_regs(REG_DSI_HACT, data, 2);

	/* input bus/pixel: full pixel wide (24bit), rising edge */
	i2c_smbus_write_byte_data(tc358778xbg.client, 0x08, 0x70);
	/* Set input format to RGB */
	i2c_smbus_write_byte_data(tc358778xbg.client, 0x09, 0x00);
	/* set output format to RGB */
	i2c_smbus_write_byte_data(tc358778xbg.client, 0x0A, 0x00);
}
#endif

struct txdt550uzpa_cmd_s {
	char cmd;
	int len;
	char data[];
};

#if 0
struct txdt550uzpa_cmd_s txdt550uzpa_cmd1 = 
	{0xB0,3,{0x98,0x85,0x0A}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd2 = 
	{0xC4,7,{0x70,0x19,0x23,0x00,0x0F,0x0F,0x00}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd3 = 
	{0xD0,6,{0x33,0x05,0x33,0x66,0x62,0x00}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd4 = 
	{0xD2,4,{0x03,0x03,0x2A,0x22}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd5 = 
	{0xD3,9,{0x33,0x33,0x05,0x03,0x59,0x59,0x22,0x26,0x22}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd6 = 
	{0xD5,10,{0x00,0x00,0x00,0x00,0x01,0x8D,0x01,0x8D,0x01,0x8D}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd7 = 
	{0xEC,7,{0x76,0x1E,0x32,0x00,0x46,0x00,0x02}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd8 = 
	{0xEF,8,{0x3C,0x05,0x52,0x13,0xE1,0x33,0x5b,0x09}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd9 = 
	{0xD6,12,{0x00,0x00,0x08,0x17,0x23,0x65,0x77,0x44,0x87,0x00,0x00,0x09}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd10 = 
	{0xEB,35,{0x9b,0xC7,0x73,0x00,0x58,0x55,0x55,0x55,0x55,0x54,0x00,0x00,0x00,0x00,0x00,0x25,0x4D,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0x55,0x55,0x55,0x55,0x32,0x77,0x55,0x43,0x55,0x5E,0xFF,0x55}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd11 = 
	{0xE5,73,{0x36,0x36,0xA1,0xF6,0xF6,0x47,0x07,0x55,0x15,0x63,0x23,0x71,0x31,0x6E,0x36,0x85,0x36,0x36,0x36,0x36,0x36,0x36,0xA8,0xF6,0xF6,0x4E,0x0E,0x5C,0x1C,0x6A,0x2A,0x78,0x38,0x76,0x35,0x8C,0x36,0x36,0x36,0x36,0x18,0x70,0x61,0x00,0x4E,0xBB,0x70,0x80,0x00,0x4E,0xBB,0xF7,0x00,0x4E,0xBB,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd12 = 
	{0xEA,66,{0x51,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x00,0x00,0x00,0x96,0x95,0x10,0x11,0x00,0x0A,0x00,0x0F,0x00,0x00,0x00,0x70,0x01,0x10,0x00,0x40,0x80,0xC0,0x00,0x00,0x01,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,0xCC,0xCC,0x22,0x33,0x33,0x00,0x11,0x00,0x11,0x00,0x11,0x00,0x11,0xCC,0xDD,0x22,0xCC,0xCC,0xCC,0xCC}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd13 = 
	{0xED,23,{0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd14 = 
	{0xEE,19,{0x22,0x10,0x02,0x02,0x0F,0x40,0x00,0x07,0x00,0x04,0x00,0x00,0xC0,0xB9,0x77,0x00,0x55,0x05,0x1F}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd15 = 
	{/*0xC7*/ 0,122,{0x00,0x76,0x00,0x6F,0x00,0x9B,0x00,0xB8,0x00,0xCF,0x00,0xE3,0x00,0xF4,0x01,0x03,0x01,0x11,0x01,0x3E,0x01,0x62,0x01,0x9B,0x01,0xC7,0x02,0x0C,0x02,0x43,0x02,0x45,0x02,0x78,0x02,0xAF,0x02,0xD3,0x03,0x02,0x03,0x20,0x03,0x46,0x03,0x52,0x03,0x5F,0x03,0x6C,0x03,0x7C,0x03,0x8F,0x03,0xA6,0x03,0xC2,0x03,0xD0,0x00,0x17,0x00,0x6F,0x00,0x9B,0x00,0xB8,0x00,0xCF,0x00,0xE3,0x00,0xF4,0x01,0x03,0x01,0x11,0x01,0x3E,0x01,0x62,0x01,0x9B,0x01,0xC7,0x02,0x0C,0x02,0x43,0x02,0x45,0x02,0x78,0x02,0xAF,0x02,0xD3,0x03,0x02,0x03,0x20,0x03,0x46,0x03,0x52,0x03,0x5F,0x03,0x6C,0x03,0x7C,0x03,0x8F,0x03,0xA6,0x03,0xC2,0x03,0xD0,0x01,0x01}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd16 = 
	{0xB0,1,{0x11}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd17 = 
	{0x11,1,{0x00}};
// Delay_ms(120);
struct txdt550uzpa_cmd_s txdt550uzpa_cmd18 = 
	{0x29,1,{0x00}};
struct txdt550uzpa_cmd_s *txdt550uzpa_cmd_table[] = {
	&txdt550uzpa_cmd1,
	&txdt550uzpa_cmd2,
	&txdt550uzpa_cmd3,
	&txdt550uzpa_cmd4,
	&txdt550uzpa_cmd5,
	&txdt550uzpa_cmd6,
	&txdt550uzpa_cmd7,
	&txdt550uzpa_cmd8,
	&txdt550uzpa_cmd9,
	&txdt550uzpa_cmd10,
	&txdt550uzpa_cmd11,
	&txdt550uzpa_cmd12,
	&txdt550uzpa_cmd13,
	&txdt550uzpa_cmd14,
	&txdt550uzpa_cmd15,
	&txdt550uzpa_cmd16,
	&txdt550uzpa_cmd17,
	&txdt550uzpa_cmd18,
	0
};
#else
struct txdt550uzpa_cmd_s txdt550uzpa_cmd1 = 
	{0xB0,3,{0x98,0x85,0x0A}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd1_2 = 
	{0xF5,1,{0x85}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd1_3 = 
	{0xD7,1,{0x27}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd1_4 = 
	{0xF5,1,{0x00}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd2 = 
	{0xC4,7,{0x70,0x19,0x23,0x00,0x0F,0x0F,0x00}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd3 = 
	{0xD0,6,{0x33,0x05,0x33,0x66,0x62,0x00}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd4 = 
	{0xD2,4,{0x03,0x03,0x2A,0x22}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd5 = 
	{0xD3,7,{0x33,0x33,0x05,0x03,0x59,0x59,0x22}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd5_2 = 
	{0xD3,2,{0x26,0x22}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd6 = 
	{0xD5,7,{0x00,0x00,0x00,0x00,0x01,0x8D,0x01}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd6_2 = 
	{0xD5,3,{0x8D,0x01,0x8D}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd7 = 
	{0xEC,7,{0x76,0x1E,0x32,0x00,0x46,0x00,0x02}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd8 = 
	{0xEF,7,{0x3C,0x05,0x52,0x13,0xE1,0x33,0x5b}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd8_2 = 
	{0xEF,1,{0x09}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd9 = 
	{0xD6,7,{0x00,0x00,0x08,0x17,0x23,0x65,0x77}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd9_2 = 
	{0xD6,5,{0x44,0x87,0x00,0x00,0x09}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd10 = 
	{0xEB,7,{0x9b,0xC7,0x73,0x00,0x58,0x55,0x55}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd10_2 = 
	{0xEB,7,{0x55,0x55,0x54,0x00,0x00,0x00,0x00}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd10_3 = 
	{0xEB,7,{0x00,0x25,0x4D,0x0F,0xFF,0xFF,0xFF}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd10_4 = 
	{0xEB,7,{0xFF,0xFF,0x55,0x55,0x55,0x55,0x32}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd10_5 = 
	{0xEB,7,{0x77,0x55,0x43,0x55,0x5E,0xFF,0x55}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd11 = 
	{0xE5,7,{0x36,0x36,0xA1,0xF6,0xF6,0x47,0x07}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd11_2 = 
	{0xE5,7,{0x55,0x15,0x63,0x23,0x71,0x31,0x6E}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd11_3 = 
	{0xE5,7,{0x36,0x85,0x36,0x36,0x36,0x36,0x36}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd11_4 = 
	{0xE5,7,{0x36,0xA8,0xF6,0xF6,0x4E,0x0E,0x5C}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd11_5 = 
	{0xE5,7,{0x1C,0x6A,0x2A,0x78,0x38,0x76,0x35}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd11_6 = 
	{0xE5,7,{0x8C,0x36,0x36,0x36,0x36,0x18,0x70}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd11_7 = 
	{0xE5,7,{0x61,0x00,0x4E,0xBB,0x70,0x80,0x00}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd11_8 = 
	{0xE5,7,{0x4E,0xBB,0xF7,0x00,0x4E,0xBB,0x00}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd11_9 = 
	{0xE5,7,{0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd11_10 = 
	{0xE5,7,{0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd11_11 = 
	{0xE5,3,{0x00,0x00,0x07}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd12 = 
	{0xEA,7,{0x51,0x00,0x00,0x00,0x00,0x00,0x00}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd12_2 = 
	{0xEA,7,{0x40,0x00,0x00,0x00,0x00,0x00,0x00}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd12_3 = 
	{0xEA,7,{0x0F,0x00,0x00,0x00,0x96,0x95,0x10}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd12_4 = 
	{0xEA,7,{0x11,0x00,0x0A,0x00,0x0F,0x00,0x00}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd12_5 = 
	{0xEA,7,{0x00,0x70,0x01,0x10,0x00,0x40,0x80}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd12_6 = 
	{0xEA,7,{0xC0,0x00,0x00,0x01,0x23,0x45,0x67}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd12_7 = 
	{0xEA,7,{0x89,0xAB,0xCD,0xEF,0xCC,0xCC,0x22}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd12_8 = 
	{0xEA,7,{0x33,0x33,0x00,0x11,0x00,0x11,0x00}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd12_9 = 
	{0xEA,7,{0x11,0x00,0x11,0xCC,0xDD,0x22,0xCC}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd12_10 = 
	{0xEA,3,{0xCC,0xCC,0xCC}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd13 =
	{0xED,7,{0x00,0x01,0x00,0x00,0x00,0x00,0x00}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd13_2 =
	{0xED,7,{0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd13_3 =
	{0xED,7,{0x00,0x00,0x00,0x00,0x00,0x00,0x00}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd13_4 =
	{0xED,2,{0x00,0x40}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd14 = 
	{0xEE,7,{0x22,0x10,0x02,0x02,0x0F,0x40,0x00}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd14_2 = 
	{0xEE,7,{0x07,0x00,0x04,0x00,0x00,0xC0,0xB9}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd14_3 = 
	{0xEE,5,{0x77,0x00,0x55,0x05,0x1F}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd15 = 
	{0xC7,7,{0x00,0x76,0x00,0x6F,0x00,0x9B,0x00}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd15_2 = 
	{0xC7,7,{0xB8,0x00,0xCF,0x00,0xE3,0x00,0xF4}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd15_3 = 
	{0xC7,7,{0x01,0x03,0x01,0x11,0x01,0x3E,0x01}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd15_4 = 
	{0xC7,7,{0x62,0x01,0x9B,0x01,0xC7,0x02,0x0C}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd15_5 = 
	{0xC7,7,{0x02,0x43,0x02,0x45,0x02,0x78,0x02}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd15_6 = 
	{0xC7,7,{0xAF,0x02,0xD3,0x03,0x02,0x03,0x20}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd15_7 = 
	{0xC7,7,{0x03,0x46,0x03,0x52,0x03,0x5F,0x03}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd15_8 = 
	{0xC7,7,{0x6C,0x03,0x7C,0x03,0x8F,0x03,0xA6}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd15_9 = 
	{0xC7,7,{0x03,0xC2,0x03,0xD0,0x00,0x17,0x00}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd15_10 = 
	{0xC7,7,{0x6F,0x00,0x9B,0x00,0xB8,0x00,0xCF}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd15_11 = 
	{0xC7,7,{0x00,0xE3,0x00,0xF4,0x01,0x03,0x01}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd15_12 = 
	{0xC7,7,{0x11,0x01,0x3E,0x01,0x62,0x01,0x9B}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd15_13 = 
	{0xC7,7,{0x01,0xC7,0x02,0x0C,0x02,0x43,0x02}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd15_14 = 
	{0xC7,7,{0x45,0x02,0x78,0x02,0xAF,0x02,0xD3}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd15_15 = 
	{0xC7,7,{0x03,0x02,0x03,0x20,0x03,0x46,0x03}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd15_16 = 
	{0xC7,7,{0x52,0x03,0x5F,0x03,0x6C,0x03,0x7C}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd15_17 = 
	{0xC7,7,{0x03,0x8F,0x03,0xA6,0x03,0xC2,0x03}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd15_18 = 
	{0xC7,3,{0xD0,0x01,0x01}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd16 = 
	{0xB0,1,{0x11}};
struct txdt550uzpa_cmd_s txdt550uzpa_cmd17 = 
	{0x11,1,{0x00}};
// Delay_ms(120);
struct txdt550uzpa_cmd_s txdt550uzpa_cmd18 = 
	{0x29,1,{0x00}};

struct txdt550uzpa_cmd_s *txdt550uzpa_cmd_table[] = {
	&txdt550uzpa_cmd1,
	&txdt550uzpa_cmd1_2,
	&txdt550uzpa_cmd1_3,
	&txdt550uzpa_cmd1_4,
	&txdt550uzpa_cmd2,
	&txdt550uzpa_cmd3,
	&txdt550uzpa_cmd4,
	&txdt550uzpa_cmd5,
	&txdt550uzpa_cmd5_2,
	&txdt550uzpa_cmd6,
	&txdt550uzpa_cmd6_2,
	&txdt550uzpa_cmd7,
	&txdt550uzpa_cmd8,
	&txdt550uzpa_cmd8_2,
	&txdt550uzpa_cmd9,
	&txdt550uzpa_cmd9_2,
	&txdt550uzpa_cmd10,
	&txdt550uzpa_cmd10_2,
	&txdt550uzpa_cmd10_3,
	&txdt550uzpa_cmd10_4,
	&txdt550uzpa_cmd10_5,
	&txdt550uzpa_cmd11,
	&txdt550uzpa_cmd11_2,
	&txdt550uzpa_cmd11_3,
	&txdt550uzpa_cmd11_4,
	&txdt550uzpa_cmd11_5,
	&txdt550uzpa_cmd11_6,
	&txdt550uzpa_cmd11_7,
	&txdt550uzpa_cmd11_8,
	&txdt550uzpa_cmd11_9,
	&txdt550uzpa_cmd11_10,
	&txdt550uzpa_cmd11_11,
	&txdt550uzpa_cmd12,
	&txdt550uzpa_cmd12_2,
	&txdt550uzpa_cmd12_3,
	&txdt550uzpa_cmd12_4,
	&txdt550uzpa_cmd12_5,
	&txdt550uzpa_cmd12_6,
	&txdt550uzpa_cmd12_7,
	&txdt550uzpa_cmd12_8,
	&txdt550uzpa_cmd12_9,
	&txdt550uzpa_cmd12_10,
	&txdt550uzpa_cmd13,
	&txdt550uzpa_cmd13_2,
	&txdt550uzpa_cmd13_3,
	&txdt550uzpa_cmd13_4,
	&txdt550uzpa_cmd14,
	&txdt550uzpa_cmd14_2,
	&txdt550uzpa_cmd14_3,
	&txdt550uzpa_cmd15,
	&txdt550uzpa_cmd15_2,
	&txdt550uzpa_cmd15_3,
	&txdt550uzpa_cmd15_4,
	&txdt550uzpa_cmd15_5,
	&txdt550uzpa_cmd15_6,
	&txdt550uzpa_cmd15_7,
	&txdt550uzpa_cmd15_8,
	&txdt550uzpa_cmd15_9,
	&txdt550uzpa_cmd15_10,
	&txdt550uzpa_cmd15_11,
	&txdt550uzpa_cmd15_12,
	&txdt550uzpa_cmd15_13,
	&txdt550uzpa_cmd15_14,
	&txdt550uzpa_cmd15_15,
	&txdt550uzpa_cmd15_16,
	&txdt550uzpa_cmd15_17,
	&txdt550uzpa_cmd15_18,
	&txdt550uzpa_cmd16,
	&txdt550uzpa_cmd17,
	&txdt550uzpa_cmd18,
	0
};

#endif

void tc358778xbg_write_long_cmd(u8 cmd, u8 *buf, int len)
{
	u16 data;
	int i, cnt;

	tc358778xbg_write_word(0x500, 0x0080);
	tc358778xbg_write_word(0x502, 0xA300);
	tc358778xbg_write_word(0x08, 0x0001);
	data = cmd;
	tc358778xbg_write_word(0x50, data);
	tc358778xbg_write_word(0x22, len);
	tc358778xbg_write_word(0xE0, 0x8000);
	tc358778xbg_write_word(0xE2, 0x0048);
	tc358778xbg_write_word(0xE4, 0x007F);

	cnt = len / 4;
	cnt += (len % 4) ? 1 : 0;
	cnt *= 2;
	for(i = 0; i < cnt; i++) {
		data = buf[2 * i] + (buf[2 * i + 1] << 8);
		tc358778xbg_write_word(0xE8, data);
	}
	tc358778xbg_write_word(0xE0, 0xC000);
	tc358778xbg_write_word(0xE0, 0x0000);
	mdelay(1);
}			

void txdt550uzpa_write_dcs_cmd(struct txdt550uzpa_cmd_s *cmd)
{
	u8 type, buf[150];
	int len;

	if (cmd->cmd == 0)
		return;
	
	switch (cmd->len) {
	case 0:
		type = 0x05;
		len = 0;
		buf[0] = cmd->cmd;
		buf[1] = 0;
		break;
	case 1:
		type = 0x15;
		len = 0;
		buf[0] = cmd->cmd;
		buf[1] = cmd->data[0];
		break;
	default:
		type = 0x39;
		len = cmd->len + 1;
		memset(&buf[0], 0, 150);
		buf[0] = cmd->cmd;
		memcpy(&buf[1], &cmd->data[0], cmd->len);
		break;
	}

	if (len > 8) {
		tc358778xbg_write_long_cmd(type, buf, len);
	} else {
		tc358778xbg_write_cmd(type, buf, len);
	}
}

void txdt550uzpa_write_cmd(struct txdt550uzpa_cmd_s *cmd)
{
	u8 type, buf[150];
	int len;

	if (cmd->cmd == 0)
		return;

	switch (cmd->len) {
	case 0:
		type = 0x03;
		len = 0;
		buf[0] = cmd->cmd;
		buf[1] = 0;
		break;
	case 1:
		type = 0x13;
		len = 0;
		buf[0] = cmd->cmd;
		buf[1] = cmd->data[0];
		break;
	default:
		type = 0x29;
		len = cmd->len + 1;
		memset(&buf[0], 0, 150);
		buf[0] = cmd->cmd;
		memcpy(&buf[1], &cmd->data[0], cmd->len);
		break;
	}

	if (len > 8) {
		tc358778xbg_write_long_cmd(type, buf, len);
	} else {
		tc358778xbg_write_cmd(type, buf, len);
	}
}

#if 0
void txdt550uzpa_read_cmd(int len)
{
	u16 data1, data2;
	int i;

	/* Set Max return size */
	tc358778xbg_write_word(0x602, 0x1037);
	tc358778xbg_write_word(0x604, 0x0000);
	tc358778xbg_write_word(0x610, 0x00FF);
	tc358778xbg_write_word(0x600, 0x0001);
	udelay(100);

	/* Reset RX FIFO */
	tc358778xbg_write_word(0x504, 0x0010);
	tc358778xbg_write_word(0x506, 0x0000);

	/* Generic Short Read (no parameter) */
	/* Generic short read 0x4 */
	tc358778xbg_write_word(0x602, 0x1004);
	tc358778xbg_write_word(0x604, 0x0000);
	tc358778xbg_write_word(0x610, 0x0000);
	tc358778xbg_write_word(0x600, 0x0001);
	udelay(100);

	do {
		tc358778xbg_read_word(0x410, &data1);
		tc358778xbg_read_word(0x412, &data2);
		if (data1 & 0x20)
			break;
	} while(1);

	tc358778xbg_read_word(0x430, &data1); /* DATAID Byte0 */
	tc358778xbg_read_word(0x432, &data2); /* Byte1 ECC */
	for (i = 0; i < len; i+=4) {
		tc358778xbg_read_word(0x430, &data1);
		tc358778xbg_read_word(0x432, &data2);
	}
	/* check error status */
	tc358778xbg_read_word(0x434, &data1);
	tc358778xbg_read_word(0x436, &data2);

}
#endif

static void txdt550uzpa_init(void)
{
#ifdef DEBUG
	printk("[sam] %s %d\n", __func__, __LINE__);
#endif
#if 0
	txdt550uzpa_write_cmd(&txdt550uzpa_cmd1);
	txdt550uzpa_write_cmd(&txdt550uzpa_cmd2);
	txdt550uzpa_write_cmd(&txdt550uzpa_cmd3);
	txdt550uzpa_write_cmd(&txdt550uzpa_cmd4);
	txdt550uzpa_write_cmd(&txdt550uzpa_cmd5);
	txdt550uzpa_write_cmd(&txdt550uzpa_cmd6);
	txdt550uzpa_write_cmd(&txdt550uzpa_cmd7);
	txdt550uzpa_write_cmd(&txdt550uzpa_cmd8);
	txdt550uzpa_write_cmd(&txdt550uzpa_cmd9);
	txdt550uzpa_write_cmd(&txdt550uzpa_cmd10);
	txdt550uzpa_write_cmd(&txdt550uzpa_cmd11);
	txdt550uzpa_write_cmd(&txdt550uzpa_cmd12);
	txdt550uzpa_write_cmd(&txdt550uzpa_cmd13);
	txdt550uzpa_write_cmd(&txdt550uzpa_cmd14);
	txdt550uzpa_write_cmd(&txdt550uzpa_cmd15);
	txdt550uzpa_write_cmd(&txdt550uzpa_cmd16);
	txdt550uzpa_write_dcs_cmd(&txdt550uzpa_cmd17);
	mdelay(120);
	txdt550uzpa_write_dcs_cmd(&txdt550uzpa_cmd18);
#else
txdt550uzpa_write_cmd(&txdt550uzpa_cmd1);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd1_2);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd1_3);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd1_4);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd2);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd3);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd4);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd5);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd5_2);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd6);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd6_2);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd7);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd8);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd8_2);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd9);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd9_2);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd10);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd10_2);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd10_3);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd10_4);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd10_5);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd11);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd11_2);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd11_3);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd11_4);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd11_5);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd11_6);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd11_7);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd11_8);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd11_9);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd11_10);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd11_11);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd12);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd12_2);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd12_3);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd12_4);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd12_5);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd12_6);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd12_7);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd12_8);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd12_9);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd12_10);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd13);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd13_2);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd13_3);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd13_4);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd14);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd14_2);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd14_3);
#if 0
txdt550uzpa_write_cmd(&txdt550uzpa_cmd15);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd15_2);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd15_3);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd15_4);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd15_5);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd15_6);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd15_7);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd15_8);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd15_9);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd15_10);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd15_11);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd15_12);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd15_13);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd15_14);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd15_15);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd15_16);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd15_17);
txdt550uzpa_write_cmd(&txdt550uzpa_cmd15_18);
#endif
txdt550uzpa_write_cmd(&txdt550uzpa_cmd16);
#if 0
txdt550uzpa_write_dcs_cmd(&txdt550uzpa_cmd17);
mdelay(120);
txdt550uzpa_write_dcs_cmd(&txdt550uzpa_cmd18);
#endif
#endif
}

static void txdt550uzpa_init2(void) {
	txdt550uzpa_write_dcs_cmd(&txdt550uzpa_cmd17);
	mdelay(120);
	txdt550uzpa_write_dcs_cmd(&txdt550uzpa_cmd18);
}

static int tc358778xbg_fb_event(struct notifier_block *nb, unsigned long val, void *v)
{
	struct fb_event *event = v;
	struct fb_info *fbi = event->info;

	dev_dbg(&tc358778xbg.client->dev, "%s event=0x%lx\n", __func__, val);

	switch (val) {
	case FB_EVENT_FB_REGISTERED:
		if (tc358778xbg.fbi == NULL)
			tc358778xbg.fbi = fbi;
		break;
	case FB_EVENT_MODE_CHANGE:
		if (tc358778xbg.fbi != NULL)
			tc358778xbg.yres_virtual = tc358778xbg.fbi->var.yres_virtual;
		if (tc358778xbg.init_flag == 0) {
#ifndef CONFIG_VIA_PROC
			tc358778xbg_init();
#endif
			tc358778xbg.init_flag = 1;
		}
		break;
	case FB_EVENT_BLANK:
		if (*((int *)event->data) == FB_BLANK_UNBLANK) {
			tc358778xbg_dispon();
		} else {
			tc358778xbg_dispoff();
		}
		break;
	case FB_EVENT_SUSPEND:
		tc358778xbg_suspend();
		tc358778xbg.init_flag = 0;
		break;
	case FB_EVENT_RESUME:
		if (tc358778xbg.init_flag == 0) {
			tc358778xbg_init();
			tc358778xbg.init_flag = 1;
		}
		break;
	}
	return 0;
}

static struct notifier_block nb = {
	.notifier_call = tc358778xbg_fb_event,
};

static int mxsfb_get_of_property(void)
{
	struct device_node *np = tc358778xbg.client->dev.of_node;
	const char *mode_str;
	int bits_per_pixel, ret;

	ret = of_property_read_string(np, "mode_str", &mode_str);
	if (ret < 0) {
		dev_warn(&tc358778xbg.client->dev, "get of property mode_str fail\n");
		return ret;
	}
	ret = of_property_read_u32(np, "bits-per-pixel", &bits_per_pixel);
	if (ret) {
		dev_warn(&tc358778xbg.client->dev, "get of property bpp fail\n");
		return ret;
	}

	tc358778xbg.mode_str = mode_str;
	tc358778xbg.bits_per_pixel = bits_per_pixel;
#ifdef DEBUG
	printk("[sam] %s %d(%s,%d)\n", __func__, __LINE__,
		tc358778xbg.mode_str, tc358778xbg.bits_per_pixel);
#endif
	return ret;
}

#ifdef CONFIG_VIA_PROC
unsigned int via_proc_value;
u16 via_i2c_index;
u16 via_i2c_value;
static ctl_table via_proc_table[];

void via_proc_dump_reg(void)
{
	int index;
	u16 data;
	
	tc358778xbg_dump_regs(0x0, 256, 2);
	tc358778xbg_dump_regs(0x100, 0x60, 2);
	tc358778xbg_dump_regs(0x200, 0x40, 2);
	tc358778xbg_dump_regs(0x400, 0x60, 2);
	tc358778xbg_dump_regs(0x500, 0x20, 2);
	tc358778xbg_dump_regs(0x600, 0x30, 2);

	index = 0x4;
	tc358778xbg_read_word(index, &data);
	printk("0x%x = 0x%x\n", index, data);
	printk("DE Polarity Active %s\n", (data & 0x10) ? "low" : "high");
	printk("Vsync Polarity Active %s\n", (data & 0x20) ? "high" : "low");
	printk("Parallel Port enable %d\n", (data & 0x40) ? 1 : 0);
	printk("Parallel Data Fmt(888/666/565) %d\n", (data & 0x300) >> 16);

	index = 0x18;
	tc358778xbg_read_word(index, &data);
	printk("0x%x = 0x%x\n", index, data);
	printk("PLL enable %d\n", (data & 0x1) ? 1 : 0);
	printk("PLL reset %s\n", (data & 0x2) ? "off" : "on");
	printk("PLL clk enable %d\n", (data & 0x10) ? 1 : 0);
	printk("Bypass clk enable %d\n", (data & 0x20) ? 1 : 0);

	index = 0x32;
	tc358778xbg_read_word(index, &data);
	printk("0x%x = 0x%x\n", index, data);
	printk("DSI Hsync Polarity %s\n", (data & 0x1) ? "high" : "low");
	printk("Reset pointers %d\n", (data & 0x4000) ? 1 : 0);
	printk("Frame stop %d\n", (data & 0x8000) ? 1 : 0);

	index = 0xF8;
	tc358778xbg_read_word(index, &data);
	printk("0x%x = 0x%x\n", index, data);
	printk("VB over flow %d\n", (data & 0x1) ? 1 : 0);
	printk("VB under flow %d\n", (data & 0x2) ? 1 : 0);

	index = 0x40C;
	tc358778xbg_read_word(index, &data);
	printk("0x%x = 0x%x\n", index, data);
	printk("TXMode %s\n", (data & 0x80) ? "HS" : "LP");

	index = 0x410;
	tc358778xbg_read_word(index, &data);
	printk("0x%x = 0x%x\n", index, data);
	printk("Halted %d\n", (data & 0x1) ? 1 : 0);
	printk("FIFO Empty %d\n", (data & 0x20) ? 1 : 0);
	printk("FIFO almost empty %d\n", (data & 0x40) ? 1 : 0);
	printk("FIFO almost full %d\n", (data & 0x80) ? 1 : 0);
	printk("Rx active %d\n", (data & 0x100) ? 1 : 0);
	printk("Tx active %d\n", (data & 0x200) ? 1 : 0);

	index = 0x518;
	tc358778xbg_read_word(index, &data);
	printk("0x%x = 0x%x\n", index, data);
	printk("DSI start %d\n", (data & 0x1) ? 1 : 0);
	
}

static int via_do_proc(ctl_table *ctl, int write,
				void *buffer, size_t *len, loff_t *ppos)
{
	int ret;
	int ctl_name;

	ctl_name = (((int)ctl - (int)via_proc_table) / sizeof(ctl_table)) + 1;
	if (!write) {
		switch (ctl_name) {
		case 1:
			via_proc_dump_reg();
			break;
		case 2:
			via_proc_value = via_i2c_index;
			break;
		case 3:
			tc358778xbg_read_word(via_i2c_index, &via_i2c_value);
			via_proc_value = via_i2c_value;
			printk("read 0x%x = 0x%x\n", via_i2c_index, via_i2c_value);
			break;
		case 4:
			txdt550uzpa_init();
			break;
		case 5:
			// tc358778xbg_init();
			break;
		case 6:
			txdt550uzpa_init2();
			break;
		default:
			break;
		}
	}

	ret = proc_dointvec(ctl, write, buffer, len, ppos);
	if (write) {
		switch (ctl_name) {
		case 1:
			break;
		case 2:
			via_i2c_index = via_proc_value;
			break;
		case 3:
			via_i2c_value = via_proc_value;
			tc358778xbg_write_word(via_i2c_index, via_i2c_value);
			printk("write 0x%x = 0x%x\n", via_i2c_index, via_i2c_value);
			break;
		case 4: // tx_cmd
			switch(via_proc_value) {
			case 0:
				{
				int i;
				for (i = 0; i < 100; i++) {
					if (txdt550uzpa_cmd_table[i] == 0)
						break;
				txdt550uzpa_write_cmd(txdt550uzpa_cmd_table[i]);
				}
				}
				break;
			default:
				if (via_proc_value <= 18)
					txdt550uzpa_write_cmd(txdt550uzpa_cmd_table[via_proc_value - 1]);
				break;
			}
			break;
		case 5: // disp_start
			tc358778xbg_init();
			break;
		case 6: /* tx_dcs_cmd */
			switch(via_proc_value) {
			case 0:
				{
				int i;
				for (i = 0; i < 100; i++) {
					if (txdt550uzpa_cmd_table[i] == 0)
						break;
				txdt550uzpa_write_dcs_cmd(txdt550uzpa_cmd_table[i]);
				}
				}
				break;
			default:
				if (via_proc_value <= 18)
					txdt550uzpa_write_dcs_cmd(txdt550uzpa_cmd_table[via_proc_value - 1]);
				break;
			}
			break;
		default:
			break;
		}
	}
	return ret;
}

static ctl_table via_proc_table[] = {
	{ /* .ctl_name = 1, */
	.procname	= "dump_reg",
	.data		= &via_proc_value,
	.maxlen		= sizeof(int),
	.mode		= 0666,
	.proc_handler = &via_do_proc,
	},
	{ /* .ctl_name = 2, */
	.procname	= "i2c_index",
	.data		= &via_proc_value,
	.maxlen		= sizeof(int),
	.mode		= 0666,
	.proc_handler = &via_do_proc,
	},
	{ /* .ctl_name = 3, */
	.procname	= "i2c_data",
	.data		= &via_proc_value,
	.maxlen 	= sizeof(int),
	.mode		= 0666,
	.proc_handler = &via_do_proc,
	},
	{ /* .ctl_name = 4, */
	.procname	= "tx_cmd",
	.data		= &via_proc_value,
	.maxlen 	= sizeof(int),
	.mode		= 0666,
	.proc_handler = &via_do_proc,
	},
	{ /* .ctl_name = 5, */
	.procname	= "disp_start",
	.data		= &via_proc_value,
	.maxlen 	= sizeof(int),
	.mode		= 0666,
	.proc_handler = &via_do_proc,
	},
	{ /* .ctl_name = 6, */
	.procname	= "tx_dcs_cmd",
	.data		= &via_proc_value,
	.maxlen 	= sizeof(int),
	.mode		= 0666,
	.proc_handler = &via_do_proc,
	},
	{ /* end of table */
	
	}
};

static ctl_table via_root_table[] = {
	{
	.procname	= "via", /* create path ==> /proc/sys/via */
	.mode		= 0555,
	.child		= via_proc_table
	},
	{ /* end of table */
	}
};
static struct ctl_table_header *via_table_header;
#endif

#define IMX_GPIO_NR(bank, nr)		(((bank) - 1) * 32 + (nr))
#define MX6Q_DISPRESET_GPIO6_11		IMX_GPIO_NR(6, 11)
#define MX6Q_DISPPWREN_GPIO6_14		IMX_GPIO_NR(6, 14)
#define MX6Q_DISPPWREN_GPIO6_16		IMX_GPIO_NR(6, 16)

static int tc358778xbg_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	memset(&tc358778xbg, 0, sizeof(tc358778xbg));

	tc358778xbg.client = client;

	dev_dbg(&tc358778xbg.client->dev, "%s\n", __func__);;

	/* display VCC power enable */
	gpio_request(MX6Q_DISPPWREN_GPIO6_16, "gpio6_16");
	gpio_direction_output(MX6Q_DISPPWREN_GPIO6_16, 1);

	/* display power enable */
	gpio_request(MX6Q_DISPPWREN_GPIO6_14, "gpio6_14");
	gpio_direction_output(MX6Q_DISPPWREN_GPIO6_14, 1);

	/* display reset pin */
	gpio_request(MX6Q_DISPRESET_GPIO6_11, "gpio6_11");
	gpio_direction_output(MX6Q_DISPRESET_GPIO6_11, 0);
	mdelay(1);
	gpio_direction_output(MX6Q_DISPRESET_GPIO6_11, 1);
	
	if (tc358778xbg_check_ID() < 0) {
		dev_err(&tc358778xbg.client->dev,
			"tc358778xbg: cound not find device\n");
		return -ENODEV;
	}

	mxsfb_get_of_property();
	fb_register_client(&nb);
#ifdef CONFIG_VIA_PROC
	via_table_header = register_sysctl_table(via_root_table);
#endif
	return 0;
}

static int tc358778xbg_remove(struct i2c_client *client)
{
	fb_unregister_client(&nb);
	tc358778xbg_suspend();
	return 0;
}

static void tc358778xbg_dispon(void)
{
	u16 data;

#ifdef DEBUG
	printk("[sam] %s %d\n", __func__, __LINE__);
#endif
	/* set display on */
	data = 0x0029;
	tc358778xbg_write_cmd(0x5, (u8 *) &data, 0);
	mdelay(100); /* Wait more than 1 frame */
}

static void tc358778xbg_dispoff(void)
{
	u16 data;

#ifdef DEBUG
	printk("[sam] %s %d\n", __func__, __LINE__);
#endif
	/* set display off */
	data = 0x0028;
	tc358778xbg_write_cmd(0x5, (u8 *) &data, 0);
	mdelay(100); /* Wait more than 1 frame */
}

static void tc358778xbg_init(void)
{
//	printk("[sam] %s %d\n", __func__, __LINE__);

	/* power on */
	/* RESX='L' */
	/* RESX='H' */
	/* Start REFCLK & PCLK */

	/* sw reset */
	tc358778xbg_write_word(REG_SYSCTL,0x01);
	udelay(1000);
	tc358778xbg_write_word(REG_SYSCTL,0x00);

	/* PLL, Clock setting */
	tc358778xbg_write_word(REG_PLLCTL0,0x1095);
	tc358778xbg_write_word(REG_PLLCTL1,0x0203);
	udelay(1000);
	tc358778xbg_write_word(REG_PLLCTL1,0x0213);

	/* DPI input */
	tc358778xbg_write_word(REG_VSDLY,0x00A0);

	/* DPHY */
	tc358778xbg_write_word(0x140,0x00); /* REG_CLW_CNTRL */
	tc358778xbg_write_word(0x142,0x00);
	tc358778xbg_write_word(0x144,0x00); /* REG_D0W_CNTRL */
	tc358778xbg_write_word(0x146,0x00);
	tc358778xbg_write_word(0x148,0x00); /* REG_D1W_CNTRL */
	tc358778xbg_write_word(0x14A,0x00);
	tc358778xbg_write_word(0x14C,0x00); /* REG_D2W_CNTRL */
	tc358778xbg_write_word(0x14E,0x00);
	tc358778xbg_write_word(0x150,0x00); /* REG_D3W_CNTRL */
	tc358778xbg_write_word(0x152,0x00);

	tc358778xbg_write_word(0x100,0x02); /* REG_CLW_DPHY */
	tc358778xbg_write_word(0x102,0x00);
	tc358778xbg_write_word(0x104,0x02); /* REG_D0W_DPHY */
	tc358778xbg_write_word(0x106,0x00);
	tc358778xbg_write_word(0x108,0x02); /* REG_D1W_DPHY */
	tc358778xbg_write_word(0x10A,0x00);
	tc358778xbg_write_word(0x10C,0x02); /* REG_D2W_DPHY */
	tc358778xbg_write_word(0x10E,0x00);
	tc358778xbg_write_word(0x110,0x02); /* REG_D3W_DPHY */
	tc358778xbg_write_word(0x112,0x00);

	/* DSI-TX PPI */
	tc358778xbg_write_word(0x210,0x1770); /* REG_LINEINITCNT */
	tc358778xbg_write_word(0x212,0x0000);
	tc358778xbg_write_word(0x214,0x0005); /* REG_LPTXTIMECNT */
	tc358778xbg_write_word(0x216,0x0000);
	tc358778xbg_write_word(0x218,0x2205); /* REG_TCLK_HEADERCNT */
	tc358778xbg_write_word(0x21A,0x0000);
	tc358778xbg_write_word(0x21C,0x0000); /* REG_TCLK_TRAILCNT */
	tc358778xbg_write_word(0x21E,0x0000);
	tc358778xbg_write_word(0x220,0x0705); /* REG_THS_HEADERCNT */
	tc358778xbg_write_word(0x222,0x0000);
	tc358778xbg_write_word(0x224,0x4E20); /* REG_TWAKEUP */
	tc358778xbg_write_word(0x226,0x0000);
	tc358778xbg_write_word(0x228,0x0000); /* REG_TCLK_POSTCNT */
	tc358778xbg_write_word(0x22A,0x0000);
	tc358778xbg_write_word(0x22C,0x0005); /* REG_THS_TRAILCNT */
	tc358778xbg_write_word(0x22E,0x0000);
	tc358778xbg_write_word(0x230,0x0005); /* REG_HSTXVREGCNT */
	tc358778xbg_write_word(0x232,0x0000);
	tc358778xbg_write_word(0x234,0x001F); /* REG_HSTXVREGEN */
	tc358778xbg_write_word(0x236,0x0000);
	tc358778xbg_write_word(0x238,0x0001); /* REG_TXOPTIONCNTRL */
	tc358778xbg_write_word(0x23A,0x0000);
	tc358778xbg_write_word(0x23C,0x0004); /* REG_BTACNTRL1 */
	tc358778xbg_write_word(0x23E,0x0005);
	tc358778xbg_write_word(0x204,0x0001); /* REG_STARTCNTRL */
	tc358778xbg_write_word(0x206,0x0000);

	/* DSI-TX Timing */
 	tc358778xbg_write_word(REG_DSI_EVENT,0x0000); /* Sync Pulse/Event mode */
	tc358778xbg_write_word(REG_DSI_VSW,0x0008);
	tc358778xbg_write_word(REG_DSI_VBPR,0x0010);
	tc358778xbg_write_word(REG_DSI_VACT,0x0780);
	tc358778xbg_write_word(REG_DSI_HSW,0x001C);
	tc358778xbg_write_word(REG_DSI_HBPR,0x006E);
	tc358778xbg_write_word(REG_DSI_HACT,0x0CA8);

	tc358778xbg_write_word(0x518,0x0001); /* REG_DSI_START, DSI start */
	tc358778xbg_write_word(0x51A,0x0000);

	/* Peripheral Setting */
	txdt550uzpa_init();

	/* set to HS mode */					
	tc358778xbg_write_word(0x500,0x0086); /* REG_DSI_CONFW, DSI mode = HS */
	tc358778xbg_write_word(0x502,0xA300);
	tc358778xbg_write_word(0x500,0x8000); /* switch to DSI mode */
	tc358778xbg_write_word(0x502,0xC300);
	
	/* DPI input start */
	tc358778xbg_write_word(0x008,0x0037);
	tc358778xbg_write_word(0x050,0x003E);
	tc358778xbg_write_word(0x032,0x0000);
	tc358778xbg_write_word(0x004,0x0044);

	/* Peripheral Setting */
	txdt550uzpa_init2();
}

static void tc358778xbg_suspend(void)
{
	u16 data;
#ifdef DEBUG
	printk("[sam] %s %d\n", __func__, __LINE__);
#endif
	/* set display off */
	data = 0x0028;
	tc358778xbg_write_cmd(0x5, (u8 *) &data, 0);
	mdelay(40); /* Wait more than 1 frame */
	

	/* enter sleep mode */
	data = 0x0010;
	tc358778xbg_write_cmd(0x5, (u8 *) &data, 0);
	mdelay(80); /* Wait more than 80ms */

	/* RGB Port Disable */
	tc358778xbg_write_word(REG_PPMISC, 0x8000); /* Set Frame Stop to 1 */
	mdelay(40); /* wait more than 1 frame */
	tc358778xbg_write_word(REG_CONFCTL, 0x0004); /* Parallel input stop */
	tc358778xbg_write_word(REG_PPMISC, 0xC000); /* RstPtr to 1 */

	/* Stop DSI conti clock */
	tc358778xbg_write_word(0x238,0x0); /* REG_TXOPTIONCNTRL */
	tc358778xbg_write_word(0x23A,0x0);
	
	/* Disable DPHY */
	tc358778xbg_write_word(0x140,0x01);
	tc358778xbg_write_word(0x142,0x00);
	tc358778xbg_write_word(0x144,0x01);
	tc358778xbg_write_word(0x146,0x00);
	tc358778xbg_write_word(0x148,0x01);
	tc358778xbg_write_word(0x14A,0x00);
	tc358778xbg_write_word(0x14C,0x01);
	tc358778xbg_write_word(0x14E,0x00);
	tc358778xbg_write_word(0x150,0x01);
	tc358778xbg_write_word(0x152,0x00);

	/* Stop RGB input including PCLK */
	/* Stop REFCLK */
}

static const struct i2c_device_id tc358778xbg_id[] = {
	{ DRV_NAME, 0},
	{ },
};
MODULE_DEVICE_TABLE(i2c, tc358778xbg_id);

static const struct of_device_id tc358778xbg_dt_ids[] = {
	{ .compatible = "TOSHIBA,TC358778XBG", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, tc358778xbg_dt_ids);

static struct i2c_driver tc358778xbg_driver = {
	.driver = {
		.name = DRV_NAME,
		.owner = THIS_MODULE,
		.of_match_table = tc358778xbg_dt_ids,
	},
	.probe		= tc358778xbg_probe,
	.remove		= tc358778xbg_remove,
	.id_table	= tc358778xbg_id,
};

module_i2c_driver(tc358778xbg_driver);

MODULE_AUTHOR("Freescale Semiconductor, Inc.");
MODULE_DESCRIPTION("TC358778XBG LCD/MIPI DSI driver");
MODULE_LICENSE("GPL");
