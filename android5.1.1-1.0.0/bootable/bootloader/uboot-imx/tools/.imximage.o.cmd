cmd_tools/imximage.o := cc -Wp,-MD,tools/.imximage.o.d -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer   -include ./include/libfdt_env.h -idirafterinclude -idirafter./arch/arm/include -I./lib/libfdt -I./tools -DCONFIG_SYS_TEXT_BASE=0x17800000 -DUSE_HOSTCC -D__KERNEL_STRICT_NAMES -D_GNU_SOURCE  -c -o tools/imximage.o tools/imximage.c

source_tools/imximage.o := tools/imximage.c

deps_tools/imximage.o := \
    $(wildcard include/config/use/plugin.h) \
  /usr/include/stdc-predef.h \
  include/libfdt_env.h \
  include/compiler.h \
    $(wildcard include/config/use/stdint.h) \
  /usr/lib/gcc/x86_64-linux-gnu/5/include/stddef.h \
  /usr/lib/gcc/x86_64-linux-gnu/5/include/stdint.h \
  /usr/include/stdint.h \
  /usr/include/features.h \
  /usr/include/x86_64-linux-gnu/sys/cdefs.h \
  /usr/include/x86_64-linux-gnu/bits/wordsize.h \
  /usr/include/x86_64-linux-gnu/gnu/stubs.h \
  /usr/include/x86_64-linux-gnu/gnu/stubs-64.h \
  /usr/include/x86_64-linux-gnu/bits/wchar.h \
  /usr/include/errno.h \
  /usr/include/x86_64-linux-gnu/bits/errno.h \
  /usr/include/linux/errno.h \
  /usr/include/x86_64-linux-gnu/asm/errno.h \
  /usr/include/asm-generic/errno.h \
  /usr/include/asm-generic/errno-base.h \
  /usr/include/stdlib.h \
  /usr/include/x86_64-linux-gnu/bits/waitflags.h \
  /usr/include/x86_64-linux-gnu/bits/waitstatus.h \
  /usr/include/endian.h \
  /usr/include/x86_64-linux-gnu/bits/endian.h \
  /usr/include/x86_64-linux-gnu/bits/byteswap.h \
  /usr/include/x86_64-linux-gnu/bits/types.h \
  /usr/include/x86_64-linux-gnu/bits/typesizes.h \
  /usr/include/x86_64-linux-gnu/bits/byteswap-16.h \
  /usr/include/xlocale.h \
  /usr/include/x86_64-linux-gnu/sys/types.h \
  /usr/include/time.h \
  /usr/include/x86_64-linux-gnu/sys/select.h \
  /usr/include/x86_64-linux-gnu/bits/select.h \
  /usr/include/x86_64-linux-gnu/bits/sigset.h \
  /usr/include/x86_64-linux-gnu/bits/time.h \
  /usr/include/x86_64-linux-gnu/bits/select2.h \
  /usr/include/x86_64-linux-gnu/sys/sysmacros.h \
  /usr/include/x86_64-linux-gnu/bits/pthreadtypes.h \
  /usr/include/alloca.h \
  /usr/include/x86_64-linux-gnu/bits/stdlib-bsearch.h \
  /usr/include/x86_64-linux-gnu/bits/stdlib-float.h \
  /usr/include/x86_64-linux-gnu/bits/stdlib.h \
  /usr/include/stdio.h \
  /usr/include/libio.h \
  /usr/include/_G_config.h \
  /usr/include/wchar.h \
  /usr/lib/gcc/x86_64-linux-gnu/5/include/stdarg.h \
  /usr/include/x86_64-linux-gnu/bits/stdio_lim.h \
  /usr/include/x86_64-linux-gnu/bits/sys_errlist.h \
  /usr/include/x86_64-linux-gnu/bits/stdio.h \
  /usr/include/x86_64-linux-gnu/bits/stdio2.h \
  /usr/include/string.h \
  /usr/include/x86_64-linux-gnu/bits/string.h \
  /usr/include/x86_64-linux-gnu/bits/string2.h \
  /usr/include/x86_64-linux-gnu/bits/string3.h \
  /usr/include/x86_64-linux-gnu/sys/mman.h \
  /usr/include/x86_64-linux-gnu/bits/mman.h \
  /usr/include/x86_64-linux-gnu/bits/mman-linux.h \
  /usr/include/fcntl.h \
  /usr/include/x86_64-linux-gnu/bits/fcntl.h \
  /usr/include/x86_64-linux-gnu/bits/fcntl-linux.h \
  /usr/include/x86_64-linux-gnu/bits/uio.h \
  /usr/include/x86_64-linux-gnu/bits/stat.h \
  /usr/include/x86_64-linux-gnu/bits/fcntl2.h \
  /usr/include/byteswap.h \
  /usr/include/x86_64-linux-gnu/bits/timex.h \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
  /usr/include/linux/posix_types.h \
  /usr/include/linux/stddef.h \
  /usr/include/x86_64-linux-gnu/asm/posix_types.h \
  /usr/include/x86_64-linux-gnu/asm/posix_types_64.h \
  /usr/include/asm-generic/posix_types.h \
  /usr/include/x86_64-linux-gnu/asm/bitsperlong.h \
  /usr/include/asm-generic/bitsperlong.h \
    $(wildcard include/config/64bit.h) \
  /usr/include/x86_64-linux-gnu/asm/types.h \
  /usr/include/asm-generic/types.h \
  /usr/include/asm-generic/int-ll64.h \
  /usr/lib/gcc/x86_64-linux-gnu/5/include/stdbool.h \
  tools/imagetool.h \
  tools/os_support.h \
  include/compiler.h \
  /usr/include/x86_64-linux-gnu/sys/stat.h \
  /usr/include/unistd.h \
  /usr/include/x86_64-linux-gnu/bits/posix_opt.h \
  /usr/include/x86_64-linux-gnu/bits/environments.h \
  /usr/include/x86_64-linux-gnu/bits/confname.h \
  /usr/include/getopt.h \
  /usr/include/x86_64-linux-gnu/bits/unistd.h \
  include/u-boot/sha1.h \
  tools/fdt_host.h \
  tools/../include/libfdt.h \
  include/libfdt_env.h \
  include/fdt.h \
  tools/../include/fdt_support.h \
    $(wildcard include/config/of/libfdt.h) \
    $(wildcard include/config/has/fsl/dr/usb.h) \
    $(wildcard include/config/has/fsl/mph/usb.h) \
    $(wildcard include/config/sys/fsl/sec/compat.h) \
    $(wildcard include/config/pci.h) \
    $(wildcard include/config/sys/fdt/pad.h) \
    $(wildcard include/config/of/board/setup.h) \
    $(wildcard include/config/of/system/setup.h) \
  include/image.h \
    $(wildcard include/config/fit.h) \
    $(wildcard include/config/fit/verbose.h) \
    $(wildcard include/config/spl/build.h) \
    $(wildcard include/config/spl/crc32/support.h) \
    $(wildcard include/config/spl/md5/support.h) \
    $(wildcard include/config/spl/sha1/support.h) \
    $(wildcard include/config/spl/sha256/support.h) \
    $(wildcard include/config/crc32.h) \
    $(wildcard include/config/md5.h) \
    $(wildcard include/config/sha1.h) \
    $(wildcard include/config/sha256.h) \
    $(wildcard include/config/fit/disable/sha256.h) \
    $(wildcard include/config/sys/boot/ramdisk/high.h) \
    $(wildcard include/config/sys/boot/get/cmdline.h) \
    $(wildcard include/config/lmb.h) \
    $(wildcard include/config/timestamp.h) \
    $(wildcard include/config/cmd/date.h) \
    $(wildcard include/config/image/format/legacy.h) \
    $(wildcard include/config/sys/boot/get/kbd.h) \
    $(wildcard include/config/fit/signature.h) \
    $(wildcard include/config/fit/best/match.h) \
    $(wildcard include/config/android/boot/image.h) \
  include/compiler.h \
  /usr/include/x86_64-linux-gnu/asm/byteorder.h \
  /usr/include/linux/byteorder/little_endian.h \
  /usr/include/linux/types.h \
  /usr/include/linux/swab.h \
  /usr/include/x86_64-linux-gnu/asm/swab.h \
  include/hash.h \
    $(wildcard include/config/sha1sum/verify.h) \
    $(wildcard include/config/crc32/verify.h) \
    $(wildcard include/config/hash/verify.h) \
  include/libfdt.h \
  include/fdt_support.h \
  tools/imximage.h \
    $(wildcard include/config/imx/fixed/ivt/offset.h) \
  include/config.h \
    $(wildcard include/config/imx/config.h) \
    $(wildcard include/config/mx6qp.h) \
    $(wildcard include/config/android/support.h) \
    $(wildcard include/config/boarddir.h) \
  include/config_defaults.h \
    $(wildcard include/config/defaults/h/.h) \
    $(wildcard include/config/bootm/linux.h) \
    $(wildcard include/config/bootm/netbsd.h) \
    $(wildcard include/config/bootm/plan9.h) \
    $(wildcard include/config/bootm/rtems.h) \
    $(wildcard include/config/bootm/vxworks.h) \
    $(wildcard include/config/gzip.h) \
    $(wildcard include/config/zlib.h) \
    $(wildcard include/config/partitions.h) \
  include/config_uncmd_spl.h \
    $(wildcard include/config/uncmd/spl/h//.h) \
    $(wildcard include/config/cmd/cdp.h) \
    $(wildcard include/config/cmd/dhcp.h) \
    $(wildcard include/config/cmd/dns.h) \
    $(wildcard include/config/cmd/link/local.h) \
    $(wildcard include/config/cmd/nfs.h) \
    $(wildcard include/config/cmd/ping.h) \
    $(wildcard include/config/cmd/rarp.h) \
    $(wildcard include/config/cmd/sntp.h) \
    $(wildcard include/config/cmd/tftpput.h) \
    $(wildcard include/config/cmd/tftpsrv.h) \
    $(wildcard include/config/of/control.h) \
    $(wildcard include/config/spl/dm.h) \
    $(wildcard include/config/dm/serial.h) \
    $(wildcard include/config/dm/gpio.h) \
    $(wildcard include/config/dm/i2c.h) \
    $(wildcard include/config/dm/spi.h) \
    $(wildcard include/config/dm/warn.h) \
    $(wildcard include/config/dm/device/remove.h) \
    $(wildcard include/config/dm/stdio.h) \
  include/configs/mx6sabresd.h \
    $(wildcard include/config/h.h) \
    $(wildcard include/config/spl.h) \
    $(wildcard include/config/spl/libcommon/support.h) \
    $(wildcard include/config/spl/mmc/support.h) \
    $(wildcard include/config/mach/type.h) \
    $(wildcard include/config/mxc/uart/base.h) \
    $(wildcard include/config/console/dev.h) \
    $(wildcard include/config/mmcroot.h) \
    $(wildcard include/config/default/fdt/file.h) \
    $(wildcard include/config/mx6q.h) \
    $(wildcard include/config/mx6dl.h) \
    $(wildcard include/config/mx6solo.h) \
    $(wildcard include/config/sys/fsl/usdhc/num.h) \
    $(wildcard include/config/sys/mmc/env/dev.h) \
    $(wildcard include/config/sys/mmc/env/part.h) \
    $(wildcard include/config/sys/use/spinor.h) \
    $(wildcard include/config/sf/default/cs.h) \
    $(wildcard include/config/cmd/pci.h) \
    $(wildcard include/config/pci/pnp.h) \
    $(wildcard include/config/pci/scan/show.h) \
    $(wildcard include/config/pcie/imx.h) \
    $(wildcard include/config/pcie/imx/perst/gpio.h) \
    $(wildcard include/config/pcie/imx/power/gpio.h) \
    $(wildcard include/config/power.h) \
    $(wildcard include/config/power/i2c.h) \
    $(wildcard include/config/power/pfuze100.h) \
    $(wildcard include/config/power/pfuze100/i2c/addr.h) \
    $(wildcard include/config/cmd/usb.h) \
    $(wildcard include/config/usb/ehci.h) \
    $(wildcard include/config/usb/ehci/mx6.h) \
    $(wildcard include/config/usb/storage.h) \
    $(wildcard include/config/ehci/hcd/init/after/reset.h) \
    $(wildcard include/config/usb/host/ether.h) \
    $(wildcard include/config/usb/ether/asix.h) \
    $(wildcard include/config/mxc/usb/portsc.h) \
    $(wildcard include/config/mxc/usb/flags.h) \
    $(wildcard include/config/usb/max/controller/count.h) \
    $(wildcard include/config/splash/screen.h) \
    $(wildcard include/config/mxc/epdc.h) \
    $(wildcard include/config/cmd/bmp.h) \
    $(wildcard include/config/lcd.h) \
    $(wildcard include/config/sys/console/is/in/env.h) \
    $(wildcard include/config/splash/is/in/mmc.h) \
    $(wildcard include/config/splash/screen/align.h) \
    $(wildcard include/config/waveform/buf/size.h) \
  arch/arm/include/asm/arch/imx-regs.h \
    $(wildcard include/config/mx6ul.h) \
    $(wildcard include/config/sys/cacheline/size.h) \
    $(wildcard include/config/mx6sl.h) \
    $(wildcard include/config/mx6sx.h) \
    $(wildcard include/config/sys/fsl/sec/addr.h) \
    $(wildcard include/config/sys/fsl/jr0/addr.h) \
    $(wildcard include/config/base/addr.h) \
  arch/arm/include/asm/imx-common/gpio.h \
  include/configs/mx6sabre_common.h \
    $(wildcard include/config/mx6.h) \
    $(wildcard include/config/secure/boot.h) \
    $(wildcard include/config/csf/size.h) \
    $(wildcard include/config/display/cpuinfo.h) \
    $(wildcard include/config/display/boardinfo.h) \
    $(wildcard include/config/cmdline/tag.h) \
    $(wildcard include/config/setup/memory/tags.h) \
    $(wildcard include/config/initrd/tag.h) \
    $(wildcard include/config/revision/tag.h) \
    $(wildcard include/config/sys/generic/board.h) \
    $(wildcard include/config/sys/malloc/len.h) \
    $(wildcard include/config/board/early/init/f.h) \
    $(wildcard include/config/board/late/init.h) \
    $(wildcard include/config/mxc/gpio.h) \
    $(wildcard include/config/mxc/uart.h) \
    $(wildcard include/config/cmd/fuse.h) \
    $(wildcard include/config/mxc/ocotp.h) \
    $(wildcard include/config/fsl/esdhc.h) \
    $(wildcard include/config/fsl/usdhc.h) \
    $(wildcard include/config/sys/fsl/esdhc/addr.h) \
    $(wildcard include/config/mmc.h) \
    $(wildcard include/config/cmd/mmc.h) \
    $(wildcard include/config/generic/mmc.h) \
    $(wildcard include/config/bounce/buffer.h) \
    $(wildcard include/config/cmd/ext2.h) \
    $(wildcard include/config/cmd/ext4.h) \
    $(wildcard include/config/cmd/ext4/write.h) \
    $(wildcard include/config/cmd/fat.h) \
    $(wildcard include/config/dos/partition.h) \
    $(wildcard include/config/support/emmc/boot.h) \
    $(wildcard include/config/cmd/mii.h) \
    $(wildcard include/config/cmd/net.h) \
    $(wildcard include/config/fec/mxc.h) \
    $(wildcard include/config/mii.h) \
    $(wildcard include/config/fec/xcv/type.h) \
    $(wildcard include/config/ethprime.h) \
    $(wildcard include/config/fec/mxc/phyaddr.h) \
    $(wildcard include/config/phylib.h) \
    $(wildcard include/config/phy/atheros.h) \
    $(wildcard include/config/env/overwrite.h) \
    $(wildcard include/config/cons/index.h) \
    $(wildcard include/config/baudrate.h) \
    $(wildcard include/config/cmd/bmode.h) \
    $(wildcard include/config/cmd/bootz.h) \
    $(wildcard include/config/cmd/setexpr.h) \
    $(wildcard include/config/cmd/imls.h) \
    $(wildcard include/config/bootdelay.h) \
    $(wildcard include/config/loadaddr.h) \
    $(wildcard include/config/sys/text/base.h) \
    $(wildcard include/config/sys/mmc/img/load/part.h) \
    $(wildcard include/config/sys/boot/nand.h) \
    $(wildcard include/config/mfg/nand/partition.h) \
    $(wildcard include/config/mfg/env/settings.h) \
    $(wildcard include/config/extra/env/settings.h) \
    $(wildcard include/config/sys/boot/sata.h) \
    $(wildcard include/config/sys/nosmp.h) \
    $(wildcard include/config/bootcommand.h) \
    $(wildcard include/config/arp/timeout.h) \
    $(wildcard include/config/sys/longhelp.h) \
    $(wildcard include/config/sys/hush/parser.h) \
    $(wildcard include/config/sys/prompt/hush/ps2.h) \
    $(wildcard include/config/auto/complete.h) \
    $(wildcard include/config/sys/cbsize.h) \
    $(wildcard include/config/sys/pbsize.h) \
    $(wildcard include/config/sys/prompt.h) \
    $(wildcard include/config/sys/maxargs.h) \
    $(wildcard include/config/sys/bargsize.h) \
    $(wildcard include/config/cmd/memtest.h) \
    $(wildcard include/config/sys/memtest/start.h) \
    $(wildcard include/config/sys/memtest/end.h) \
    $(wildcard include/config/sys/memtest/scratch.h) \
    $(wildcard include/config/sys/load/addr.h) \
    $(wildcard include/config/cmdline/editing.h) \
    $(wildcard include/config/stacksize.h) \
    $(wildcard include/config/nr/dram/banks.h) \
    $(wildcard include/config/sys/sdram/base.h) \
    $(wildcard include/config/sys/init/ram/addr.h) \
    $(wildcard include/config/sys/init/ram/size.h) \
    $(wildcard include/config/sys/init/sp/offset.h) \
    $(wildcard include/config/sys/init/sp/addr.h) \
    $(wildcard include/config/sys/no/flash.h) \
    $(wildcard include/config/env/size.h) \
    $(wildcard include/config/sys/boot/spinor.h) \
    $(wildcard include/config/env/is/in/spi/flash.h) \
    $(wildcard include/config/sys/boot/eimnor.h) \
    $(wildcard include/config/sys/use/eimnor.h) \
    $(wildcard include/config/env/is/in/flash.h) \
    $(wildcard include/config/sys/use/nand.h) \
    $(wildcard include/config/env/is/in/nand.h) \
    $(wildcard include/config/env/is/in/sata.h) \
    $(wildcard include/config/cmd/sata.h) \
    $(wildcard include/config/env/is/in/mmc.h) \
    $(wildcard include/config/dwc/ahsata.h) \
    $(wildcard include/config/sys/sata/max/device.h) \
    $(wildcard include/config/dwc/ahsata/port/id.h) \
    $(wildcard include/config/dwc/ahsata/base/addr.h) \
    $(wildcard include/config/lba48.h) \
    $(wildcard include/config/libata.h) \
    $(wildcard include/config/cmd/sf.h) \
    $(wildcard include/config/spi/flash.h) \
    $(wildcard include/config/spi/flash/stmicro.h) \
    $(wildcard include/config/mxc/spi.h) \
    $(wildcard include/config/sf/default/bus.h) \
    $(wildcard include/config/sf/default/speed.h) \
    $(wildcard include/config/sf/default/mode.h) \
    $(wildcard include/config/sys/flash/base.h) \
    $(wildcard include/config/sys/flash/sect/size.h) \
    $(wildcard include/config/sys/max/flash/banks.h) \
    $(wildcard include/config/sys/max/flash/sect.h) \
    $(wildcard include/config/sys/flash/cfi.h) \
    $(wildcard include/config/flash/cfi/driver.h) \
    $(wildcard include/config/sys/flash/use/buffer/write.h) \
    $(wildcard include/config/sys/flash/empty/info.h) \
    $(wildcard include/config/cmd/nand.h) \
    $(wildcard include/config/cmd/nand/trimffs.h) \
    $(wildcard include/config/nand/mxs.h) \
    $(wildcard include/config/sys/max/nand/device.h) \
    $(wildcard include/config/sys/nand/base.h) \
    $(wildcard include/config/sys/nand/5/addr/cycle.h) \
    $(wildcard include/config/sys/nand/onfi/detection.h) \
    $(wildcard include/config/apbh/dma.h) \
    $(wildcard include/config/apbh/dma/burst.h) \
    $(wildcard include/config/apbh/dma/burst8.h) \
    $(wildcard include/config/env/offset.h) \
    $(wildcard include/config/env/sect/size.h) \
    $(wildcard include/config/env/spi/bus.h) \
    $(wildcard include/config/env/spi/cs.h) \
    $(wildcard include/config/env/spi/mode.h) \
    $(wildcard include/config/env/spi/max/hz.h) \
    $(wildcard include/config/sata/env/dev.h) \
    $(wildcard include/config/sys/dcache/off.h) \
    $(wildcard include/config/cmd/cache.h) \
    $(wildcard include/config/cmd/i2c.h) \
    $(wildcard include/config/sys/i2c.h) \
    $(wildcard include/config/sys/i2c/mxc.h) \
    $(wildcard include/config/sys/i2c/speed.h) \
    $(wildcard include/config/video.h) \
    $(wildcard include/config/video/ipuv3.h) \
    $(wildcard include/config/cfb/console.h) \
    $(wildcard include/config/vga/as/single/device.h) \
    $(wildcard include/config/sys/console/overwrite/routine.h) \
    $(wildcard include/config/video/bmp/rle8.h) \
    $(wildcard include/config/bmp/16bpp.h) \
    $(wildcard include/config/video/logo.h) \
    $(wildcard include/config/video/bmp/logo.h) \
    $(wildcard include/config/ipuv3/clk.h) \
    $(wildcard include/config/imx/hdmi.h) \
    $(wildcard include/config/imx/video/skip.h) \
  include/configs/mx6_common.h \
    $(wildcard include/config/arm/errata/743622.h) \
    $(wildcard include/config/arm/errata/751472.h) \
    $(wildcard include/config/arm/errata/794072.h) \
    $(wildcard include/config/arm/errata/761320.h) \
    $(wildcard include/config/arm/errata/845369.h) \
    $(wildcard include/config/sys/bootm/len.h) \
    $(wildcard include/config/sys/l2cache/off.h) \
    $(wildcard include/config/sys/l2/pl310.h) \
    $(wildcard include/config/sys/pl310/base.h) \
    $(wildcard include/config/mp.h) \
    $(wildcard include/config/gpt/timer.h) \
    $(wildcard include/config/syscounter/timer.h) \
    $(wildcard include/config/sc/timer/clk.h) \
    $(wildcard include/config/board/postclk/init.h) \
    $(wildcard include/config/ldo/bypass/check.h) \
    $(wildcard include/config/mxc/gpt/hclk.h) \
    $(wildcard include/config/imx/thermal.h) \
  include/linux/sizes.h \
  include/config_cmd_default.h \
    $(wildcard include/config/cmd/default/h.h) \
    $(wildcard include/config/cmd/bdi.h) \
    $(wildcard include/config/cmd/bootd.h) \
    $(wildcard include/config/cmd/console.h) \
    $(wildcard include/config/cmd/echo.h) \
    $(wildcard include/config/cmd/editenv.h) \
    $(wildcard include/config/cmd/env/exists.h) \
    $(wildcard include/config/cmd/fpga.h) \
    $(wildcard include/config/cmd/imi.h) \
    $(wildcard include/config/cmd/itest.h) \
    $(wildcard include/config/cmd/flash.h) \
    $(wildcard include/config/cmd/loadb.h) \
    $(wildcard include/config/cmd/loads.h) \
    $(wildcard include/config/cmd/memory.h) \
    $(wildcard include/config/cmd/misc.h) \
    $(wildcard include/config/cmd/run.h) \
    $(wildcard include/config/cmd/saveenv.h) \
    $(wildcard include/config/cmd/setgetdcr.h) \
    $(wildcard include/config/cmd/source.h) \
    $(wildcard include/config/cmd/ximg.h) \
  include/configs/mx6sabreandroid_common.h \
    $(wildcard include/config/ci/udc.h) \
    $(wildcard include/config/usbd/hs.h) \
    $(wildcard include/config/usb/gadget/dualspeed.h) \
    $(wildcard include/config/usb/gadget.h) \
    $(wildcard include/config/cmd/usb/mass/storage.h) \
    $(wildcard include/config/usb/gadget/mass/storage.h) \
    $(wildcard include/config/usbdownload/gadget.h) \
    $(wildcard include/config/usb/gadget/vbus/draw.h) \
    $(wildcard include/config/g/dnl/vendor/num.h) \
    $(wildcard include/config/g/dnl/product/num.h) \
    $(wildcard include/config/g/dnl/manufacturer.h) \
    $(wildcard include/config/cmd/fastboot.h) \
    $(wildcard include/config/fastboot/flash.h) \
    $(wildcard include/config/fsl/fastboot.h) \
    $(wildcard include/config/android/recovery.h) \
    $(wildcard include/config/fastboot/storage/nand.h) \
    $(wildcard include/config/fastboot/storage/sata.h) \
    $(wildcard include/config/fastboot/storage/mmc.h) \
    $(wildcard include/config/android/main/mmc/bus.h) \
    $(wildcard include/config/android/boot/partition/mmc.h) \
    $(wildcard include/config/android/system/partition/mmc.h) \
    $(wildcard include/config/android/recovery/partition/mmc.h) \
    $(wildcard include/config/android/cache/partition/mmc.h) \
    $(wildcard include/config/android/data/partition/mmc.h) \
    $(wildcard include/config/cmd/boota.h) \
    $(wildcard include/config/support/raw/initrd.h) \
    $(wildcard include/config/serial/tag.h) \
    $(wildcard include/config/usb/fastboot/buf/addr.h) \
    $(wildcard include/config/usb/fastboot/buf/size.h) \
  arch/arm/include/asm/config.h \
    $(wildcard include/config/h/.h) \
    $(wildcard include/config/arm64.h) \
    $(wildcard include/config/phys/64bit.h) \
    $(wildcard include/config/static/rela.h) \
    $(wildcard include/config/fsl/lsch3.h) \
    $(wildcard include/config/ls102xa.h) \
  include/config_fallbacks.h \
    $(wildcard include/config/fallbacks/h.h) \
    $(wildcard include/config/spl/pad/to.h) \
    $(wildcard include/config/spl/max/size.h) \
    $(wildcard include/config/sys/baudrate/table.h) \
    $(wildcard include/config/fs/fat.h) \
    $(wildcard include/config/fs/ext4.h) \
    $(wildcard include/config/ext4/write.h) \
    $(wildcard include/config/cmd/ide.h) \
    $(wildcard include/config/cmd/scsi.h) \
    $(wildcard include/config/cmd/part.h) \
    $(wildcard include/config/cmd/gpt.h) \
    $(wildcard include/config/systemace.h) \
    $(wildcard include/config/sandbox.h) \
    $(wildcard include/config/partition/uuids.h) \
    $(wildcard include/config/efi/partition.h) \
    $(wildcard include/config/random/uuid.h) \
    $(wildcard include/config/cmd/uuid.h) \
    $(wildcard include/config/bootp/pxe.h) \
    $(wildcard include/config/lib/uuid.h) \
    $(wildcard include/config/lib/rand.h) \
    $(wildcard include/config/lib/hw/rand.h) \
    $(wildcard include/config/api.h) \
    $(wildcard include/config/disable/image/legacy.h) \

tools/imximage.o: $(deps_tools/imximage.o)

$(deps_tools/imximage.o):
