OUTDIR = out
TMPDIR = tmp
KERNEL ?= kernel/kernel
DT     ?= kernel/dt.img
PKGDIR = packages

all:
	mkdir -p src/etc/udhcpc
	ln -sf udhcpc/resolv.conf src/etc/resolv.conf
	ln -sf /tmp/resolv.conf   src/etc/udhcpc/resolv.conf
	ln -sf /tmp/fb.modes      src/etc/fb.modes
	ln -sf /proc/mounts       src/etc/mtab
	mkdir -p $(TMPDIR)/bspinst_dir
	for i in `find $(PKGDIR) -type f | sort`; do \
		tar zxf $$i -C $(TMPDIR)/bspinst_dir; \
	done
	cp -af src/* $(TMPDIR)/bspinst_dir
	find $(TMPDIR)/bspinst_dir/usr/bin \
	$(TMPDIR)/bspinst_dir/bin \
	$(TMPDIR)/bspinst_dir/etc/rc.d \
	$(TMPDIR)/bspinst_dir/etc/udev/scripts \
	-type f -exec chmod +x {} \;
	chmod -R 777 $(TMPDIR)/bspinst_dir
	mkdir -p $(OUTDIR)/bspinst
	make initrd
	make bootimg
	make scriptcmd
	mv $(TMPDIR)/boot.img $(OUTDIR)/bspinst/bspinst.img

initrd:
	$(shell cd $(TMPDIR)/bspinst_dir; find . | cpio --quiet -o -H newc | gzip -9 > ../initrd.cpio.gz)

bootimg:
	chmod +x ./tools/mkbootimg
	./tools/mkbootimg \
	--second $(DT) \
	--kernel $(KERNEL) \
	--ramdisk $(TMPDIR)/initrd.cpio.gz \
	--base           0x14000000 \
	--ramdisk_offset 0x01000000 \
	--tags_offset    0x00000100 \
	--cmdline        'console=ttymxc1,115200 video=mxcfb0:dev=lcd,HUAQIN-TXDT550,if=RGB24,bpp=32 video=mxcfb1:off video=mxcfb2:off video=mxcfb3:off fbmem=28M vmalloc=400M androidboot.console=ttymxc1 consoleblank=0 androidboot.hardware=freescale audio=hdmi androidboot.selinux=disabled root=/dev/ram0 rwi fbcon=map:4' \
	--pagesize 2048 \
	-o $(TMPDIR)/boot.img

scriptcmd:
	sh ./src/usr/bin/mkscriptcmd.sh scriptcmd.txt $(OUTDIR)/scriptcmd

clean:
	find src -type l -exec rm -vf {} \;
	rm -rf $(OUTDIR) $(TMPDIR)

install:
	fastboot -S0 flash boot  out/bspinst/bspinst.img
	fastboot -S0 flash boot2 out/bspinst/bspinst.img
	fastboot reboot
