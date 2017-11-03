#!/bin/sh
# Copyright (c) 2016  VIA Technologies, Inc. All rights reserved.
#
# Add-ons for user-defined installation
#

printf "\nExecute bspinst_addons.sh\n\n"

bindir=$PWD/bin:$PWD/bspinst/bin:/mnt/mmcblk0p1/bspinst/bin:/mnt/sda1/bspinst/bin:/usr/bin

export PATH=$bindir:$PATH

source `which bspinst_api.sh`

printf "\nExecute bspinst_addons.sh done. \n\n"

