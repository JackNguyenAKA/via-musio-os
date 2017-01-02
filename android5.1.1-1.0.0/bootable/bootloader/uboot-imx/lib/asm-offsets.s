	.arch armv7-a
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 4
	.eabi_attribute 18, 4
	.file	"asm-offsets.c"
@ GNU C (GCC) version 4.6.x-google 20120106 (prerelease) (arm-eabi)
@	compiled by GNU C version 4.4.3, GMP version 4.2.4, MPFR version 2.4.1, MPC version 0.8.1
@ GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
@ options passed:  -nostdinc -I include -I ./arch/arm/include
@ -iprefix /home/susan/via_Musio-OS/android5.1.1-1.0.0/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/../lib/gcc/arm-eabi/4.6.x-google/
@ -D__USES_INITFINI__ -D __KERNEL__ -D __UBOOT__
@ -D CONFIG_SYS_TEXT_BASE=0x17800000 -D __ARM__ -D DO_DEPS_ONLY
@ -D KBUILD_STR(s)=#s -D KBUILD_BASENAME=KBUILD_STR(asm_offsets)
@ -D KBUILD_MODNAME=KBUILD_STR(asm_offsets)
@ -isystem /home/susan/via_Musio-OS/android5.1.1-1.0.0/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/../lib/gcc/arm-eabi/4.6.x-google/include
@ -include ./include/linux/kconfig.h -MD lib/.asm-offsets.s.d
@ lib/asm-offsets.c -marm -mno-thumb-interwork -mabi=aapcs-linux
@ -mword-relocations -msoft-float -march=armv7-a -mfpu=vfp
@ -auxbase-strip lib/asm-offsets.s -g -Os -Wall -Wstrict-prototypes
@ -Wno-format-security -Wno-format-nonliteral -fno-builtin -ffreestanding
@ -fno-stack-protector -fstack-usage -ffunction-sections -fdata-sections
@ -fno-common -ffixed-r9 -fverbose-asm
@ options enabled:  -fauto-inc-dec -fbranch-count-reg -fcaller-saves
@ -fcombine-stack-adjustments -fcompare-elim -fcprop-registers
@ -fcrossjumping -fcse-follow-jumps -fdata-sections -fdefer-pop
@ -fdelete-null-pointer-checks -fdevirtualize -fdwarf2-cfi-asm
@ -fearly-inlining -feliminate-unused-debug-types -fexpensive-optimizations
@ -fforward-propagate -ffunction-cse -ffunction-sections -fgcse -fgcse-lm
@ -fguess-branch-probability -fident -fif-conversion -fif-conversion2
@ -findirect-inlining -finline -finline-functions
@ -finline-functions-called-once -finline-hot-caller
@ -finline-small-functions -fipa-cp -fipa-profile -fipa-pure-const
@ -fipa-reference -fipa-sra -fira-share-save-slots -fira-share-spill-slots
@ -fivopts -fkeep-static-consts -fleading-underscore -fmath-errno
@ -fmerge-constants -fmerge-debug-strings -fmove-loop-invariants
@ -fomit-frame-pointer -foptimize-register-move -foptimize-sibling-calls
@ -fpartial-inlining -fpeephole -fpeephole2 -fprefetch-loop-arrays
@ -freg-struct-return -fregmove -freorder-blocks -freorder-functions
@ -frerun-cse-after-loop -fripa-peel-size-limit -fripa-unroll-size-limit
@ -fsched-critical-path-heuristic -fsched-dep-count-heuristic
@ -fsched-group-heuristic -fsched-interblock -fsched-last-insn-heuristic
@ -fsched-rank-heuristic -fsched-spec -fsched-spec-insn-heuristic
@ -fsched-stalled-insns-dep -fschedule-insns2 -fsection-anchors
@ -fshow-column -fsigned-zeros -fsplit-ivs-in-unroller -fsplit-wide-types
@ -fstrict-aliasing -fstrict-enum-precision -fstrict-overflow
@ -fstrict-volatile-bitfields -fthread-jumps -ftoplevel-reorder
@ -ftrapping-math -ftree-bit-ccp -ftree-builtin-call-dce -ftree-ccp
@ -ftree-ch -ftree-copy-prop -ftree-copyrename -ftree-cselim -ftree-dce
@ -ftree-dominator-opts -ftree-dse -ftree-forwprop -ftree-fre
@ -ftree-loop-if-convert -ftree-loop-im -ftree-loop-ivcanon
@ -ftree-loop-optimize -ftree-parallelize-loops= -ftree-phiprop -ftree-pre
@ -ftree-pta -ftree-reassoc -ftree-scev-cprop -ftree-sink
@ -ftree-slp-vectorize -ftree-sra -ftree-switch-conversion -ftree-ter
@ -ftree-vect-loop-version -ftree-vrp -funit-at-a-time -fvar-tracking
@ -fvar-tracking-assignments -fverbose-asm -fzero-initialized-in-bss
@ -mlittle-endian -msched-prolog -mword-relocations

	.text
