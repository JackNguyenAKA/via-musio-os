#!/bin/sh
#
# OTA installer -- install ota.zip to system folder
#
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

if [ $# -lt 2 ]; then
	echo "Usage: instota.sh ota.zip systemdir [target_type]"
	exit 0
fi

OTAPKG=$1
SYSTEM_DIR=$(echo $2|sed 's/\/$//g')
TARGET_TYPE=$3
UPDATE_SCRIPT=$SYSTEM_DIR/META-INF/com/google/android/updater-$TARGET_TYPE-script
TMPFILE=/tmp/runota.log

line_by_line ()
{
	while read line; do
		txt="$txt $line"
		if [ -n "$(echo "$line"|grep \;)" ]; then
			echo "$txt"|tr '();' ' '|tr -d '",'|sed 's/^ //g'|grep -v \!
			txt=
		fi
	done < $1
}

#format
#getprop
#mount
#package_extract_dir
#set_metadata
#set_metadata_recursive
#show_progress
#symlink
#unmount
#write_raw_image
#set_perm
#set_perm_recursive

symlink ()
{
	for arg in $@; do
		if [ "$arg" != "$1" ]; then
			link=${SYSTEM_DIR}$arg
			rm -f $link
			linkdir=$(dirname $link)
			if [ ! -e $linkdir ]; then
				mkdir -p $linkdir
			fi
			ln -s $1 $link
		fi
	done
};

# usage: cap_to_name index
cap_to_name ()
{
	caps="\
		cap_chown            \
		cap_dac_override     \
		cap_dac_read_search  \
		cap_fowner           \
		cap_fsetid           \
		cap_kill             \
		cap_setgid           \
		cap_setuid           \
		cap_setpcap          \
		cap_linux_immutable  \
		cap_net_bind_service \
		cap_net_broadcast    \
		cap_net_admin        \
		cap_net_raw          \
		cap_ipc_lock         \
		cap_ipc_owner        \
		cap_sys_module       \
		cap_sys_rawio        \
		cap_sys_chroot       \
		cap_sys_ptrace       \
		cap_sys_pacct        \
		cap_sys_admin        \
		cap_sys_boot         \
		cap_sys_nice         \
		cap_sys_resource     \
		cap_sys_time         \
		cap_sys_tty_config   \
		cap_mknod            \
		cap_lease            \
		cap_audit_write      \
		cap_audit_control    \
		cap_setfcap          \
		cap_mac_override     \
		cap_mac_admin        \
		cap_syslog           \
		cap_wake_alarm       \
		cap_block_suspend    \
		"
	i=0
	str=
	for name in $caps; do
		if [ $i -eq $1 ]; then
			echo $name
			break;
		fi
		i=$((i+1))
	done
}

# usage: set_raw_cap hex filename
set_raw_cap ()
{
	text=
	rawcap=$(printf "%d" $1)

	if [ $rawcap -eq 0 ]; then
		return
	fi

	i=0
	while [ "$rawcap" -gt 0 ]; do
		bit=$(expr $rawcap % 2)
		if [ "$bit" -eq 1 ]; then
			if [ -n "$text" ]; then
				text="$text,$(cap_to_name $i)"
			else
				text="$(cap_to_name $i)"
			fi
		fi
		i=$((i+1))
		rawcap=$((rawcap/2))
	done

	if [ -n "$text" ]; then
		text="$text=p"

		if [ -e "$2" ]; then
			setcap $text $2
			getcap $2
		else
			echo setcap $text
		fi
	fi
}

# ex. set_metadata_recursive("/system/bin", \
#                            "uid", 0, "gid", 2000, \
#                            "dmode", 0755, "fmode", 0755, \
#                            "capabilities", 0x0, \
#                            "selabel", "u:object_r:system_file:s0");
set_metadata_recursive ()
{
	uid=0
	gid=0
	dmode=755
	fmode=755
	selabel="u:object_r:system_file:s0"
	capabilities="0x0"
	i=1
	prev_arg=""
	for arg in $@; do
		case $prev_arg in
		"uid")
			uid=$arg
			;;
		"gid")
			gid=$arg
			;;
		"dmode")
			dmode=$arg
			;;
		"fmode")
			fmode=$arg
			;;
		"selabel")
			selabel=$arg
			;;
		"capabilities")
			capabilities=$arg
			;;
		esac
		prev_arg=$arg
	done

	chown -R $uid.$gid ${SYSTEM_DIR}$1
	chmod -R $dmode ${SYSTEM_DIR}$1
	setfilecon "$selabel" ${SYSTEM_DIR}$1
	set_raw_cap "$capabilities" ${SYSTEM_DIR}$1

	for f in $(find ${SYSTEM_DIR}$1/ -type f); do
		chmod $fmode $f
		setfilecon "$selabel" $f
		set_raw_cap "$capabilities" $f
	done

	for d in $(find ${SYSTEM_DIR}$1/ -type d); do
		chmod $dmode $d
		setfilecon "$selabel" $d
		getfilecon $d
		set_raw_cap "$capabilities" $d
	done
};

