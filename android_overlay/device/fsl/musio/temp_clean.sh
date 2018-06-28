#!/system/bin/sh
setprop musio.persist.reboot 0
touch /sdcard/firstboot
pm clear com.google.android.gms