.Ltext0:
@ Compiler executable checksum: 7e0a3541f55243241c747ff83ef85799

	.cfi_sections	.debug_frame
	.section	.text.startup.main,"ax",%progbits
	.align	2
	.global	main
	.type	main, %function
main:
.LFB144:
	.file 1 "lib/asm-offsets.c"
	.loc 1 20 0
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	.loc 1 22 0
@ 22 "lib/asm-offsets.c" 1
	
.ascii "->GENERATED_GBL_DATA_SIZE #256 (sizeof(struct global_data) + 15) & ~15"	@
@ 0 "" 2
	.loc 1 25 0
@ 25 "lib/asm-offsets.c" 1
	
.ascii "->GENERATED_BD_INFO_SIZE #80 (sizeof(struct bd_info) + 15) & ~15"	@
@ 0 "" 2
	.loc 1 28 0
@ 28 "lib/asm-offsets.c" 1
	
.ascii "->GD_SIZE #248 sizeof(struct global_data)"	@
@ 0 "" 2
	.loc 1 30 0
@ 30 "lib/asm-offsets.c" 1
	
.ascii "->GD_BD #0 offsetof(struct global_data, bd)"	@
@ 0 "" 2
	.loc 1 32 0
@ 32 "lib/asm-offsets.c" 1
	
.ascii "->GD_MALLOC_BASE #192 offsetof(struct global_data, malloc_base)"	@
@ 0 "" 2
	.loc 1 35 0
@ 35 "lib/asm-offsets.c" 1
	
.ascii "->GD_RELOCADDR #48 offsetof(struct global_data, relocaddr)"	@
@ 0 "" 2
	.loc 1 37 0
@ 37 "lib/asm-offsets.c" 1
	
.ascii "->GD_RELOC_OFF #68 offsetof(struct global_data, reloc_off)"	@
@ 0 "" 2
	.loc 1 39 0
@ 39 "lib/asm-offsets.c" 1
	
.ascii "->GD_START_ADDR_SP #64 offsetof(struct global_data, start_addr_sp)"	@
@ 0 "" 2
	.loc 1 42 0
	mov	r0, #0	@,
	bx	lr	@
	.cfi_endproc
.LFE144:
	.size	main, .-main
	.text
.Letext0:
	.file 2 "./arch/arm/include/asm/types.h"
	.file 3 "include/linux/types.h"
	.file 4 "include/asm-generic/u-boot.h"
	.file 5 "include/net.h"
	.section	.debug_info,"",%progbits
