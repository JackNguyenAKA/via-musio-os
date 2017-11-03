#!/bin/sh
# Copyright (c) 2016  VIA Technologies, Inc. All rights reserved.

bindir=$PWD/bin:$PWD/bspinst/bin:/mnt/mmcblk1p1/bspinst/bin:/mnt/sda1/bspinst/bin:/usr/bin

export PATH=$bindir:$PATH

exec $(which bspinst_boot_musio.sh)
