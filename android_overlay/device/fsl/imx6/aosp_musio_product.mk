# This is a FSL Android Reference Design platform based on i.MX6Q ARD board
# It will inherit from FSL core product which in turn inherit from Google generic
# For 3.3.0 with new dictionary API and Wifi login function
# With Edu-Cambridge from Darren

$(call inherit-product, device/fsl/imx6/imx6.mk)
#$(call inherit-product-if-exists,vendor/google/products/gms.mk)
$(call inherit-product, vendor/google/build/opengapps-for-musio-2.mk)

ifneq ($(wildcard device/fsl/musio/fstab_nand.freescale),)
$(shell touch device/fsl/musio/fstab_nand.freescale)
endif

ifneq ($(wildcard device/fsl/musio/fstab.freescale),)
$(shell touch device/fsl/musio/fstab.freescale)
endif

GAPPS_VARIANT := nano

# Overrides
PRODUCT_NAME := aosp_musio
PRODUCT_DEVICE := musio

PRODUCT_BRAND := Softbank
PRODUCT_MODEL := MUSIO X
PRODUCT_MANUFACTURER := AKA
PRODUCT_LOCALES := en_US


PRODUCT_PACKAGES += \
    AppsAlarm \
    AppsAcademyLauncher \
    AppsAcademyIcebreaker \
    AppsClientManager \
    AppsDictionaryJpn2Eng \
    AppsDictionaryEng2Jpn \
    AppsEduhub \
    AppsGamehub \
    AppsGamehubMusioAdventure \
    AppsGamehubTicTacToe \
    AppsLearnersChat \
    AppsRadio \
    AppsSophy \
    AppsSophyBubble \
    AppsSophyCardTarot\
    AppsSophyCalculator\
    AppsSophyCardMusicLaunchpad \
    AppsSophyCardMusicVideo \
    AppsSophyCollectRocket \
    AppsSophyMusioAdventure \
    AppsSophyProgramming \
    AppsSophyKitchen \
    AppsSophyTutor \
    AppsUpdate \
    AppsWardrobe \
    AppsInspect \
    MusioLauncher \
    MusioMemberProfile \
    MusioSetting \
    MusioArduino \
    MusioController \
    MusioEmotion \
    MusioLiveFace \
    MusioHome \
    MusioMember \
    MusioMode \
    MusioMuse \
    MusioKeyboard \
    MusioPush \
    MusioSophy \
    MusioSphinx \
    MusioStatus \
    MusioSystem \
    MusioTimeout \
    MusioTouch \
    MusioTTS \
    MusioUI \
    MusioVision \
    AppsEdu \
    AppsEduCambridge \
PRODUCT_COPY_FILES += \
	device/fsl/musio/init.rc:root/init.freescale.rc \
        device/fsl/musio/init.i.MX6Q.rc:root/init.freescale.i.MX6Q.rc \
        device/fsl/musio/init.i.MX6DL.rc:root/init.freescale.i.MX6DL.rc \
	device/fsl/musio/init.i.MX6QP.rc:root/init.freescale.i.MX6QP.rc \
	device/fsl/musio/init.musio.sh:system/etc/init.musio.sh \
	device/fsl/musio/init.window.sh:system/etc/init.window.sh \
	device/fsl/musio/temp_clean.sh:system/etc/temp_clean.sh \
	device/fsl/musio/init.firmware.sh:system/etc/init.firmware.sh \
	device/fsl/musio/audio_policy.conf:system/etc/audio_policy.conf \
	device/fsl/musio/audio_effects.conf:system/vendor/etc/audio_effects.conf \
	device/fsl/musio/check_wifi_mac.sh:system/etc/check_wifi_mac.sh \
	device/fsl/musio/power_off.ogg:system/media/audio/ui/power_off.ogg \
	device/fsl/musio/wl18xx-fw-4.bin:system/etc/wl18xx-fw-4.bin \
	#device/fsl/musio/bootanimation.zip:system/media/bootanimation.zip \

