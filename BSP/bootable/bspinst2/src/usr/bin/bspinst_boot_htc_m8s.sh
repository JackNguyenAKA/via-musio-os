#!/bin/sh
# Copyright (c) 2016  VIA Technologies, Inc. All rights reserved.

bindir=$PWD/bin:$PWD/bspinst/bin:/mnt/mmcblk0p1/bspinst/bin:/mnt/sda1/bspinst/bin:/usr/bin

export PATH=$bindir:$PATH

source `which bspinst_var.sh`
source `which bspinst_ui.sh`
source `which bspinst_api.sh`

sysctl -w kernel.printk=0

srcdir=`get_src_dir`

echo srcdir = $srcdir

part_id()
{
  parted -s /dev/mmcblk0 print 2>/dev/null | \
  sed 's/ext4/    /g' | sed 's/fat16/     /g' | \
  awk '{ print $1, $5 }'| grep \ $1$ | awk '{ print $1; }'
}

cat /dev/zero > /dev/fb0
fbset -g 1080 1920 1080 1920 32
fbset > /etc/fb.modes

bspinst_start

# create device node
mdev -s

umount /dev/mmcblk0*

ratio 1; msg "."; sleep 1
ratio 2; msg "."; sleep 1
ratio 3; msg "."; sleep 1

mmcblk_vboot=mmcblk0p$(part_id vboot)
mmcblk_vboot1=mmcblk0p$(part_id vboot1)
mmcblk_vboot2=mmcblk0p$(part_id vboot2)

if [ -e /dev/$mmcblk_vboot ];then
  ratio 11; msg "vboot.bin -> /dev/$mmcblk_vboot"
  cat $srcdir/vboot-*.bin > /dev/$mmcblk_vboot;
  sleep 1
fi

if [ -e /dev/$mmcblk_vboot1 ];then
  ratio 12; msg "vboot1.bin -> /dev/$mmcblk_vboot1"
  cat $srcdir/vboot1-*.bin > /dev/$mmcblk_vboot1;
  sleep 1
fi

if [ -e /dev/$mmcblk_vboot2 ];then
  ratio 13; msg "vboot2.bin -> /dev/$mmcblk_vboot2"
  cat $srcdir/vboot2-*.bin > /dev/$mmcblk_vboot2;
  sleep 1
fi

erase ()
{
  name=$1
  blkdev=mmcblk0p$(part_id $name)
  if [ -e /dev/$blkdev ];then
    msg "erase $blkdev"
    cat /dev/zero > /dev/$blkdev
    sleep 1
  fi
}

mkfs_ext4 ()
{
  label=$1
  name=$2
  blkdev=mmcblk0p$(part_id $name)
  if [ -e /dev/$blkdev ];then
    msg "format $label ($blkdev)"
    mkfs.ext4 -v -L $label /dev/$blkdev
    sleep 1
  fi
}

inst_fw ()
{
  name=$1
  blkdev=mmcblk0p$(part_id $name)
  if [ -n "$2" ] && [ ! -e $srcdir/${name}.img ]; then
    name=$2
  fi
  if [ -e /dev/$blkdev ] && [ -e $srcdir/${name}.img ]; then
    msg "installing ${name}.img ($blkdev)"
    dd if=$srcdir/${name}.img of=/dev/$blkdev
    sleep 1
  fi
}

inst_simg ()
{
  name=$1
  blkdev=mmcblk0p$(part_id $name)
  if [ -n "$2" ]; then
    name=$2
  fi
  if [ -e /dev/$blkdev ] && [ -e $srcdir/${name}.img ]; then
    msg "installing ${name}.img ($blkdev)"
    simg2img $srcdir/${name}.img /dev/$blkdev
    sleep 1
  fi
}

ratio 14; mkfs_ext4 cache cache
ratio 15; mkfs_ext4 userdata thalesboot
ratio 16; mkfs_ext4 userdata thalesapps
erase 17; erase emmcconfig
erase 18; erase emmcconfig2

ratio 20; inst_fw boot
ratio 25; inst_fw boot2 boot
ratio 26; inst_fw rfg_0
ratio 27; inst_fw rfg_1
ratio 28; inst_fw rfg_2
ratio 29; inst_fw rfg_3
ratio 30; inst_fw rfg_4
ratio 31; inst_fw rfg_5
ratio 32; inst_fw rfg_6
ratio 33; inst_fw rfg_7
ratio 34; inst_fw fsg
ratio 35; inst_fw radio
ratio 36; inst_fw modem_st1
ratio 37; inst_fw modem_st2
ratio 38; inst_fw cdma_record
ratio 39; inst_fw fsc

if [ -e $srcdir/boot.img ]; then
  ratio 40; inst_simg system
  ratio 70; inst_simg system2
  sleep 1
else
  ratio 25; msg "!! cannot find system.img !!"
  sleep 5
fi

sync; sleep 1;               

ratio 100;

bspinst_stop
