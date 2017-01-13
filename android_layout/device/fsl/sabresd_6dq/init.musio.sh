#!/system/bin/sh
pm disable com.android.launcher3
ime set com.akaintelligence.musio.musiosimplekeyboard/.services.MusioKeyboardService2
settings put secure spell_checker_enabled 0
