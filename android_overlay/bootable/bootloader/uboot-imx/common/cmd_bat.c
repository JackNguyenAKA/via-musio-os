/*
 * Copyright (C) 2010 Texas Instruments
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <asm/gpio.h>
#include <asm/io.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include <exports.h>


extern int bq27x00_init(void);
extern int read_battery(void);

void power_off(void)
{
	unsigned short *address;

	/* control snvs-rtc (base-add 0x20cc030) (SNVS_LPCR 0x4) */
	address = (unsigned short *)(0x20cc038);
	writeb(0x60, address);
}

void power_reset(void)
{
	unsigned int *address;

	address = (unsigned int *)(0x20bc034);
	writew(0x4, address);

	writew(0x5555, address + 0x16);
	writew(0xaaaa, address + 0x16);	/* load minimum 1/2 second timeout */
}

void power_button_test(void)
{
	int value = 0, tmp;
	unsigned long b_ts, a_ts;
	while (1) {
		// show battery percent
		/*
			read_battery() will report percent of battery
		*/
		gpio_direction_input(IMX_GPIO_NR(3, 29));
		value = gpio_get_value(IMX_GPIO_NR(3, 29));
		if (tmp != value) {
			if (value){
				b_ts = 0;
				//printf("release button:%d, ts:%d\n", value, a_ts);	
				tmp = value;
			}
			else {
				b_ts = get_timer(0);
				//printf("push button:%d, ts:%d\n", value, b_ts);
				tmp = value;
			}
		}
		a_ts = get_timer(0);
		if (a_ts - b_ts >= 2000 && b_ts != 0)
			break;
	}
}

int do_vbat(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	if (argc == 2) {
		if (strncmp(argv[1], "status", 7) == 0) {
			bq27x00_init();
		} else if (strncmp(argv[1], "test", 5) == 0) {
			power_button_test();
		} else if (strncmp(argv[1], "poweroff", 5) == 0) {
			power_off();
		} else if (strncmp(argv[1], "reset", 5) == 0) {
			power_reset();
		} else {
			goto bat_cmd_usage;
		}
	} else {
		goto bat_cmd_usage;
	}
	return 0;

bat_cmd_usage:
	return cmd_usage(cmdtp);
}

U_BOOT_CMD(
	bat, 2, 1, do_vbat,
	"battery charging",
	"status - display battery percent\n"
	"test - power button\n"
	"bat poweroff - system power off\n"
	"bat reset - system reset\n"
);

