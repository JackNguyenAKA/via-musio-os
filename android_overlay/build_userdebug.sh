#!/bin/bash
source build/envsetup.sh
lunch aosp_musio-userdebug
make -j16
