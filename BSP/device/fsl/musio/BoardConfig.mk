#
# Product-specific compile-time definitions.
#

include device/fsl/imx6/soc/imx6dq.mk
include device/fsl/sabresd_6dq/build_id.mk
include device/fsl/imx6/BoardConfigCommon.mk
include device/fsl-proprietary/gpu-viv/fsl-gpu.mk
include device/via-proprietary/mpu-9250/mpu-9250.mk
# sabresd_6dq default target for EXT4
BUILD_TARGET_FS ?= ext4
include device/fsl/imx6/imx6_target_fs.mk

ifeq ($(BUILD_TARGET_DEVICE),sd)
ADDITIONAL_BUILD_PROPERTIES += \
                        ro.boot.storage_type=sd
ifneq ($(BUILD_TARGET_FS),f2fs)
TARGET_RECOVERY_FSTAB = device/fsl/sabresd_6dq/fstab_sd.freescale
# build for ext4
PRODUCT_COPY_FILES +=	\
	device/fsl/sabresd_6dq/fstab_sd.freescale:root/fstab.freescale
else
TARGET_RECOVERY_FSTAB = device/fsl/sabresd_6dq/fstab_sd-f2fs.freescale
# build for f2fs
PRODUCT_COPY_FILES +=	\
	device/fsl/sabresd_6dq/fstab_sd-f2fs.freescale:root/fstab.freescale
endif # BUILD_TARGET_FS
else
ADDITIONAL_BUILD_PROPERTIES += \
                        ro.boot.storage_type=emmc
ifneq ($(BUILD_TARGET_FS),f2fs)
TARGET_RECOVERY_FSTAB = device/fsl/sabresd_6dq/fstab.freescale
# build for ext4
PRODUCT_COPY_FILES +=	\
	device/fsl/sabresd_6dq/fstab.freescale:root/fstab.freescale
else
TARGET_RECOVERY_FSTAB = device/fsl/sabresd_6dq/fstab-f2fs.freescale
# build for f2fs
PRODUCT_COPY_FILES +=	\
	device/fsl/sabresd_6dq/fstab-f2fs.freescale:root/fstab.freescale
endif # BUILD_TARGET_FS
endif # BUILD_TARGET_DEVICE


TARGET_BOOTLOADER_BOARD_NAME := SABRESD
PRODUCT_MODEL := SABRESD-MX6DQ

TARGET_RELEASETOOLS_EXTENSIONS := device/fsl/imx6
# UNITE is a virtual device support both atheros and realtek wifi(ar6103 and rtl8723as)
#BOARD_WLAN_DEVICE            := UNITE
#WPA_SUPPLICANT_VERSION       := VER_0_8_UNITE
TARGET_KERNEL_MODULES        := \
				hardware/ti/wlan/mac80211/compat_wl18xx/compat/compat.ko:system/lib/modules/compat.ko \
				hardware/ti/wlan/mac80211/compat_wl18xx/net/mac80211/mac80211.ko:system/lib/modules/mac80211.ko \
				hardware/ti/wlan/mac80211/compat_wl18xx/net/wireless/cfg80211.ko:system/lib/modules/cfg80211.ko \
				hardware/ti/wlan/mac80211/compat_wl18xx/drivers/net/wireless/ti/wl18xx/wl18xx.ko:system/lib/modules/wl18xx.ko \
				hardware/ti/wlan/mac80211/compat_wl18xx/drivers/net/wireless/ti/wlcore/wlcore.ko:system/lib/modules/wlcore.ko \
				hardware/ti/wlan/mac80211/compat_wl18xx/drivers/net/wireless/ti/wlcore/wlcore_sdio.ko:system/lib/modules/wlcore_sdio.ko
#                               kernel_imx/net/wireless/cfg80211.ko:system/lib/modules/cfg80211_realtek.ko \
#                               kernel_imx/drivers/net/wireless/rtlwifi/rtl8821as/8821as.ko:system/lib/modules/8821as.ko \
#                               kernel_imx/drivers/net/wireless/bcmdhd/bcmdhd.ko:system/lib/modules/bcmdhd.ko

BOARD_WPA_SUPPLICANT_DRIVER  := NL80211
BOARD_HOSTAPD_DRIVER         := NL80211

BOARD_HOSTAPD_PRIVATE_LIB_RTL               := lib_driver_cmd_rtl
BOARD_WPA_SUPPLICANT_PRIVATE_LIB_RTL        := lib_driver_cmd_rtl
BOARD_HOSTAPD_PRIVATE_LIB_BCM               := lib_driver_cmd_bcmdhd
BOARD_WPA_SUPPLICANT_PRIVATE_LIB_BCM        := lib_driver_cmd_bcmdhd

#BOARD_SUPPORT_BCM_WIFI  := true
#BOARD_SUPPORT_RTL_WIFI  := true

#for intel vendor
ifeq ($(BOARD_WLAN_VENDOR),INTEL)
BOARD_HOSTAPD_PRIVATE_LIB                := private_lib_driver_cmd
BOARD_WPA_SUPPLICANT_PRIVATE_LIB         := private_lib_driver_cmd
WPA_SUPPLICANT_VERSION                   := VER_0_8_X
HOSTAPD_VERSION                          := VER_0_8_X
BOARD_WPA_SUPPLICANT_PRIVATE_LIB         := private_lib_driver_cmd_intel
WIFI_DRIVER_MODULE_PATH                  := "/system/lib/modules/iwlagn.ko"
WIFI_DRIVER_MODULE_NAME                  := "iwlagn"
WIFI_DRIVER_MODULE_PATH                  ?= auto
endif

