# This is a FSL Android Reference Design platform based on i.MX6Q ARD board
# It will inherit from FSL core product which in turn inherit from Google generic

$(call inherit-product, device/fsl/imx6/imx6.mk)
$(call inherit-product-if-exists,vendor/google/products/gms.mk)

ifneq ($(wildcard device/fsl/sabresd_6dq/fstab_nand.freescale),)
$(shell touch device/fsl/sabresd_6dq/fstab_nand.freescale)
endif

ifneq ($(wildcard device/fsl/sabresd_6dq/fstab.freescale),)
$(shell touch device/fsl/sabresd_6dq/fstab.freescale)
endif

# Overrides
PRODUCT_NAME := sabresd_6dq
PRODUCT_DEVICE := sabresd_6dq

PRODUCT_COPY_FILES += \
	device/fsl/sabresd_6dq/init.rc:root/init.freescale.rc \
        device/fsl/sabresd_6dq/init.i.MX6Q.rc:root/init.freescale.i.MX6Q.rc \
        device/fsl/sabresd_6dq/init.i.MX6DL.rc:root/init.freescale.i.MX6DL.rc \
	device/fsl/sabresd_6dq/init.i.MX6QP.rc:root/init.freescale.i.MX6QP.rc \
	device/fsl/sabresd_6dq/audio_policy.conf:system/etc/audio_policy.conf \
	device/fsl/sabresd_6dq/audio_effects.conf:system/vendor/etc/audio_effects.conf

PRODUCT_COPY_FILES +=	\
	external/linux-firmware-imx/firmware/vpu/vpu_fw_imx6d.bin:system/lib/firmware/vpu/vpu_fw_imx6d.bin 	\
	external/linux-firmware-imx/firmware/vpu/vpu_fw_imx6q.bin:system/lib/firmware/vpu/vpu_fw_imx6q.bin

# File copy for simcom 3G modem
PRODUCT_COPY_FILES += \
	device/fsl/sabresd_6dq/modem/simcom/3gdata_call.conf:system/bin/3gdata_call.conf \
	device/fsl/sabresd_6dq/modem/simcom/init.gprs-pppd:system/bin/init.gprs-pppd \
	device/fsl/sabresd_6dq/modem/simcom/rild:system/bin/rild \
	device/fsl/sabresd_6dq/modem/simcom/libreference-ril.so:system/lib/libreference-ril.so \
	device/fsl/sabresd_6dq/modem/simcom/libril.so:system/lib/libril.so

# setup dm-verity configs.
ifneq ($(BUILD_TARGET_DEVICE),sd)
 PRODUCT_SYSTEM_VERITY_PARTITION := /dev/block/mmcblk3p5
 $(call inherit-product, build/target/product/verity.mk)
else 
 PRODUCT_SYSTEM_VERITY_PARTITION := /dev/block/mmcblk2p5
 $(call inherit-product, build/target/product/verity.mk)

endif

# GPU files

DEVICE_PACKAGE_OVERLAYS := device/fsl/sabresd_6dq/overlay

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
	device/fsl/sabresd_6dq/required_hardware.xml:system/etc/permissions/required_hardware.xml
PRODUCT_PACKAGES += AudioRoute

# bsservice
PRODUCT_COPY_FILES += device/fsl/sabresd_6dq/bss_musio:system/bin/bss_musio

$(call inherit-product-if-exists, device/ti/proprietary-open/wl12xx/wlan/wl12xx-wlan-fw-products.mk)
$(call inherit-product-if-exists, device/ti/proprietary-open/wl12xx/wpan/wl12xx-wpan-fw-products.mk)
$(call inherit-product-if-exists, hardware/ti/wpan/ti-wpan-products.mk)
