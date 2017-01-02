cmd_arch/arm/lib/sections.o := /home/susan/via_Musio-OS/android5.1.1-1.0.0/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-gcc -Wp,-MD,arch/arm/lib/.sections.o.d  -nostdinc -isystem /home/susan/via_Musio-OS/android5.1.1-1.0.0/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/../lib/gcc/arm-eabi/4.6.x-google/include -Iinclude  -I./arch/arm/include -include ./include/linux/kconfig.h -D__KERNEL__ -D__UBOOT__ -DCONFIG_SYS_TEXT_BASE=0x17800000 -Wall -Wstrict-prototypes -Wno-format-security -fno-builtin -ffreestanding -Os -fno-stack-protector -g -fstack-usage -Wno-format-nonliteral -D__ARM__ -marm -mno-thumb-interwork -mabi=aapcs-linux -mword-relocations -ffunction-sections -fdata-sections -fno-common -ffixed-r9 -msoft-float -pipe -march=armv7-a    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(sections)"  -D"KBUILD_MODNAME=KBUILD_STR(sections)" -c -o arch/arm/lib/sections.o arch/arm/lib/sections.c

source_arch/arm/lib/sections.o := arch/arm/lib/sections.c

deps_arch/arm/lib/sections.o := \

arch/arm/lib/sections.o: $(deps_arch/arm/lib/sections.o)

$(deps_arch/arm/lib/sections.o):