.Ldebug_info0:
	.4byte	0x392
	.2byte	0x2
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.4byte	.LASF55
	.byte	0x1
	.4byte	.LASF56
	.4byte	.LASF57
	.4byte	0
	.4byte	0
	.4byte	.Ldebug_ranges0+0
	.4byte	.Ldebug_line0
	.4byte	.Ldebug_pubnames0
	.4byte	.Ldebug_pubtypes0
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.4byte	.LASF0
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF1
	.uleb128 0x2
	.byte	0x2
	.byte	0x7
	.4byte	.LASF2
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.4byte	.LASF3
	.uleb128 0x2
	.byte	0x2
	.byte	0x5
	.4byte	.LASF4
	.uleb128 0x3
	.byte	0x4
	.byte	0x5
	.ascii	"int\000"
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF5
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.4byte	.LASF6
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.4byte	.LASF7
	.uleb128 0x4
	.4byte	.LASF8
	.byte	0x2
	.byte	0x34
	.4byte	0x38
	.uleb128 0x4
	.4byte	.LASF9
	.byte	0x2
	.byte	0x35
	.4byte	0x38
	.uleb128 0x2
	.byte	0x4
	.byte	0x5
	.4byte	.LASF10
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.4byte	.LASF11
	.uleb128 0x4
	.4byte	.LASF12
	.byte	0x3
	.byte	0x59
	.4byte	0x38
	.uleb128 0x5
	.byte	0x4
	.uleb128 0x6
	.byte	0x8
	.byte	0x4
	.byte	0x84
	.4byte	0xc6
	.uleb128 0x7
	.4byte	.LASF13
	.byte	0x4
	.byte	0x85
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x7
	.4byte	.LASF14
	.byte	0x4
	.byte	0x86
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.byte	0
	.uleb128 0x8
	.4byte	.LASF35
	.byte	0x50
	.byte	0x4
	.byte	0x1b
	.4byte	0x1dd
	.uleb128 0x7
	.4byte	.LASF15
	.byte	0x4
	.byte	0x1c
	.4byte	0x38
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x7
	.4byte	.LASF16
	.byte	0x4
	.byte	0x1d
	.4byte	0x7b
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x7
	.4byte	.LASF17
	.byte	0x4
	.byte	0x1e
	.4byte	0x38
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x7
	.4byte	.LASF18
	.byte	0x4
	.byte	0x1f
	.4byte	0x38
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0x7
	.4byte	.LASF19
	.byte	0x4
	.byte	0x20
	.4byte	0x38
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0x7
	.4byte	.LASF20
	.byte	0x4
	.byte	0x21
	.4byte	0x38
	.byte	0x2
	.byte	0x23
	.uleb128 0x14
	.uleb128 0x7
	.4byte	.LASF21
	.byte	0x4
	.byte	0x22
	.4byte	0x38
	.byte	0x2
	.byte	0x23
	.uleb128 0x18
	.uleb128 0x7
	.4byte	.LASF22
	.byte	0x4
	.byte	0x28
	.4byte	0x38
	.byte	0x2
	.byte	0x23
	.uleb128 0x1c
	.uleb128 0x7
	.4byte	.LASF23
	.byte	0x4
	.byte	0x29
	.4byte	0x38
	.byte	0x2
	.byte	0x23
	.uleb128 0x20
	.uleb128 0x7
	.4byte	.LASF24
	.byte	0x4
	.byte	0x2a
	.4byte	0x38
	.byte	0x2
	.byte	0x23
	.uleb128 0x24
	.uleb128 0x7
	.4byte	.LASF25
	.byte	0x4
	.byte	0x36
	.4byte	0x38
	.byte	0x2
	.byte	0x23
	.uleb128 0x28
	.uleb128 0x7
	.4byte	.LASF26
	.byte	0x4
	.byte	0x37
	.4byte	0x38
	.byte	0x2
	.byte	0x23
	.uleb128 0x2c
	.uleb128 0x7
	.4byte	.LASF27
	.byte	0x4
	.byte	0x38
	.4byte	0x1dd
	.byte	0x2
	.byte	0x23
	.uleb128 0x30
	.uleb128 0x7
	.4byte	.LASF28
	.byte	0x4
	.byte	0x39
	.4byte	0x3f
	.byte	0x2
	.byte	0x23
	.uleb128 0x36
	.uleb128 0x7
	.4byte	.LASF29
	.byte	0x4
	.byte	0x3a
	.4byte	0x38
	.byte	0x2
	.byte	0x23
	.uleb128 0x38
	.uleb128 0x7
	.4byte	.LASF30
	.byte	0x4
	.byte	0x3b
	.4byte	0x38
	.byte	0x2
	.byte	0x23
	.uleb128 0x3c
	.uleb128 0x7
	.4byte	.LASF31
	.byte	0x4
	.byte	0x81
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x40
	.uleb128 0x7
	.4byte	.LASF32
	.byte	0x4
	.byte	0x82
	.4byte	0x94
	.byte	0x2
	.byte	0x23
	.uleb128 0x44
	.uleb128 0x7
	.4byte	.LASF33
	.byte	0x4
	.byte	0x87
	.4byte	0x1ed
	.byte	0x2
	.byte	0x23
	.uleb128 0x48
	.byte	0
	.uleb128 0x9
	.4byte	0x31
	.4byte	0x1ed
	.uleb128 0xa
	.4byte	0x5b
	.byte	0x5
	.byte	0
	.uleb128 0x9
	.4byte	0xa1
	.4byte	0x1fd
	.uleb128 0xa
	.4byte	0x5b
	.byte	0
	.byte	0
	.uleb128 0x4
	.4byte	.LASF34
	.byte	0x4
	.byte	0x89
	.4byte	0xc6
	.uleb128 0xb
	.byte	0x4
	.4byte	0x1fd
	.uleb128 0x8
	.4byte	.LASF36
	.byte	0x40
	.byte	0x5
	.byte	0x51
	.4byte	0x2c3
	.uleb128 0x7
	.4byte	.LASF37
	.byte	0x5
	.byte	0x52
	.4byte	0x2c3
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x7
	.4byte	.LASF38
	.byte	0x5
	.byte	0x53
	.4byte	0x1dd
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0x7
	.4byte	.LASF39
	.byte	0x5
	.byte	0x54
	.4byte	0x70
	.byte	0x2
	.byte	0x23
	.uleb128 0x18
	.uleb128 0x7
	.4byte	.LASF40
	.byte	0x5
	.byte	0x55
	.4byte	0x54
	.byte	0x2
	.byte	0x23
	.uleb128 0x1c
	.uleb128 0x7
	.4byte	.LASF41
	.byte	0x5
	.byte	0x57
	.4byte	0x2ee
	.byte	0x2
	.byte	0x23
	.uleb128 0x20
	.uleb128 0x7
	.4byte	.LASF42
	.byte	0x5
	.byte	0x58
	.4byte	0x30e
	.byte	0x2
	.byte	0x23
	.uleb128 0x24
	.uleb128 0x7
	.4byte	.LASF43
	.byte	0x5
	.byte	0x59
	.4byte	0x324
	.byte	0x2
	.byte	0x23
	.uleb128 0x28
	.uleb128 0x7
	.4byte	.LASF44
	.byte	0x5
	.byte	0x5a
	.4byte	0x336
	.byte	0x2
	.byte	0x23
	.uleb128 0x2c
	.uleb128 0x7
	.4byte	.LASF45
	.byte	0x5
	.byte	0x5e
	.4byte	0x324
	.byte	0x2
	.byte	0x23
	.uleb128 0x30
	.uleb128 0x7
	.4byte	.LASF46
	.byte	0x5
	.byte	0x5f
	.4byte	0x2e8
	.byte	0x2
	.byte	0x23
	.uleb128 0x34
	.uleb128 0x7
	.4byte	.LASF47
	.byte	0x5
	.byte	0x60
	.4byte	0x54
	.byte	0x2
	.byte	0x23
	.uleb128 0x38
	.uleb128 0x7
	.4byte	.LASF48
	.byte	0x5
	.byte	0x61
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x3c
	.byte	0
	.uleb128 0x9
	.4byte	0x8d
	.4byte	0x2d3
	.uleb128 0xa
	.4byte	0x5b
	.byte	0xf
	.byte	0
	.uleb128 0xc
	.byte	0x1
	.4byte	0x54
	.4byte	0x2e8
	.uleb128 0xd
	.4byte	0x2e8
	.uleb128 0xd
	.4byte	0x208
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.4byte	0x20e
	.uleb128 0xb
	.byte	0x4
	.4byte	0x2d3
	.uleb128 0xc
	.byte	0x1
	.4byte	0x54
	.4byte	0x30e
	.uleb128 0xd
	.4byte	0x2e8
	.uleb128 0xd
	.4byte	0x9f
	.uleb128 0xd
	.4byte	0x54
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.4byte	0x2f4
	.uleb128 0xc
	.byte	0x1
	.4byte	0x54
	.4byte	0x324
	.uleb128 0xd
	.4byte	0x2e8
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.4byte	0x314
	.uleb128 0xe
	.byte	0x1
	.4byte	0x336
	.uleb128 0xd
	.4byte	0x2e8
	.byte	0
	.uleb128 0xb
	.byte	0x4
	.4byte	0x32a
	.uleb128 0xf
	.4byte	.LASF58
	.byte	0x4
	.byte	0x5
	.2byte	0x20e
	.4byte	0x362
	.uleb128 0x10
	.4byte	.LASF49
	.sleb128 0
	.uleb128 0x10
	.4byte	.LASF50
	.sleb128 1
	.uleb128 0x10
	.4byte	.LASF51
	.sleb128 2
	.uleb128 0x10
	.4byte	.LASF52
	.sleb128 3
	.byte	0
	.uleb128 0x11
	.byte	0x1
	.4byte	.LASF59
	.byte	0x1
	.byte	0x13
	.byte	0x1
	.4byte	0x54
	.4byte	.LFB144
	.4byte	.LFE144
	.byte	0x2
	.byte	0x7d
	.sleb128 0
	.uleb128 0x12
	.4byte	.LASF53
	.byte	0x5
	.byte	0x6b
	.4byte	0x2e8
	.byte	0x1
	.byte	0x1
	.uleb128 0x13
	.4byte	.LASF54
	.byte	0x5
	.2byte	0x214
	.4byte	0x33c
	.byte	0x1
	.byte	0x1
	.byte	0
	.section	.debug_abbrev,"",%progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x52
	.uleb128 0x1
	.uleb128 0x55
	.uleb128 0x6
	.uleb128 0x10
	.uleb128 0x6
	.uleb128 0x2134
	.uleb128 0x6
	.uleb128 0x2135
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0x15
	.byte	0x1
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x5
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x15
	.byte	0x1
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0x4
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x10
	.uleb128 0x28
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1c
	.uleb128 0xd
	.byte	0
	.byte	0
	.uleb128 0x11
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x12
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0
	.byte	0
	.uleb128 0x13
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_pubnames,"",%progbits
.Ldebug_pubnames0:
	.4byte	0x17
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0x396
	.4byte	0x362
	.ascii	"main\000"
	.4byte	0
	.section	.debug_pubtypes,"",%progbits
