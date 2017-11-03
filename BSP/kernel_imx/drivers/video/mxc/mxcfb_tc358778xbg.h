/*
 * Copyright (C) 2013 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __INCLUDE_TC358778XBG_H
#define __INCLUDE_TC358778XBG_H

/* Global Group */
#define REG_CHIPID		0x00
#define REG_SYSCTL		0x02
#define REG_CONFCTL		0x04
#define REG_VSDLY		0x06
#define REG_DATAFMT		0x08
#define REG_GPIOEN		0x0E
#define REG_GPIODIR		0x10
#define REG_GPIOIN		0x12
#define REG_GPIOOUT		0x14
#define REG_PLLCTL0		0x16
#define REG_PLLCTL1		0x18
#define REG_CMDBYTE		0x22
#define REG_PPMISC		0x32
#define REG_DSITX_DT		0x50
#define REG_FIFOSTATUS		0xF8

/* TX PHY */
#define REG_CLW_DPHY		0x100
#define REG_D0W_DPHY		0x104
#define REG_D1W_DPHY		0x108
#define REG_D2W_DPHY		0x10C
#define REG_D3W_DPHY		0x110
#define REG_CLW_CNTRL		0x140
#define REG_D0W_CNTRL		0x144
#define REG_D1W_CNTRL		0x148
#define REG_D2W_CNTRL		0x14C
#define REG_D3W_CNTRL		0x150

/* TX PPI */
#define REG_STARTCNTRL		0x204
#define REG_STAUS		0x208
#define REG_LINEINITCNT		0x210
#define REG_LPTXTIMECNT		0x214
#define REG_TCLK_HEADERCNT	0x218
#define REG_TCLK_TRAILCNT	0x21C
#define REG_THS_HEADERCNT	0x220
#define REG_TWAKEUP		0x224
#define REG_TCLK_POSTCNT	0x228
#define REG_THS_TRAILCNT	0x22C
#define REG_HSTXVREGCNT		0x230
#define REG_HSTXVREGEN		0x234
#define REG_TXOPTIONCNTRL	0x238
#define REG_BTACNTRL1		0x23C

/* TX CTRL */
#define REG_DSI_CONTROL		0x40C
#define REG_DSI_STATUS		0x410
#define REG_DSI_INT		0x414
#define REG_DSI_INT_ENA		0x418
#define REG_DSI_DSICMD_RXFIFO	0x430
#define REG_DSI_DSI_ACKERR	0x434
#define REG_DSI_ACKERR_INTENA	0x438
#define REG_DSI_ACKERR_HALT	0x43C
#define REG_DSI_RXERR		0x440
#define REG_DSI_RXERR_INTENA	0x444
#define REG_DSI_RXERR_HALT	0x448
#define REG_DSI_ERR		0x44C
#define REG_DSI_ERR_INTENA	0x450
#define REG_DSI_ERR_HALT	0x454
#define REG_DSI_CONFW		0x500
#define REG_DSI_RESET		0x504
#define REG_DSI_INT_CLR		0x50C
#define REG_DSI_START		0x518

/* DSITX CTRL */
#define REG_DSICMD_TX		0x600
#define REG_DSICMD_TYPE		0x602
#define REG_DSICMD_WC		0x604
#define REG_DSICMD_WD0		0x610
#define REG_DSICMD_WD1		0x612
#define REG_DSICMD_WD2		0x614
#define REG_DSICMD_WD3		0x616
#define REG_DSI_EVENT		0x620
#define REG_DSI_VSW		0x622
#define REG_DSI_VBPR		0x624
#define REG_DSI_VACT		0x626
#define REG_DSI_HSW		0x628
#define REG_DSI_HBPR		0x62A
#define REG_DSI_HACT		0x62C

/* Debug */
#define REG_VBUFCTL		0xE0
#define REG_DBG_WIDTH		0xE2
#define REG_DBG_VBLANK		0xE4
#define REG_DBG_DATA		0xE8

#endif
