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
# quiet ENJOYMENT OR NON-INFRINGEMENT.

# BSP installation low level API

# quiet - do command in silence
# @command
quiet ()
{
	$@ 2>/dev/null >/dev/null
	#$@ >/dev/null

	return $?
}

# regular_name - translate @name to reqular expression.
# @name
regular_name ()
{
	local name=$1
	echo -n $name | sed 's/\//\\\//g' | sed 's/\./\\\./g'
}

# base_name - extract filename from whole path name
# @path_name
# @full_name
base_name ()
{
	local path_name=$1
	local full_name=$2

	local reg_path_name=`regular_name $path_name`
	echo -n $full_name | sed "s/$reg_path_name//g"
}

# fsearch - search a file by pattern match
# @from		the searching directory
# @pattern	the file name pattern
# @result	the matched file name found
fsearch ()
{
	local from=$1
	local pattern=
	local result=

	eval "pattern=\$$2"

	if [ -z "$pattern" ]; then
		return 
	fi

	#printf "> Search for firmware ... "

	for i in `find $from -type f -name "$pattern" -follow | sort -r`
	do
		result=$i
		break
	done
	
	if [ ! -e "$result" ]; then
		#printf "Not found (%s) \n" $pattern
		printf "fsearch: file is not found (%s) \n" $pattern
	else
		#printf "Found \n%s\n" $result
		eval "$2=\"$result\""

		if [ $# == 3 ]; then
			eval "$3=\"$result\""
		fi
	fi
}

get_src_dir()
{
	if [ -e ./boot.img ]; then
		echo "$PWD"
	elif [ -e ./bspinst ]; then
		echo "$PWD/bspinst"
	elif [ -e /mnt/mmcblk0p1/bspinst ]; then
		echo "/mnt/mmcblk0p1/bspinst"
	elif [ -e /mnt/mmcblk1p1/bspinst ]; then
		echo "/mnt/mmcblk1p1/bspinst"
	elif [ -e /mnt/sda1/bspinst ]; then
		echo "/mnt/sda1/bspinst"
	fi
}

hex_to_dec ()
{
	printf "%u" $1
}

dec_to_hex ()
{
	printf "0x%x" $1
}

#
# PLEASE DON'T MODIFY FUNCTIONS ABOVE THIS LINE.
# THOSE FUNCTIONS ARE THE HEART OF BSPINST2.
#

whereis ()
{
	local file=$1
	local srcdir=`get_src_dir`

	quiet fsearch $srcdir file

	echo $file
}

catch_input_event ()
{
	local handler=/usr/bin/input.sh
	local code=/tmp/catch_input_event.dat
	if [ ! -e $handler ]; then
		echo "#!/bin/sh" > $handler
		echo "echo \$1 > $code" >> $handler
		chmod +x $handler
		sync
	fi

	rm -f $code

	while [ ! -e $code ]; do
		sleep 1
	done

	i=`cat /proc/partitions|wc -l`
	if [ $i -ne $g_nr_mnt ]; then
		echo $KEY_VOLUMEDOWN
	else
		cat $code
	fi
}

# get_var - load environment variables 
# @args
get_var ()
{
	for exp in $@; do
		eval $exp
	done
}

rpc_start ()
{
	srcdir=`get_src_dir`
	ini="/tmp/bspinst.ini"

	rpc 2>/dev/null >/dev/null && return 255
	if [ $g_ui -eq 0 ]; then return 255; fi

	mdev -s
	get_var $@
	fbset > /etc/fb.modes
	mount -t proc none /proc
	mount -t tmpfs none /dev/shm

	sync; sleep 1

	echo "[font1]" > $ini
	echo "size = $g_fs" >> $ini
	echo "file = $g_font1" >> $ini
	echo >> $ini
	echo "[signage]" >> $ini
	echo "blank = 1" >> $ini

        args="--dfb:system=fbdev,bg-none,no-hardware,no-vt,disable-module=keyboard,mode=1080x1920,pixelformat=ARGB"

	cat /dev/zero >/dev/fb0 2>/dev/null
	rpcsrv2d -s -i 1 -f /tmp/bspinst.ini $args
	sleep 1

	return 0
}

render_main ()
{
	local srcdir=`get_src_dir`

	if [ $g_ui -eq 0 ] || [ ! -e $srcdir/version ]; then
		return 1
	fi

	set_color 0 0 255
	fillrect 0 0 $g_xres $g_yres

	set_font_color 255 255 255

	drawstr `fwmul 4` `bsmul 1`  `head -1 $srcdir/version`
	drawstr `fwmul 4` `bsmul 2`  `head -2 $srcdir/version |tail -1`
	drawstr `fwmul 4` `bsmul 4`  "----------------------------------"

	drawstr `fwmul 6` `bsmul 6`  `grep U-Boot    $srcdir/version | head -1`
	drawstr `fwmul 6` `bsmul 7`  `grep Kernel    $srcdir/version | head -1`
	drawstr `fwmul 6` `bsmul 9`  `grep Base      $srcdir/version | head -1`
	drawstr `fwmul 6` `bsmul 10` `grep Reference $srcdir/version | head -1`
	drawstr `fwmul 6` `bsmul 11` `grep OtherInfo $srcdir/version | head -1`

	ratio 0
	show_warnings_by_ticker
}

bspinst_start ()
{
	rpc_start
	render_main
	g_nr_mnt=`cat /proc/partitions|wc -l`
}

bspinst_stop ()
{
	local srcdir=`get_src_dir`
	keyinst

	sync
	msg "Please remove installation media!"

	# Wait until storage unplugged or ENTER pressed

	if [ -e /proc/partitions ]; then
		i=`cat /proc/partitions|wc -l`
		j=`cat /proc/partitions|wc -l`
		while [ $j == $i ]; do
			read -t 1 key
			if [ $? -eq 0 ]; then
				break
			fi
			j=`cat /proc/partitions|wc -l`
			#if [ ! -e $srcdir/scriptcmd ]; then
			#	break;
			#fi
		done
	else
		while [ -e $0 ]; do
			read -t 1 key
			if [ $? -eq 0 ]; then
				break
			fi
		done
	fi

	sync
	stop_delay 3
	reboot -f -n
}