# ex. set_metadata("/system/bin/app_process", \
#                  "uid", 0, "gid", 2000, "mode", 0755, \
#                  "capabilities", 0x0, \
#                  "selabel", "u:object_r:zygote_exec:s0");
set_metadata ()
{
	uid=0
	gid=0
	mode=755
	selabel="u:object_r:system_file:s0"
	capabilities="0x0"
	i=1
	prev_arg=""
	for arg in $@; do
		case $prev_arg in
		"uid")
			uid=$arg
			;;
		"gid")
			gid=$arg
			;;
		"mode")
			mode=$arg
			;;
		"selabel")
			selabel=$arg
			;;
		"capabilities")
			capabilities=$arg
			;;
		esac
		prev_arg=$arg
	done

	chown $uid.$gid ${SYSTEM_DIR}$1
	chmod $mode ${SYSTEM_DIR}$1
	setfilecon "$selabel" ${SYSTEM_DIR}$1
	getfilecon ${SYSTEM_DIR}$1
	set_raw_cap "$capabilities" ${SYSTEM_DIR}$1
};

package_extract_dir ()
{
	from=$SYSTEM_DIR/$1
	to=${SYSTEM_DIR}$2
	if [ "$from" != "$to" ]; then
		cp -af $from/* $to/
	fi
};

# ex. set_perm 0 3003 02750 /system/bin/netcfg  
set_perm ()
{
	chown $1.$2 ${SYSTEM_DIR}$4
	chmod $3 ${SYSTEM_DIR}$4
}
 
# ex. set_perm_recursive 0 2000 0755 0755 /system/bin  
set_perm_recursive ()
{
	chown -R $1.$2 ${SYSTEM_DIR}$5
	chmod -R $3 ${SYSTEM_DIR}$5

	for f in $(find ${SYSTEM_DIR}$5 -type f); do
		chmod $4 $f
	done
}

rm -f $SYSTEM_DIR/system
ln -s . $SYSTEM_DIR/system
unzip -o $OTAPKG -d $SYSTEM_DIR/

# SELinux
sysctl -w kernel.printk=0
! selinuxenabled && load_policy -i
getenforce

# Fallback to default Android update-script
if [ ! -e "$UPDATE_SCRIPT" ]; then
	UPDATE_SCRIPT=$SYSTEM_DIR/META-INF/com/google/android/updater-script
fi

line_by_line $UPDATE_SCRIPT | \
  grep -e set_metadata -e symlink -e package_extract_dir -e set_perm \
  > $TMPFILE

echo ===========================================================================
cat $TMPFILE
echo ===========================================================================

while read line; do
	$line
done < $TMPFILE

rm -rf ${SYSTEM_DIR}/META-INF \
       ${SYSTEM_DIR}/boot.img \
       ${SYSTEM_DIR}/file_contexts \
       ${SYSTEM_DIR}/recovery \
       ${SYSTEM_DIR}/system

echo "done."
