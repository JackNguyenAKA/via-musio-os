#!/bin/sh
# Copyright (c) VIA Technologies, Inc. All rights reserved.
# A simple script to Add / remove partitions for USB and SD/MMC
# Written by Vincent Chen

partition=$1
device=$1
action=$2
mntdir=/mnt
# logfile=/tmp/udevmountd.log
logfile=/dev/null

# %k = mmcblk[0-9]{p[1-9]}, mspblk[0-9]{p[1-9]}, xdblk[0-9]{p[1-9]},
#      sd[a-z]{[1-9]}

if [ $# -lt 2 ]; then
	echo "Usage: $0 %k [add|remove]" >> $logfile
	exit 0
fi

if [ "$action" == "add" ]; then
	if [ ! -e "/dev/$partition" ]; then
		echo "skip $device" >> $logfile
		exit 0
	fi
fi

if [ "$action" == "remove" ]; then
	mtab=`cat /etc/mtab|grep $partition`
	if [ ! -z "$mtab" ]; then
		echo "umount $mntdir/$partition" >> $logfile
		/bin/umount -lf $mntdir/$partition 2>> $logfile
		/bin/rmdir $mntdir/$partition
	fi
	exit 0
fi

if [ ! -e /etc/mtab ]; then
	ln -s /proc/mounts /etc/mtab
fi

if [ ! -z "`echo $device|grep mmcblk`" ]; then
	device=`echo $device|sed 's/p[0-9]//g'`
elif [ ! -z "`echo $device|grep mspblk`" ]; then
	device=`echo $device|sed 's/p[0-9]//g'`
elif [ ! -z "`echo $device|grep xdblk`" ]; then
	device=`echo $device|sed 's/p[0-9]//g'`
elif [ ! -z "`echo $device|grep sd`" ]; then
	device=`echo $device|sed 's/[0-9]//g'`
fi

# echo "device = $device, partitiontion = $partition" >> $logfile

if [ $device == $partition ]; then
	table=`/sbin/fdisk -l /dev/$device 2>/dev/null`
	if [ -z "$table" ]; then
		echo "skip $device" >> $logfile
		exit 0
	fi
	nparts=`/sbin/fdisk -l /dev/$device|grep ^/dev|wc -l`
	if [ "$nparts" != "0" ]; then
		echo "skip $device" >> $logfile
		exit 0
	fi
fi

if [ "$action" == "add" ]; then
	mtab=`cat /etc/mtab|grep $device`
	if [ ! -z "$mtab" ]; then
		echo "umount $mntdir/$partition" >> $logfile
		/bin/umount -lf $mntdir/$device 2>> $logfile
	fi
	echo "mount $mntdir/$partition" >> $logfile
	/bin/mkdir -p $mntdir/$partition
	/bin/mount /dev/$partition $mntdir/$partition 2>> $logfile
fi