# Connectivity - Wi-Fi
USES_TI_MAC80211 := true
ifeq ($(USES_TI_MAC80211),true)
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
WPA_SUPPLICANT_VERSION      := VER_0_8_X
BOARD_HOSTAPD_DRIVER        := NL80211
BOARD_WPA_SUPPLICANT_PRIVATE_LIB  := lib_driver_cmd_wl12xx
BOARD_HOSTAPD_PRIVATE_LIB         := lib_driver_cmd_wl12xx
BOARD_WLAN_DEVICE           := wl12xx_mac80211
BOARD_SOFTAP_DEVICE         := wl12xx_mac80211
COMMON_GLOBAL_CFLAGS += -DUSES_TI_MAC80211
COMMON_GLOBAL_CFLAGS += -DANDROID_P2P_STUB
endif

WIFI_DRIVER_FW_PATH_STA        := "/system/etc/firmware/bcm/fw_bcmdhd.bin"
WIFI_DRIVER_FW_PATH_P2P        := "/system/etc/firmware/bcm/fw_bcmdhd.bin"
WIFI_DRIVER_FW_PATH_AP         := "/system/etc/firmware/bcm/fw_bcmdhd_apsta.bin"
WIFI_DRIVER_FW_PATH_PARAM      := "/sys/module/bcmdhd/parameters/firmware_path"

BOARD_MODEM_VENDOR := AMAZON

USE_ATHR_GPS_HARDWARE := true
USE_QEMU_GPS_HARDWARE := false

#for accelerator sensor, need to define sensor type here
BOARD_HAS_SENSOR := true
SENSOR_MMA8451 := true

# for recovery service
TARGET_SELECT_KEY := 28

# we don't support sparse image.
TARGET_USERIMAGES_SPARSE_EXT_DISABLED := false
DM_VERITY_RUNTIME_CONFIG := true
# uncomment below lins if use NAND
#TARGET_USERIMAGES_USE_UBIFS = true


ifeq ($(TARGET_USERIMAGES_USE_UBIFS),true)
UBI_ROOT_INI := device/fsl/sabresd_6dq/ubi/ubinize.ini
TARGET_MKUBIFS_ARGS := -m 4096 -e 516096 -c 4096 -x none
TARGET_UBIRAW_ARGS := -m 4096 -p 512KiB $(UBI_ROOT_INI)
endif

ifeq ($(TARGET_USERIMAGES_USE_UBIFS),true)
ifeq ($(TARGET_USERIMAGES_USE_EXT4),true)
$(error "TARGET_USERIMAGES_USE_UBIFS and TARGET_USERIMAGES_USE_EXT4 config open in same time, please only choose one target file system image")
endif
endif

BOARD_KERNEL_CMDLINE := console=ttymxc1,115200 init=/init video=mxcfb0:dev=lcd,HUAQIN-TXDT550,if=RGB24,bpp=32 video=mxcfb1:off video=mxcfb2:off video=mxcfb3:off fbmem=28M vmalloc=400M androidboot.console=ttymxc1 consoleblank=0 androidboot.hardware=freescale audio=adau1761 androidboot.selinux=disabled

ifeq ($(TARGET_USERIMAGES_USE_UBIFS),true)
#UBI boot command line.
# Note: this NAND partition table must align with MFGTool's config.
BOARD_KERNEL_CMDLINE +=  mtdparts=gpmi-nand:16m(bootloader),16m(bootimg),128m(recovery),-(root) gpmi_debug_init ubi.mtd=3
endif


# Broadcom BCM4339 BT
#BOARD_HAVE_BLUETOOTH_BCM := true
BOARD_HAVE_BLUETOOTH_TI := true
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := device/fsl/sabresd_6dq/bluetooth

USE_ION_ALLOCATOR := false
USE_GPU_ALLOCATOR := true

# camera hal v2
IMX_CAMERA_HAL_V2 := true

#define consumer IR HAL support
IMX6_CONSUMER_IR_HAL := true

TARGET_BOOTLOADER_CONFIG := musio:mx6qsabreliteandroid_config vab820:mx6qsabreliteandroid_config imx6q:mx6qsabresdandroid_config imx6dl:mx6dlsabresdandroid_config imx6qp:mx6qpsabresdandroid_config
TARGET_BOARD_DTS_CONFIG := musio:imx6q-musio.dtb vab820:imx6q-vab820.dtb imx6q:imx6q-sabresd.dtb imx6dl:imx6dl-sabresd.dtb imx6q-ldo:imx6q-sabresd-ldo.dtb imx6qp:imx6qp-sabresd.dtb

BOARD_SEPOLICY_DIRS := \
       device/fsl/imx6/sepolicy \
       device/fsl/sabresd_6dq/sepolicy

BOARD_SEPOLICY_UNION := \
       domain.te \
       system_app.te \
       system_server.te \
       untrusted_app.te \
       sensors.te \
       init_shell.te \
       bluetooth.te \
       kernel.te \
       mediaserver.te \
       file_contexts \
       genfs_contexts \
       fs_use  \
       rild.te \
       init.te \
       netd.te \
       bootanim.te \
       dnsmasq.te \
       recovery.te \
       device.te \
       zygote.te


