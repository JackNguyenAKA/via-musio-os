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

# The rendering API for GUI should be found here.

#export PATH=.:$src_dir/bin:$PATH

#source $src_dir/bin/bspinst_var.sh
#source $src_dir/bin/bspinst_api.sh

# set_color - set background color
# @red
# @green
# @blue
set_color ()
{
	g_bcr=$1
	g_bcg=$2
	g_bcb=$3
}

# set_font_size - set font size
# @size
set_font_size ()
{
	if [ $g_ui -eq 1 ]; then
		g_fs=$1
		quiet rpc picture set_font_size $g_fs
	fi
}

# set_font_color - set font color
# @red
# @green
# @blue
set_font_color ()
{
	g_fcr=$1
	g_fcg=$2
	g_fcb=$3
}

# set_border - set border size
# @top
# @bottom
# @left
# @right
set_border ()
{
	g_bdr_top=$1
	g_bdr_bottom=$2
	g_bdr_left=$3
	g_bdr_right=$4
}

# fsmul - font size multiplier
# @n
fsmul ()
{
	expr $g_fs \* $1
}

# fwmul - font width multiplier
# @n
fwmul ()
{
	expr $g_fs \* $1 \* 2 / 3
}

bsmul ()
{
	expr $g_bsln \* $1
}

# drawrect
# @x
# @y
# @w
# @h
# @lw
drawrect ()
{
	if [ $g_ui -eq 1 ]; then
		quiet rpc picture create_widget
		quiet rpc picture set_geometry $1 $2 $3 $4
		quiet rpc picture set_font_color $g_fcr $g_fcg $g_fcb
		quiet rpc picture set_boarder $5 $5 $5 $5
		quiet rpc picture set_ttl 2
		quiet rpc picture add_widget
	fi
}

# fillrect
# @x
# @y
# @w
# @h
fillrect ()
{
	if [ $g_ui -eq 1 ]; then
		quiet rpc picture create_widget
		quiet rpc picture set_geometry $1 $2 $3 $4
		quiet rpc picture set_fg_color $g_bcr $g_bcg $g_bcb
		quiet rpc picture set_ttl 2
		quiet rpc picture add_widget
	fi
}

# drawstr
# @x
# @y
# @string
drawstr ()
{
	local str="`echo $@ | cut -d\  -f3-`"
	local h=`expr $g_fs + 0`

	if [ -z "$str" ]; then
		return
	fi

	if [ $g_ui -eq 1 ]; then
		quiet rpc picture create_widget
		quiet rpc picture set_geometry $1 $2 0 $h
		quiet rpc picture set_fg_color $g_bcr $g_bcg $g_bcb
		quiet rpc picture set_font_color $g_fcr $g_fcg $g_fcb
		quiet rpc picture set_boarder $g_bdr_top $g_bdr_bottom $g_bdr_left $g_bdr_right
		quiet rpc picture set_string "$str"
		quiet rpc picture set_ttl 2
		quiet rpc picture set_alignment 1
		quiet rpc picture add_widget
	fi
}

# drawmsg - draw one line for one message only
# @x
# @y
# @string
drawmsg ()
{
	if [ $g_ui -eq 1 ]; then
		local w=`expr $g_xres - $1`
		local h=`expr $g_fs \* 2`
		quiet rpc picture create_widget
		quiet rpc picture set_geometry $1 $2 $w $h
		quiet rpc picture set_fg_color $g_bcr $g_bcg $g_bcb
		quiet rpc picture set_font_color $g_fcr $g_fcg $g_fcb
		quiet rpc picture set_string `echo $@ | cut -d\  -f3-`
		quiet rpc picture set_ttl 2
		quiet rpc picture set_alignment 1
		quiet rpc picture add_widget
	fi
}

# English warning messages for ticker
show_warnings_by_ticker ()
{
	tmsg=/tmp/msg.txt

	echo -n "Warnings! " > $tmsg
	echo -n "Please don't power off! " >> $tmsg
	# echo -n "The system will auto reboot for you. " >> $tmsg
	echo -n "Please wait..." >> $tmsg
	
	rpc ticker set_bg_color $g_bcr $g_bcg $g_bcb
	rpc ticker set_fg_color $g_fcr $g_fcg $g_fcb
	rpc ticker set_geometry 0 `expr $g_yres - 80` $g_xres 80
	rpc ticker set_fontsize $g_fs
	rpc ticker load_file $tmsg
	rpc ticker load_font $g_font1
	rpc ticker start
}

# Set progress bar
show_progress_bar ()
{
	local x;
	local y;
	local w;
	local h;
	local len=`expr $3 \* $5 / 100`

	set_color 255 255 0
	set_font_color 255 255 255

	if [ "$len" != "0" ]; then
		fillrect $1 $2 $len $4 
	fi
	drawrect $1 $2 $3 $4 4
	x=`expr $1 + $3 + $g_fs`
	y=`expr $2`
	w=`fwmul 6`
	h=`fsmul 1`

	set_color 0 0 255
	drawstr $x $y " $5 %"
}

test_progress_bar ()
{
	for t in 0 1 2 3 4 5 6 7 8 9 10; do
		show_progress_bar `fsmul 4` `bsmul 13` `fsmul 15` `fsmul 1` `expr $t \* 10`
		sleep 1
	done
}

# startup_delay
#
# Wait a few seconds before installation
startup_delay ()
{
	t=$1
	while [ $t -gt 0 ]; do
		msg "Installation will begin in $t seconds"
		sleep 1
		let "t = t - 1"
	done
}

# stop_delay
#
# Wait a few seconds before reboot
stop_delay ()
{
	t=$1
	while [ $t -gt 0 ]; do
		msg "System will reboot in $t seconds"
		sleep 1
		let "t = t - 1"
	done
}

msg ()
{
	if [ $g_ui -eq 1 ]; then
		drawmsg `fwmul 6` `bsmul 14` $@
	fi
	echo $@
}

ratio ()
{
	g_ratio=$1
	if [ $g_ui -eq 1 ]; then
		show_progress_bar `fwmul 6` `bsmul 13` `fwmul 26` `fsmul 1` $1
	fi
}

