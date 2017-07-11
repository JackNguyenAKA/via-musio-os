#!/system/bin/sh
ime set com.akaintelligence.musio.keyboard/.services.MusioKeyboard
settings put secure spell_checker_enabled 0
setprop musio.boot 0
start adbd