.Ldebug_pubtypes0:
	.4byte	0x176
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0x396
	.4byte	0x31
	.ascii	"unsigned char\000"
	.4byte	0x38
	.ascii	"long unsigned int\000"
	.4byte	0x3f
	.ascii	"short unsigned int\000"
	.4byte	0x46
	.ascii	"signed char\000"
	.4byte	0x4d
	.ascii	"short int\000"
	.4byte	0x54
	.ascii	"int\000"
	.4byte	0x5b
	.ascii	"unsigned int\000"
	.4byte	0x62
	.ascii	"long long int\000"
	.4byte	0x69
	.ascii	"long long unsigned int\000"
	.4byte	0x70
	.ascii	"phys_addr_t\000"
	.4byte	0x7b
	.ascii	"phys_size_t\000"
	.4byte	0x86
	.ascii	"long int\000"
	.4byte	0x8d
	.ascii	"char\000"
	.4byte	0x94
	.ascii	"ulong\000"
	.4byte	0xc6
	.ascii	"bd_info\000"
	.4byte	0x1fd
	.ascii	"bd_t\000"
	.4byte	0x20e
	.ascii	"eth_device\000"
	.4byte	0x33c
	.ascii	"net_loop_state\000"
	.4byte	0x349
	.ascii	"NETLOOP_CONTINUE\000"
	.4byte	0x34f
	.ascii	"NETLOOP_RESTART\000"
	.4byte	0x355
	.ascii	"NETLOOP_SUCCESS\000"
	.4byte	0x35b
	.ascii	"NETLOOP_FAIL\000"
	.4byte	0
	.section	.debug_aranges,"",%progbits
	.4byte	0x1c
	.2byte	0x2
	.4byte	.Ldebug_info0
	.byte	0x4
	.byte	0
	.2byte	0
	.2byte	0
	.4byte	.LFB144
	.4byte	.LFE144-.LFB144
	.4byte	0
	.4byte	0
	.section	.debug_ranges,"",%progbits
