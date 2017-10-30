if  [ ls /system/etc/wl18xx-fw-4.bin ]
then
	rm /system/etc/firmware/ti-connectivity/wl18xx-fw-4.bin
	mv /system/etc/wl18xx-fw-4.bin /system/etc/firmware/ti-connectivity/wl18xx-fw-4.bin
	chmod 644 /system/etc/firmware/ti-connectivity/wl18xx-fw-4.bin
fi
