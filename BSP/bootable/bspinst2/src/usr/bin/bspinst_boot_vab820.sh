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

board=$(find $srcdir -maxdepth 1 -name "boot-*.img" \
        -exec basename {} \; | sort | head -1 | cut -c6- | cut -d\. -f1)

if [ -z "$board" ]; then
  board='?'
fi

echo board name is ${board}.

part_id()
{
  parted -s /dev/mmcblk0 print 2>/dev/null | \
  sed 's/ext4/    /g' | sed 's/fat16/     /g' | \
  awk '{ print $1, $5 }'| grep \ $1$ | awk '{ print $1; }'
}

_dots=/tmp/dots

run_dots()
{
  touch $_dots
  i=0
  j=0
  local str_=
  while [ -e $_dots ] && [ $j -lt 120 ]; do
    str_="${str_}."
    msg $str_
    i=$((i+1))
    j=$((j+1))
    sleep 2s
    if [ $i -gt $1 ]; then
      str_=
      i=0
    fi
  done
}

start_dots()
{
  run_dots 30&
}

stop_dots()
{
  rm -f $_dots
}

render_ui ()
{
  if [ $g_ui -eq 0 ] || [ -e $srcdir/version ]; then
    return 1
  fi

  set_color 0 0 255
  fillrect 0 0 $g_xres $g_yres

  set_font_color 255 255 255

  fillrect 0 0 $g_xres $g_yres

  drawstr `fwmul 4` `bsmul 1`  "VIA Technologies, Inc."
  drawstr `fwmul 4` `bsmul 3`  $(echo $board|tr [a-z] [A-Z]) "Firmware Installation"
  drawstr `fwmul 4` `bsmul 4`  "----------------------------------"

  drawstr `fwmul 6` `bsmul 6`  "Android ?"
  drawstr `fwmul 6` `bsmul 8`  "U-Boot ?"
  drawstr `fwmul 6` `bsmul 10` "Linux ?"

  ratio 0

  for f in boot-${board}.img \
           recovery-${board}.img \
           system.img \
           u-boot-${board}.imx
  do
    if [ ! -e "$srcdir/$f" ]; then
      set_font_color 255 0 0
      msg "halt. $f is missing."
      exit 255
    else
      msg "found $f"
      sleep 0.5s
    fi
  done

  show_warnings_by_ticker

  msg "checking firmware integrity."
  sleep 1

  local syslog=/tmp/syslog

  start_dots
  strings $srcdir/system.img | grep -e ro.build -e vermagic > $syslog
  stop_dots

  local linux_version=$(cat /proc/version | cut -d\  -f3 | cut -d- -f1)
  local android_version=$(grep ro.build.version.release= $syslog | \
    cut -d= -f2)
  local build_date=$(grep ro.build.date= $syslog | \
    cut -d= -f2 | awk '{ print $2 " " $3 " " $6 " - " $4 }')

  str=$(strings $srcdir/u-boot-${board}.imx |grep 'U-Boot 20'|grep \()
  str1=$(echo $str|cut -d\  -f1)
  str2=$(echo $str|cut -d\  -f2|cut -d- -f1)
  str3=\($(echo $str|cut -d\( -f2)
  local uboot_version="$str1 $str2 $str3"

  drawstr `fwmul 6` `bsmul 6`  "Android $android_version ($build_date)"
  drawstr `fwmul 6` `bsmul 8`  $uboot_version
  drawstr `fwmul 6` `bsmul 10`  "Linux version $linux_version"
}

cat /dev/zero > /dev/fb0
# fbset -g 1080 1920 1080 1920 32
fbset > /etc/fb.modes

bspinst_start
render_ui

# create device node
mdev -s

umount /dev/mmcblk0*

ratio 1; msg "."; sleep 1
#ratio 2; msg "."; sleep 1
#ratio 3; msg "."; sleep 1

mkfs_ext4 ()
{
  label=$1
  blkdev=$2
  if [ -e /dev/$blkdev ];then
    msg "format $label ($blkdev)"
    mkfs.ext4 -v -L $label /dev/$blkdev
    sleep 1
  fi
}

inst_fw ()
{
  firmware=$1
  blkdev=$2
  if [ -e /dev/$blkdev ] && [ -e $srcdir/$firmware ]; then
    msg "installing $firmware ($blkdev)"
    dd if=$srcdir/$firmware of=/dev/$blkdev
    sleep 1
  fi
}

inst_simg ()
{
  firmware=$1
  blkdev=$2
  if [ -e /dev/$blkdev ] && [ -e $srcdir/$firmware ]; then
    msg "installing $firmware ($blkdev)"
    simg2img $srcdir/$firmware /dev/$blkdev
    sleep 1
  fi
}

if [ -z "$board" ] || [ "$board" = "?" ]; then
  msg "cannot detect board name. abort!"
  bspinst_stop
  exit 255
fi

# 1 boot        16M
# 2 recovery    16M
# 3 dummy        1M
# 4 data      2048M
# 5 system     800M
# 6 cache      512M
# 7 device       8M
# 8 misc         6M
# 9 datafooter   2M

device=/dev/mmcblk0
cmdfile=/tmp/mkpart.cmd

echo "unit MiB"                  > $cmdfile
echo "mkpart primary 8 24"      >> $cmdfile
echo "mkpart primary 24 40"     >> $cmdfile
echo "mkpart extended 40 1400"  >> $cmdfile
echo "mkpart primary 1400 -1"   >> $cmdfile
echo "mkpart logical 41 841"    >> $cmdfile
echo "mkpart logical 842 1354"  >> $cmdfile
echo "mkpart logical 1355 1363" >> $cmdfile
echo "mkpart logical 1364 1370" >> $cmdfile
echo "mkpart logical 1371 1373" >> $cmdfile
echo "print"                    >> $cmdfile
echo "quit"                     >> $cmdfile

dd if=/dev/zero of=$device bs=1k count=1 conv=notrunc
parted -s $device mktable msdos
cat $cmdfile | parted $device
partprobe

sleep 3

# Partprobe will notify udevd to mount partitions.
# We have to unmount all partitions again.
umount /dev/mmcblk0*
df -h

ratio 13; mkfs_ext4 device   mmcblk0p7
ratio 14; mkfs_ext4 cache    mmcblk0p6
ratio 15; mkfs_ext4 userdata mmcblk0p4

ratio 20; inst_fw boot-${board}.img     mmcblk0p1
ratio 25; inst_fw recovery-${board}.img mmcblk0p2

if [ -e $srcdir/u-boot-${board}.imx ]; then
  ratio 30; msg "erasing spi flash (mtd0)"
  flash_erase /dev/mtd0 0 0
  ratio 35; msg installing u-boot-${board}.imx
  dd if=$srcdir/u-boot-${board}.imx of=/dev/mtd0 bs=1k seek=1
  dd if=$srcdir/u-boot-${board}.imx of=/dev/mmcblk0 bs=1k seek=1
  sleep 1
fi

ratio 40; inst_simg system.img mmcblk0p5

sync; sleep 1;               

ratio 100;

bspinst_stop

exit 0