.Ldebug_ranges0:
	.4byte	.LFB144
	.4byte	.LFE144
	.4byte	0
	.4byte	0
	.section	.debug_line,"",%progbits
.Ldebug_line0:
	.section	.debug_str,"MS",%progbits,1
.LASF53:
	.ascii	"eth_current\000"
.LASF22:
	.ascii	"bi_arm_freq\000"
.LASF32:
	.ascii	"bi_boot_params\000"
.LASF27:
	.ascii	"bi_enetaddr\000"
.LASF44:
	.ascii	"halt\000"
.LASF40:
	.ascii	"state\000"
.LASF48:
	.ascii	"priv\000"
.LASF31:
	.ascii	"bi_arch_number\000"
.LASF19:
	.ascii	"bi_flashoffset\000"
.LASF13:
	.ascii	"start\000"
.LASF41:
	.ascii	"init\000"
.LASF37:
	.ascii	"name\000"
.LASF25:
	.ascii	"bi_bootflags\000"
.LASF29:
	.ascii	"bi_intfreq\000"
.LASF54:
	.ascii	"net_state\000"
.LASF28:
	.ascii	"bi_ethspeed\000"
.LASF26:
	.ascii	"bi_ip_addr\000"
.LASF38:
	.ascii	"enetaddr\000"