PRODUCT_COPY_FILES += \
    packages/apps/libpocketsphinx_jni.so:system/lib/libpocketsphinx_jni.so \
    packages/apps/libengine.so:system/lib/libengine.so \
    packages/apps/libmultitouch2.so:system/lib/libmultitouch2.so \
    packages/apps/libserial_port.so:system/lib/libserial_port.so \
    packages/apps/liblipitk.so:system/lib/liblipitk.so \
    packages/apps/librealm-jni.so:system/lib/librealm-jni.so \
    packages/apps/libdetection_based_tracker.so:system/lib/libdetection_based_tracker.so \
    packages/apps/libopencv_java3.so:system/lib/libopencv_java3.so \
    packages/apps/libgdx.so:system/lib/libgdx.so \
    packages/apps/libgdx-box2d.so:system/lib/libgdx-box2d.so \

PRODUCT_COPY_FILES +=	\
	external/linux-firmware-imx/firmware/vpu/vpu_fw_imx6d.bin:system/lib/firmware/vpu/vpu_fw_imx6d.bin 	\
	external/linux-firmware-imx/firmware/vpu/vpu_fw_imx6q.bin:system/lib/firmware/vpu/vpu_fw_imx6q.bin

# File copy for simcom 3G modem
PRODUCT_COPY_FILES += \
	device/fsl/musio/modem/simcom/3gdata_call.conf:system/bin/3gdata_call.conf \
	device/fsl/musio/modem/simcom/init.gprs-pppd:system/bin/init.gprs-pppd \
	device/fsl/musio/modem/simcom/rild:system/bin/rild \
	device/fsl/musio/modem/simcom/libreference-ril.so:system/lib/libreference-ril.so \
	device/fsl/musio/modem/simcom/libril.so:system/lib/libril.so

# setup dm-verity configs.
ifneq ($(BUILD_TARGET_DEVICE),sd)
 PRODUCT_SYSTEM_VERITY_PARTITION := /dev/block/mmcblk3p5
 $(call inherit-product, build/target/product/verity.mk)
else 
 PRODUCT_SYSTEM_VERITY_PARTITION := /dev/block/mmcblk2p5
 $(call inherit-product, build/target/product/verity.mk)

endif

# GPU files

DEVICE_PACKAGE_OVERLAYS := device/fsl/musio/overlay

PRODUCT_CHARACTERISTICS := tablet

PRODUCT_AAPT_CONFIG += xlarge large tvdpi hdpi

PRODUCT_COPY_FILES += \
	frameworks/native/data/etc/tablet_core_hardware.xml:system/etc/permissions/tablet_core_hardware.xml \
	frameworks/native/data/etc/android.hardware.camera.xml:system/etc/permissions/android.hardware.camera.xml \
	frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
	frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
	frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
	frameworks/native/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
	frameworks/native/data/etc/android.hardware.faketouch.xml:system/etc/permissions/android.hardware.faketouch.xml \
	frameworks/native/data/etc/android.software.sip.voip.xml:system/etc/permissions/android.software.sip.voip.xml \
	frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
	frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
	frameworks/native/data/etc/android.hardware.bluetooth.xml:system/etc/permissions/android.hardware.bluetooth.xml \
	frameworks/native/data/etc/android.hardware.bluetooth_le.xml:system/etc/permissions/android.hardware.bluetooth_le.xml \
	frameworks/native/data/etc/android.hardware.consumerir.xml:system/etc/permissions/android.hardware.consumerir.xml \
	frameworks/native/data/etc/android.hardware.ethernet.xml:system/etc/permissions/android.hardware.ethernet.xml \
	frameworks/native/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml \
	frameworks/native/data/etc/android.hardware.telephony.cdma.xml:system/etc/permissions/android.hardware.telephony.cdma.xml \
	device/fsl/musio/required_hardware.xml:system/etc/permissions/required_hardware.xml
PRODUCT_PACKAGES += AudioRoute

# bsservice
PRODUCT_COPY_FILES += device/fsl/musio/bss_musio:system/bin/bss_musio

$(call inherit-product-if-exists, device/ti/proprietary-open/wl12xx/wlan/wl12xx-wlan-fw-products.mk)
$(call inherit-product-if-exists, device/ti/proprietary-open/wl12xx/wpan/wl12xx-wpan-fw-products.mk)
$(call inherit-product-if-exists, hardware/ti/wpan/ti-wpan-products.mk)
