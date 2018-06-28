#!/system/bin/sh
ime set com.akaintelligence.musio.keyboard/.services.MusioKeyboard
settings put secure spell_checker_enabled 0
[ -f "/sdcard/firstboot" ] || setprop musio.persist.reboot 1 
setprop musio.boot 0
start adbd
