#!/system/bin/sh
ime set com.akaintelligence.musio.musiosimplekeyboard/.services.MusioKeyboardService2
settings put secure spell_checker_enabled 0
setprop musio.boot false
serprop musio.init true