.LASF39:
	.ascii	"iobase\000"
.LASF34:
	.ascii	"bd_t\000"
.LASF0:
	.ascii	"unsigned char\000"
.LASF17:
	.ascii	"bi_flashstart\000"
.LASF1:
	.ascii	"long unsigned int\000"
.LASF2:
	.ascii	"short unsigned int\000"
.LASF18:
	.ascii	"bi_flashsize\000"
.LASF23:
	.ascii	"bi_dsp_freq\000"
.LASF21:
	.ascii	"bi_sramsize\000"
.LASF15:
	.ascii	"bi_memstart\000"
.LASF57:
	.ascii	"/home/susan/via_Musio-OS/android5.1.1-1.0.0/bootabl"
	.ascii	"e/bootloader/uboot-imx\000"
.LASF59:
	.ascii	"main\000"
.LASF51:
	.ascii	"NETLOOP_SUCCESS\000"
.LASF33:
	.ascii	"bi_dram\000"
.LASF12:
	.ascii	"ulong\000"
.LASF5:
	.ascii	"unsigned int\000"
.LASF55:
	.ascii	"GNU C 4.6.x-google 20120106 (prerelease)\000"
.LASF42:
	.ascii	"send\000"
.LASF20:
	.ascii	"bi_sramstart\000"
.LASF11:
	.ascii	"char\000"
.LASF16:
	.ascii	"bi_memsize\000"
.LASF30:
	.ascii	"bi_busfreq\000"
.LASF8:
	.ascii	"phys_addr_t\000"
.LASF9:
	.ascii	"phys_size_t\000"
.LASF43:
	.ascii	"recv\000"
.LASF6:
	.ascii	"long long int\000"
.LASF50:
	.ascii	"NETLOOP_RESTART\000"
.LASF47:
	.ascii	"index\000"
.LASF35:
	.ascii	"bd_info\000"
.LASF4:
	.ascii	"short int\000"
.LASF24:
	.ascii	"bi_ddr_freq\000"
.LASF36:
	.ascii	"eth_device\000"
.LASF49:
	.ascii	"NETLOOP_CONTINUE\000"
.LASF10:
	.ascii	"long int\000"
.LASF58:
	.ascii	"net_loop_state\000"
.LASF3:
	.ascii	"signed char\000"
.LASF45:
	.ascii	"write_hwaddr\000"
.LASF46:
	.ascii	"next\000"
.LASF14:
	.ascii	"size\000"
.LASF52:
	.ascii	"NETLOOP_FAIL\000"
.LASF7:
	.ascii	"long long unsigned int\000"
.LASF56:
	.ascii	"lib/asm-offsets.c\000"
	.ident	"GCC: (GNU) 4.6.x-google 20120106 (prerelease)"
	.section	.note.GNU-stack,"",%progbits
