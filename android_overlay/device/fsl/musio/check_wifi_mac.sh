#!/system/bin/sh
dd if=/dev/mtd/mtd0 of=/data/report bs=4k skip=1023 count=1
serial=`cat /data/report | grep serial`
$serial
