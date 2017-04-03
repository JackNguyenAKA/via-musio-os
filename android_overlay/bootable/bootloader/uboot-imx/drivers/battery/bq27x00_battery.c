/*
 * BQ27x00 battery driver
 *
 * Copyright (C) 2008 Rodolfo Giometti <giometti@linux.it>
 * Copyright (C) 2008 Eurotech S.p.A. <info@eurotech.it>
 * Copyright (C) 2010-2011 Lars-Peter Clausen <lars@metafoo.de>
 * Copyright (C) 2011 Pali Rohár <pali.rohar@gmail.com>
 *
 * Based on a previous work by Copyright (C) 2008 Texas Instruments, Inc.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/*
 * Datasheets:
 * http://focus.ti.com/docs/prod/folders/print/bq27000.html
 * http://focus.ti.com/docs/prod/folders/print/bq27500.html
 * http://www.ti.com/product/bq27425-g1
 * http://www.ti.com/product/BQ27742-G1
 */
	 
#include <common.h>
#include <i2c.h>

int read_battery(void)
{
	struct i2c_adapter *bat_i2c;
	unsigned int result = 0;
	char percent[2];

	bat_i2c = i2c_get_adapter(1);

	result = bat_i2c->read(bat_i2c, 0x55, 0x2c, 1, percent, 2);
	if (result) {
		printf("fail to read device by i2c protocol\n");
		return -1;
	}

	return percent[1]<<8 | percent[0];
}

void bq27x00_init(void)
{
	int percent;

	percent = read_battery();

	if (percent > 0)
		printf("Battery:%d %\n", percent);

}
 

