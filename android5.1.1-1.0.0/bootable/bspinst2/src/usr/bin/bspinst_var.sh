# Copyright (c) 2008-2011 VIA Technologies, Inc. All Rights Reserved.
#
# This PROPRIETARY SOFTWARE is the property of VIA Technologies, Inc.
# and may contain trade secrets and/or other confidential information of
# VIA Technologies, Inc. This file shall not be disclosed to any
# third party, in whole or in part, without prior written consent of
# VIA.
#
# THIS PROPRIETARY SOFTWARE AND ANY RELATED DOCUMENTATION ARE PROVIDED
# AS IS, WITH ALL FAULTS, AND WITHOUT WARRANTY OF ANY KIND EITHER EXPRESS
# OR IMPLIED, AND WONDERMEDIA TECHNOLOGIES, INC. DISCLAIMS ALL EXPRESS
# OR IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
# QUIET ENJOYMENT OR NON-INFRINGEMENT.

# Global variables for drawing API.

video_mode=`fbset | grep - | cut -d\  -f2 | sed 's/\"//g' | cut -d- -f1`

# Display resolution
# @xres	X-resolution
# @yres Y-resolution
g_xres=`echo ${video_mode} | cut -dx -f1`
g_yres=`echo ${video_mode} | cut -dx -f2`

# Backgound color
# @bcr red color
# @bcg green color
# @bcb blue color
g_bcr=0
g_bcg=0
g_bcb=255

# Foreground color
# @fcr red color
# @fcg green color
# @fcb blue color
g_fcr=255
g_fcg=255
g_fcb=255

# Border
g_bdr_top=0
g_bdr_bottom=0
g_bdr_left=0
g_bdr_right=0

# Font config
# hint: for 1024x768, best font size is 32.
g_fs=32
fs1=`expr $g_xres \* $g_fs / 1024`
fs2=`expr $g_yres \* $g_fs / 768`
if [ $fs1 -lt $fs2 ]; then
	g_fs=$fs1
else
	g_fs=$fs2
fi
echo "set font $g_fs, ${g_xres}x${g_yres}"

g_bsln=`expr $g_fs \* 4 / 3`

g_font1="/usr/share/fonts/truetype/courbd.ttf"

# DirectFB UI Settings
g_ui=1
g_ratio=0

KEY_0="0xb"
KEY_1="0x2"
KEY_2="0x3"
KEY_3="0x4"
KEY_4="0x5"
KEY_5="0x6"
KEY_6="0x7"
KEY_7="0x8"
KEY_8="0x9"
KEY_9="0xa"
KEY_ENTER="0x1c"
KEY_BACK="0x9e"
KEY_VOLUMEDOWN="0x72"
KEY_VOLUMEUP="0x73"

# Mount
g_nr_mnt=0
