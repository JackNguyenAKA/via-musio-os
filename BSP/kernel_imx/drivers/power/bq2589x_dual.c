/*
 * BQ2589x battery charging driver
 *
 * Copyright (C) 2013 Texas Instruments
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/power_supply.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/of_gpio.h>
#include "bq2589x_reg.h"

enum bq2589x_vbus_type {
	BQ2589X_VBUS_NONE,
	BQ2589X_VBUS_USB_SDP,
	BQ2589X_VBUS_USB_CDP,
	BQ2589X_VBUS_USB_DCP,
	BQ2589X_VBUS_MAXC,
	BQ2589X_VBUS_UNKNOWN,
	BQ2589X_VBUS_NONSTAND,
	BQ2589X_VBUS_OTG,
	BQ2589X_VBUS_TYPE_NUM,
};

enum bq2589x_part_no {
	BQ25890 = 0x03,
	BQ25892 = 0x00,
	BQ25895 = 0x07,
};

int otg_flag = 0;

#define BQ2589X_STATUS_PLUGIN			0x0001
#define BQ2589X_STATUS_PG				0x0002
#define BQ2589X_STATUS_FAULT			0x0008

#define IMX_GPIO_NR(bank, nr)			(((bank) - 1) * 32 + (nr))
#define IRQ_GPIO_NUM					IMX_GPIO_NR(4, 11)

#define BQ2589X_STATUS_EXIST			0x0100
#define BQ2589X_STATUS_CHARGE_ENABLE 	0x0200

struct bq2589x_config {
	bool	enable_auto_dpdm;
/*	bool	enable_12v;*/

	int		charge_voltage;
	int		charge_current;

	bool	enable_term;
	int		term_current;

	bool 	enable_ico;
	bool	enable_absolute_vindpm;
};


struct bq2589x {
	struct device *dev;
	struct i2c_client *client;

	enum   bq2589x_part_no part_no;
	int    revision;

	unsigned int    status;
	int		vbus_type;

	bool	enabled;

	bool    interrupt;


	int     vbus_volt;
	int     vbat_volt;

	int     rsoc;
	struct	bq2589x_config	cfg;
	struct work_struct irq_work;
	struct work_struct adapter_in_work;
	struct work_struct adapter_out_work;
	struct delayed_work monitor_work;
	struct delayed_work ico_work;
	struct delayed_work pe_volt_tune_work;
	struct delayed_work check_pe_tuneup_work;
	struct delayed_work charger2_enable_work;

	struct power_supply usb;
	struct power_supply wall;
	struct power_supply *batt_psy;
};

struct pe_ctrl {
	bool enable;
	bool tune_up_volt;
	bool tune_down_volt;
	bool tune_done;
	bool tune_fail;
	int  tune_count;
	int  target_volt;
	int	 high_volt_level;/* vbus volt > this threshold means tune up successfully */
	int  low_volt_level; /* vbus volt < this threshold means tune down successfully */
	int  vbat_min_volt;  /* to tune up voltage only when vbat > this threshold */
};

static struct bq2589x *g_bq1;
static struct pe_ctrl pe;


static DEFINE_MUTEX(bq2589x_i2c_lock);

static int bq2589x_read_byte(struct bq2589x *bq, u8 *data, u8 reg)
{
	int ret;

	mutex_lock(&bq2589x_i2c_lock);
	ret = i2c_smbus_read_byte_data(bq->client, reg);
	if (ret < 0) {
		dev_err(bq->dev, "failed to read 0x%.2x\n", reg);
		mutex_unlock(&bq2589x_i2c_lock);
		return ret;
	}

	*data = (u8)ret;
	mutex_unlock(&bq2589x_i2c_lock);

	return 0;
}

static int bq2589x_write_byte(struct bq2589x *bq, u8 reg, u8 data)
{
	int ret;
	mutex_lock(&bq2589x_i2c_lock);
	ret = i2c_smbus_write_byte_data(bq->client, reg, data);
	mutex_unlock(&bq2589x_i2c_lock);
	return ret;
}

static int bq2589x_update_bits(struct bq2589x *bq, u8 reg, u8 mask, u8 data)
{
	int ret;
	u8 tmp;

	ret = bq2589x_read_byte(bq, &tmp, reg);

	if (ret)
		return ret;

	tmp &= ~mask;
	tmp |= data & mask;

	return bq2589x_write_byte(bq, reg, tmp);
}


static enum bq2589x_vbus_type bq2589x_get_vbus_type(struct bq2589x *bq)
{
	u8 val = 0;
	int ret;
	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_0B);
	if (ret < 0)
		return 0;
	val &= BQ2589X_VBUS_STAT_MASK;
	val >>= BQ2589X_VBUS_STAT_SHIFT;

	return val;
}


static int bq2589x_enable_otg(struct bq2589x *bq)
{
	u8 val = BQ2589X_OTG_ENABLE << BQ2589X_OTG_CONFIG_SHIFT;

	return bq2589x_update_bits(bq, BQ2589X_REG_03, BQ2589X_OTG_CONFIG_MASK, val);

}

static int bq2589x_disable_otg(struct bq2589x *bq)
{
	u8 val = BQ2589X_OTG_DISABLE << BQ2589X_OTG_CONFIG_SHIFT;

	return bq2589x_update_bits(bq, BQ2589X_REG_03, BQ2589X_OTG_CONFIG_MASK, val);

}
EXPORT_SYMBOL_GPL(bq2589x_disable_otg);

static int bq2589x_set_otg_volt(struct bq2589x *bq, int volt)
{
	u8 val = 0;

	if (volt < BQ2589X_BOOSTV_BASE)
		volt = BQ2589X_BOOSTV_BASE;
	if (volt > BQ2589X_BOOSTV_BASE + (BQ2589X_BOOSTV_MASK >> BQ2589X_BOOSTV_SHIFT) * BQ2589X_BOOSTV_LSB)
		volt = BQ2589X_BOOSTV_BASE + (BQ2589X_BOOSTV_MASK >> BQ2589X_BOOSTV_SHIFT) * BQ2589X_BOOSTV_LSB;


	val = ((volt - BQ2589X_BOOSTV_BASE) / BQ2589X_BOOSTV_LSB) << BQ2589X_BOOSTV_SHIFT;

	return bq2589x_update_bits(bq, BQ2589X_REG_0A, BQ2589X_BOOSTV_MASK, val);

}
EXPORT_SYMBOL_GPL(bq2589x_set_otg_volt);

static int bq2589x_set_otg_current(struct bq2589x *bq, int curr)
{
	u8 temp;

	if (curr == 500)
		temp = BQ2589X_BOOST_LIM_500MA;
	else if (curr == 700)
		temp = BQ2589X_BOOST_LIM_700MA;
	else if (curr == 1100)
		temp = BQ2589X_BOOST_LIM_1100MA;
	else if (curr == 1600)
		temp = BQ2589X_BOOST_LIM_1600MA;
	else if (curr == 1800)
		temp = BQ2589X_BOOST_LIM_1800MA;
	else if (curr == 2100)
		temp = BQ2589X_BOOST_LIM_2100MA;
	else if (curr == 2400)
		temp = BQ2589X_BOOST_LIM_2400MA;
	else
		temp = BQ2589X_BOOST_LIM_1300MA;

	return bq2589x_update_bits(bq, BQ2589X_REG_0A, BQ2589X_BOOST_LIM_MASK, temp << BQ2589X_BOOST_LIM_SHIFT);
}
EXPORT_SYMBOL_GPL(bq2589x_set_otg_current);

static int bq2589x_enable_charger(struct bq2589x *bq)
{
	int ret;
	u8 val = BQ2589X_CHG_ENABLE << BQ2589X_CHG_CONFIG_SHIFT;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_03, BQ2589X_CHG_CONFIG_MASK, val);
	if (ret == 0)
		bq->status |= BQ2589X_STATUS_CHARGE_ENABLE;
	return ret;
}

static int bq2589x_disable_charger(struct bq2589x *bq)
{
	int ret;
	u8 val = BQ2589X_CHG_DISABLE << BQ2589X_CHG_CONFIG_SHIFT;
	if (g_bq1 == NULL)
		return -1;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_03, BQ2589X_CHG_CONFIG_MASK, val);
	if (ret == 0)
		bq->status &= ~BQ2589X_STATUS_CHARGE_ENABLE;
	return ret;
}


int bq2589x_enable_charger_gol(void)
{
	int ret;
	u8 val = BQ2589X_CHG_ENABLE << BQ2589X_CHG_CONFIG_SHIFT;
	if (g_bq1 == NULL)
		return -1;
	
	ret = bq2589x_update_bits(g_bq1, BQ2589X_REG_03, BQ2589X_CHG_CONFIG_MASK, val);
	if (ret == 0)
		g_bq1->status |= BQ2589X_STATUS_CHARGE_ENABLE;
	return ret;
}
EXPORT_SYMBOL_GPL(bq2589x_enable_charger_gol);

int bq2589x_disable_charger_gol(void)

{
	int ret;
	u8 val = BQ2589X_CHG_DISABLE << BQ2589X_CHG_CONFIG_SHIFT;

	ret = bq2589x_update_bits(g_bq1, BQ2589X_REG_03, BQ2589X_CHG_CONFIG_MASK, val);
	if (ret == 0)
		g_bq1->status &= ~BQ2589X_STATUS_CHARGE_ENABLE;
	return ret;
}
EXPORT_SYMBOL_GPL(bq2589x_disable_charger_gol);

/* interfaces that can be called by other module */
int bq2589x_adc_start(struct bq2589x *bq, bool oneshot)
{
	u8 val;
	int ret;

	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_02);
	if (ret < 0) {
		dev_err(bq->dev, "%s failed to read register 0x02:%d\n", __func__, ret);
		return ret;
	}

	if (((val & BQ2589X_CONV_RATE_MASK) >> BQ2589X_CONV_RATE_SHIFT) == BQ2589X_ADC_CONTINUE_ENABLE)
		return 0; /*is doing continuous scan*/
	if (oneshot)
		ret = bq2589x_update_bits(bq, BQ2589X_REG_02, BQ2589X_CONV_START_MASK, BQ2589X_CONV_START << BQ2589X_CONV_START_SHIFT);
	else
		ret = bq2589x_update_bits(bq, BQ2589X_REG_02, BQ2589X_CONV_RATE_MASK, BQ2589X_ADC_CONTINUE_ENABLE << BQ2589X_CONV_RATE_SHIFT);
	return ret;
}
EXPORT_SYMBOL_GPL(bq2589x_adc_start);

int bq2589x_adc_stop(struct bq2589x *bq)
{
	return bq2589x_update_bits(bq, BQ2589X_REG_02, BQ2589X_CONV_RATE_MASK, BQ2589X_ADC_CONTINUE_DISABLE << BQ2589X_CONV_RATE_SHIFT);
}
EXPORT_SYMBOL_GPL(bq2589x_adc_stop);


int bq2589x_adc_read_battery_volt(struct bq2589x *bq)
{
	uint8_t val;
	int volt;
	int ret;

	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_0E);
	if (ret < 0) {
		dev_err(bq->dev, "read battery voltage failed :%d\n", ret);
		return ret;
	} else {
		volt = BQ2589X_BATV_BASE + ((val & BQ2589X_BATV_MASK) >> BQ2589X_BATV_SHIFT) * BQ2589X_BATV_LSB ;
		return volt;
	}
}
EXPORT_SYMBOL_GPL(bq2589x_adc_read_battery_volt);


int bq2589x_adc_read_sys_volt(struct bq2589x *bq)
{
	uint8_t val;
	int volt;
	int ret;

	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_0F);
	if (ret < 0) {
		dev_err(bq->dev, "read system voltage failed :%d\n", ret);
		return ret;
	} else {
		volt = BQ2589X_SYSV_BASE + ((val & BQ2589X_SYSV_MASK) >> BQ2589X_SYSV_SHIFT) * BQ2589X_SYSV_LSB ;
		return volt;
	}
}
EXPORT_SYMBOL_GPL(bq2589x_adc_read_sys_volt);

int bq2589x_adc_read_vbus_volt(struct bq2589x *bq)
{
	uint8_t val;
	int volt;
	int ret;

	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_11);
	if (ret < 0) {
		dev_err(bq->dev, "read vbus voltage failed :%d\n", ret);
		return ret;
	} else {
		volt = BQ2589X_VBUSV_BASE + ((val & BQ2589X_VBUSV_MASK) >> BQ2589X_VBUSV_SHIFT) * BQ2589X_VBUSV_LSB ;
		return volt;
	}
}
EXPORT_SYMBOL_GPL(bq2589x_adc_read_vbus_volt);

int bq2589x_adc_read_temperature(struct bq2589x *bq)
{
	uint8_t val;
	int temp;
	int ret;

	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_10);
	if (ret < 0) {
		dev_err(bq->dev, "read temperature failed :%d\n", ret);
		return ret;
	} else {
		temp = BQ2589X_TSPCT_BASE + ((val & BQ2589X_TSPCT_MASK) >> BQ2589X_TSPCT_SHIFT) * BQ2589X_TSPCT_LSB ;
		return temp;
	}
}
EXPORT_SYMBOL_GPL(bq2589x_adc_read_temperature);

int bq2589x_adc_read_charge_current(struct bq2589x *bq)
{
	uint8_t val;
	int volt;
	int ret;

	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_12);
	if (ret < 0) {
		dev_err(bq->dev, "read charge current failed :%d\n", ret);
		return ret;
	} else {
		volt = (int)(BQ2589X_ICHGR_BASE + ((val & BQ2589X_ICHGR_MASK) >> BQ2589X_ICHGR_SHIFT) * BQ2589X_ICHGR_LSB) ;
		return volt;
	}
}
EXPORT_SYMBOL_GPL(bq2589x_adc_read_charge_current);

int bq2589x_set_chargecurrent(struct bq2589x *bq, int curr)
{

	u8 ichg;

	ichg = (curr - BQ2589X_ICHG_BASE) / BQ2589X_ICHG_LSB;
	return bq2589x_update_bits(bq, BQ2589X_REG_04, BQ2589X_ICHG_MASK, ichg << BQ2589X_ICHG_SHIFT);

}
EXPORT_SYMBOL_GPL(bq2589x_set_chargecurrent);

int bq2589x_set_term_current(struct bq2589x *bq, int curr)
{
	u8 iterm;

	iterm = (curr - BQ2589X_ITERM_BASE) / BQ2589X_ITERM_LSB;

	return bq2589x_update_bits(bq, BQ2589X_REG_05, BQ2589X_ITERM_MASK, iterm << BQ2589X_ITERM_SHIFT);
}
EXPORT_SYMBOL_GPL(bq2589x_set_term_current);


int bq2589x_set_prechg_current(struct bq2589x *bq, int curr)
{
	u8 iprechg;

	iprechg = (curr - BQ2589X_IPRECHG_BASE) / BQ2589X_IPRECHG_LSB;

	return bq2589x_update_bits(bq, BQ2589X_REG_05, BQ2589X_IPRECHG_MASK, iprechg << BQ2589X_IPRECHG_SHIFT);
}
EXPORT_SYMBOL_GPL(bq2589x_set_prechg_current);

int bq2589x_set_chargevoltage(struct bq2589x *bq, int volt)
{
	u8 val;

	val = (volt - BQ2589X_VREG_BASE) / BQ2589X_VREG_LSB;
	return bq2589x_update_bits(bq, BQ2589X_REG_06, BQ2589X_VREG_MASK, val << BQ2589X_VREG_SHIFT);
}
EXPORT_SYMBOL_GPL(bq2589x_set_chargevoltage);


int bq2589x_set_input_volt_limit(struct bq2589x *bq, int volt)
{
	u8 val;
	val = (volt - BQ2589X_VINDPM_BASE) / BQ2589X_VINDPM_LSB;
	return bq2589x_update_bits(bq, BQ2589X_REG_0D, BQ2589X_VINDPM_MASK, val << BQ2589X_VINDPM_SHIFT);
}
EXPORT_SYMBOL_GPL(bq2589x_set_input_volt_limit);

int bq2589x_set_input_current_limit(struct bq2589x *bq, int curr)
{

	u8 val;

	val = (curr - BQ2589X_IINLIM_BASE) / BQ2589X_IINLIM_LSB;
	return bq2589x_update_bits(bq, BQ2589X_REG_00, BQ2589X_IINLIM_MASK, val << BQ2589X_IINLIM_SHIFT);
}
EXPORT_SYMBOL_GPL(bq2589x_set_input_current_limit);

int bq2589x_input_current_OTG(int curr)
{

	u8 ichg;

	return bq2589x_set_input_current_limit(g_bq1, curr);
}
EXPORT_SYMBOL_GPL(bq2589x_input_current_OTG);

int bq2589x_set_vindpm_offset(struct bq2589x *bq, int offset)
{
	u8 val;

	val = (offset - BQ2589X_VINDPMOS_BASE) / BQ2589X_VINDPMOS_LSB;
	return bq2589x_update_bits(bq, BQ2589X_REG_01, BQ2589X_VINDPMOS_MASK, val << BQ2589X_VINDPMOS_SHIFT);
}
EXPORT_SYMBOL_GPL(bq2589x_set_vindpm_offset);


void bq2589x_start_charging(struct bq2589x *bq)
{
	bq2589x_enable_charger(bq);
}
EXPORT_SYMBOL_GPL(bq2589x_start_charging);

void bq2589x_stop_charging(struct bq2589x *bq)
{
	bq2589x_disable_charger(bq);
}
EXPORT_SYMBOL_GPL(bq2589x_stop_charging);

int bq2589x_get_charging_status(struct bq2589x *bq)
{
	u8 val = 0;
	int ret;

	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_0B);
	if (ret < 0) {
		dev_err(bq->dev, "%s Failed to read register 0x0b:%d\n", __func__, ret);
		return ret;
	}
	val &= BQ2589X_CHRG_STAT_MASK;
	val >>= BQ2589X_CHRG_STAT_SHIFT;
	return val;
}
EXPORT_SYMBOL_GPL(bq2589x_get_charging_status);

int bq2589x_power_good(struct bq2589x *bq)
{
	u8 val = 0;
	int ret;

	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_0B);
	if (ret < 0) {
		dev_err(bq->dev, "%s Failed to read register 0x0b:%d\n", __func__, ret);
		return ret;
	}
	val &= BQ2589X_PG_STAT_MASK;
	val >>= BQ2589X_PG_STAT_SHIFT;
	return val;
}
EXPORT_SYMBOL_GPL(bq2589x_power_good);

void bq2589x_set_otg(struct bq2589x *bq, int enable)
{
	int ret;

	if (enable) {
		ret = bq2589x_enable_otg(bq);
		if (ret < 0) {
			dev_err(bq->dev, "%s:Failed to enable otg-%d\n", __func__, ret);
			return;
		}
	} else {
		ret = bq2589x_disable_otg(bq);
		if (ret < 0)
			dev_err(bq->dev, "%s:Failed to disable otg-%d\n", __func__, ret);
	}
}
EXPORT_SYMBOL_GPL(bq2589x_set_otg);

int bq2589x_set_watchdog_timer(struct bq2589x *bq, u8 timeout)
{
	return bq2589x_update_bits(bq, BQ2589X_REG_07, BQ2589X_WDT_MASK, (u8)((timeout - BQ2589X_WDT_BASE) / BQ2589X_WDT_LSB) << BQ2589X_WDT_SHIFT);
}
EXPORT_SYMBOL_GPL(bq2589x_set_watchdog_timer);

int bq2589x_disable_watchdog_timer(struct bq2589x *bq)
{
	u8 val = BQ2589X_WDT_DISABLE << BQ2589X_WDT_SHIFT;

	return bq2589x_update_bits(bq, BQ2589X_REG_07, BQ2589X_WDT_MASK, val);
}
EXPORT_SYMBOL_GPL(bq2589x_disable_watchdog_timer);

int bq2589x_reset_watchdog_timer(struct bq2589x *bq)
{
	u8 val = BQ2589X_WDT_RESET << BQ2589X_WDT_RESET_SHIFT;

	return bq2589x_update_bits(bq, BQ2589X_REG_03, BQ2589X_WDT_RESET_MASK, val);
}
EXPORT_SYMBOL_GPL(bq2589x_reset_watchdog_timer);

int bq2589x_force_dpdm(struct bq2589x *bq)
{
	int ret;
	u8 val = BQ2589X_FORCE_DPDM << BQ2589X_FORCE_DPDM_SHIFT;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_02, BQ2589X_FORCE_DPDM_MASK, val);
	if (ret)
		return ret;

	msleep(10);
	return 0;

}
EXPORT_SYMBOL_GPL(bq2589x_force_dpdm);

int bq2589x_reset_chip(struct bq2589x *bq)
{
	int ret;
	u8 val = BQ2589X_RESET << BQ2589X_RESET_SHIFT;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_14, BQ2589X_RESET_MASK, val);
	return ret;
}
EXPORT_SYMBOL_GPL(bq2589x_reset_chip);

int bq2589x_enter_ship_mode(struct bq2589x *bq)
{
	int ret;
	u8 val = BQ2589X_BATFET_OFF << BQ2589X_BATFET_DIS_SHIFT;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_09, BQ2589X_BATFET_DIS_MASK, val);
	return ret;

}
EXPORT_SYMBOL_GPL(bq2589x_enter_ship_mode);

int bq2589x_exit_ship_mode(struct bq2589x *bq)
{
	int ret;
	u8 val = 0x0 << BQ2589X_BATFET_DIS_SHIFT;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_09, BQ2589X_BATFET_DIS_MASK, val);
	return ret;

}
EXPORT_SYMBOL_GPL(bq2589x_exit_ship_mode);


int bq2589x_enter_hiz_mode(struct bq2589x *bq)
{
	u8 val = BQ2589X_HIZ_ENABLE << BQ2589X_ENHIZ_SHIFT;

	return bq2589x_update_bits(bq, BQ2589X_REG_00, BQ2589X_ENHIZ_MASK, val);

}
EXPORT_SYMBOL_GPL(bq2589x_enter_hiz_mode);

int bq2589x_exit_hiz_mode(struct bq2589x *bq)
{

	u8 val = BQ2589X_HIZ_DISABLE << BQ2589X_ENHIZ_SHIFT;

	return bq2589x_update_bits(bq, BQ2589X_REG_00, BQ2589X_ENHIZ_MASK, val);

}
EXPORT_SYMBOL_GPL(bq2589x_exit_hiz_mode);

int bq2589x_get_hiz_mode(struct bq2589x *bq, u8 *state)
{
	u8 val;
	int ret;

	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_00);
	if (ret)
		return ret;
	*state = (val & BQ2589X_ENHIZ_MASK) >> BQ2589X_ENHIZ_SHIFT;

	return 0;
}
EXPORT_SYMBOL_GPL(bq2589x_get_hiz_mode);

int bq2589x_enable_ilim_pin(struct bq2589x *bq)
{
	u8 val = BQ2589X_ENILIM_ENABLE << BQ2589X_ENILIM_SHIFT;

	return bq2589x_update_bits(bq, BQ2589X_REG_00, BQ2589X_ENILIM_MASK, val);

}
EXPORT_SYMBOL_GPL(bq2589x_enable_ilim_pin);

int bq2589x_disable_ilim_pin(struct bq2589x *bq)
{
	u8 val = BQ2589X_ENILIM_DISABLE << BQ2589X_ENILIM_SHIFT;

	return bq2589x_update_bits(bq, BQ2589X_REG_00, BQ2589X_ENILIM_MASK, val);

}
EXPORT_SYMBOL_GPL(bq2589x_disable_ilim_pin);


int bq2589x_pumpx_enable(struct bq2589x *bq, int enable)
{
	u8 val;
	int ret;

	if (enable)
		val = BQ2589X_PUMPX_ENABLE << BQ2589X_EN_PUMPX_SHIFT;
	else
		val = BQ2589X_PUMPX_DISABLE << BQ2589X_EN_PUMPX_SHIFT;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_04, BQ2589X_EN_PUMPX_MASK, val);

	return ret;
}
EXPORT_SYMBOL_GPL(bq2589x_pumpx_enable);

int bq2589x_pumpx_increase_volt(struct bq2589x *bq)
{
	u8 val;
	int ret;

	val = BQ2589X_PUMPX_UP << BQ2589X_PUMPX_UP_SHIFT;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_09, BQ2589X_PUMPX_UP_MASK, val);

	return ret;

}
EXPORT_SYMBOL_GPL(bq2589x_pumpx_increase_volt);

int bq2589x_pumpx_increase_volt_done(struct bq2589x *bq)
{
	u8 val;
	int ret;

	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_09);
	if (ret)
		return ret;

	if (val & BQ2589X_PUMPX_UP_MASK)
		return 1;   /* not finished*/
	else
		return 0;   /* pumpx up finished*/

}
EXPORT_SYMBOL_GPL(bq2589x_pumpx_increase_volt_done);

int bq2589x_pumpx_decrease_volt(struct bq2589x *bq)
{
	u8 val;
	int ret;

	val = BQ2589X_PUMPX_DOWN << BQ2589X_PUMPX_DOWN_SHIFT;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_09, BQ2589X_PUMPX_DOWN_MASK, val);

	return ret;

}
EXPORT_SYMBOL_GPL(bq2589x_pumpx_decrease_volt);

int bq2589x_pumpx_decrease_volt_done(struct bq2589x *bq)
{
	u8 val;
	int ret;

	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_09);
	if (ret)
		return ret;

	if (val & BQ2589X_PUMPX_DOWN_MASK)
		return 1;   /* not finished*/
	else
		return 0;   /* pumpx down finished*/

}
EXPORT_SYMBOL_GPL(bq2589x_pumpx_decrease_volt_done);

static int bq2589x_force_ico(struct bq2589x *bq)
{
	u8 val;
	int ret;

	val = BQ2589X_FORCE_ICO << BQ2589X_FORCE_ICO_SHIFT;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_09, BQ2589X_FORCE_ICO_MASK, val);

	return ret;
}
EXPORT_SYMBOL_GPL(bq2589x_force_ico);

static int bq2589x_check_force_ico_done(struct bq2589x *bq)
{
	u8 val;
	int ret;

	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_14);
	if (ret)
		return ret;

	if (val & BQ2589X_ICO_OPTIMIZED_MASK)
		return 1;  /*finished*/
	else
		return 0;   /* in progress*/
}
EXPORT_SYMBOL_GPL(bq2589x_check_force_ico_done);

static int bq2589x_enable_term(struct bq2589x* bq, bool enable)
{
	u8 val;
	int ret;

	if (enable)
		val = BQ2589X_TERM_ENABLE << BQ2589X_EN_TERM_SHIFT;
	else
		val = BQ2589X_TERM_DISABLE << BQ2589X_EN_TERM_SHIFT;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_07, BQ2589X_EN_TERM_MASK, val);

	return ret;
}
EXPORT_SYMBOL_GPL(bq2589x_enable_term);
static int bq2589x_enable_auto_dpdm(struct bq2589x* bq, bool enable)
{
	u8 val;
	int ret;
	
	if (enable)
		val = BQ2589X_AUTO_DPDM_ENABLE << BQ2589X_AUTO_DPDM_EN_SHIFT;
	else
		val = BQ2589X_AUTO_DPDM_DISABLE << BQ2589X_AUTO_DPDM_EN_SHIFT;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_02, BQ2589X_AUTO_DPDM_EN_MASK, val);

	return ret;

}
EXPORT_SYMBOL_GPL(bq2589x_enable_auto_dpdm);

static int bq2589x_enable_absolute_vindpm(struct bq2589x* bq, bool enable)
{
	u8 val;
	int ret;
	
	if (enable)
		val = BQ2589X_FORCE_VINDPM_ENABLE << BQ2589X_FORCE_VINDPM_SHIFT;
	else
		val = BQ2589X_FORCE_VINDPM_DISABLE << BQ2589X_FORCE_VINDPM_SHIFT;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_0D, BQ2589X_FORCE_VINDPM_MASK, val);

	return ret;

}
EXPORT_SYMBOL_GPL(bq2589x_enable_absolute_vindpm);

static int bq2589x_enable_ico(struct bq2589x* bq, bool enable)
{
	u8 val;
	int ret;
	
	if (enable)
		val = BQ2589X_ICO_ENABLE << BQ2589X_ICOEN_SHIFT;
	else
		val = BQ2589X_ICO_DISABLE << BQ2589X_ICOEN_SHIFT;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_02, BQ2589X_ICOEN_MASK, val);

	return ret;

}
EXPORT_SYMBOL_GPL(bq2589x_enable_ico);
static int bq2589x_read_idpm_limit(struct bq2589x *bq)
{
	uint8_t val;
	int curr;
	int ret;

	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_13);
	if (ret < 0) {
		dev_err(bq->dev, "read vbus voltage failed :%d\n", ret);
		return ret;
	} else {
		curr = BQ2589X_IDPM_LIM_BASE + ((val & BQ2589X_IDPM_LIM_MASK) >> BQ2589X_IDPM_LIM_SHIFT) * BQ2589X_IDPM_LIM_LSB ;
		return curr;
	}
}
EXPORT_SYMBOL_GPL(bq2589x_read_idpm_limit);

static bool bq2589x_is_charge_done(struct bq2589x *bq)
{
	int ret;
	u8 val;

	ret = bq2589x_read_byte(bq, &val, BQ2589X_REG_0B);
	if (ret < 0) {
		dev_err(bq->dev, "%s:read REG0B failed :%d\n", __func__, ret);
		return false;
	}
	val &= BQ2589X_CHRG_STAT_MASK;
	val >>= BQ2589X_CHRG_STAT_SHIFT;

	return (val == BQ2589X_CHRG_STAT_CHGDONE);
}
EXPORT_SYMBOL_GPL(bq2589x_is_charge_done);

static bool bq2589x_is_fast_charge_timer(struct bq2589x *bq, int hour)
{
	u8 val;
	int ret = 0;
	if (hour == 5)
		ret = bq2589x_update_bits(bq, BQ2589X_REG_07, BQ2589X_CHG_TIMER_MASK, 0x0<<BQ2589X_CHG_TIMER_SHIFT);
	else if (hour == 8)
		ret = bq2589x_update_bits(bq, BQ2589X_REG_07, BQ2589X_CHG_TIMER_MASK, 0x1<<BQ2589X_CHG_TIMER_SHIFT);
	else if (hour == 12)
		ret = bq2589x_update_bits(bq, BQ2589X_REG_07, BQ2589X_CHG_TIMER_MASK, 0x2<<BQ2589X_CHG_TIMER_SHIFT);
	else if (hour == 20)
		ret = bq2589x_update_bits(bq, BQ2589X_REG_07, BQ2589X_CHG_TIMER_MASK, 0x3<<BQ2589X_CHG_TIMER_SHIFT);
	else
		printk("hour should be 5, 8, 12, 20\n");

	return ret;
}
EXPORT_SYMBOL_GPL(bq2589x_is_fast_charge_timer);

static bool bq2589x_disable_HAXC_HVDCP(struct bq2589x *bq)
{
	u8 val;
	int ret = 0;

	ret = bq2589x_update_bits(bq, BQ2589X_REG_02, BQ2589X_MAXCEN_MASK, BQ2589X_MAXC_DISABLE);
	if (ret < 0)
		printk("HAXC disable error\n");

	ret = bq2589x_update_bits(bq, BQ2589X_REG_02, BQ2589X_HVDCPEN_MASK, BQ2589X_HVDCP_DISABLE);
	if (ret < 0)
		printk("HVDCP disable error\n");

	return ret;
}
EXPORT_SYMBOL_GPL(bq2589x_disable_HAXC_HVDCP);

static int bq2589x_init_device(struct bq2589x *bq)
{

	int ret;
	u8 addr, tmp;

	/*common initialization*/
	bq2589x_disable_watchdog_timer(bq);

	bq2589x_enable_auto_dpdm(bq, bq->cfg.enable_auto_dpdm);
	bq2589x_enable_term(bq, bq->cfg.enable_term);
	bq2589x_enable_ico(bq, bq->cfg.enable_ico);
	bq2589x_enable_absolute_vindpm(bq, bq->cfg.enable_absolute_vindpm);

	ret = bq2589x_set_vindpm_offset(bq, 600);
	if (ret < 0) {
		dev_err(bq->dev, "%s:Failed to set vindpm offset:%d\n", __func__, ret);
		return ret;
	}

	// Input current limit
	//bq2589x_read_byte(bq, &tmp, BQ2589X_REG_0B);
	//tmp &= BQ2589X_VBUS_STAT_MASK;
	//tmp >>= BQ2589X_VBUS_STAT_SHIFT;
	if (otg_flag != 1) {
		ret = bq2589x_set_input_current_limit(bq, 3000);
		if (ret < 0) {
			dev_err(bq->dev, "%s:Failed to set input current:%d\n", __func__, ret);
			return ret;
		}
	}

	ret = bq2589x_set_input_volt_limit(bq, 4400);
	if (ret < 0) {
		dev_err(bq->dev, "%s:Failed to set input volt limit:%d\n", __func__, ret);
		return ret;
	}

	ret = bq2589x_set_term_current(bq, bq->cfg.term_current);
	if (ret < 0) {
		dev_err(bq->dev, "%s:Failed to set termination current:%d\n", __func__, ret);
		return ret;
	}

	ret = bq2589x_set_chargevoltage(bq, bq->cfg.charge_voltage);
	if (ret < 0) {
		dev_err(bq->dev, "%s:Failed to set charge voltage:%d\n", __func__, ret);
		return ret;
	}

	ret = bq2589x_set_chargecurrent(bq, bq->cfg.charge_current);
	if (ret < 0) {
		dev_err(bq->dev, "%s:Failed to set charge current:%d\n", __func__, ret);
		return ret;
	}

	ret = bq2589x_set_prechg_current(bq, 320);
	if (ret < 0) {
		dev_err(bq->dev, "%s:Failed to set prechg current:%d\n", __func__, ret);
		return ret;
	}

	ret = bq2589x_enable_charger(bq);
	if (ret < 0) {
		dev_err(bq->dev, "%s:Failed to enable charger:%d\n", __func__, ret);
		return ret;
	}

	bq2589x_disable_ilim_pin(bq);

	bq2589x_enable_otg(bq);

	bq2589x_exit_ship_mode(bq);

	bq2589x_is_fast_charge_timer(bq, 20);
	bq2589x_read_byte(bq, &tmp, BQ2589X_REG_07);
	bq2589x_disable_HAXC_HVDCP(bq);
	
	bq2589x_adc_start(bq, false);

	return ret;
}


static int bq2589x_charge_status(struct bq2589x *bq)
{
	u8 val = 0;

	bq2589x_read_byte(bq, &val, BQ2589X_REG_0B);
	val &= BQ2589X_CHRG_STAT_MASK;
	val >>= BQ2589X_CHRG_STAT_SHIFT;
	switch (val) {
	case BQ2589X_CHRG_STAT_FASTCHG:
		return POWER_SUPPLY_CHARGE_TYPE_FAST;
	case BQ2589X_CHRG_STAT_PRECHG:
		return POWER_SUPPLY_CHARGE_TYPE_TRICKLE;
	case BQ2589X_CHRG_STAT_CHGDONE:
		return POWER_SUPPLY_CHARGE_TYPE_TRICKLE;
	case BQ2589X_CHRG_STAT_IDLE:
		return POWER_SUPPLY_CHARGE_TYPE_NONE;
	default:
		return POWER_SUPPLY_CHARGE_TYPE_UNKNOWN;
	}
}

static enum power_supply_property bq2589x_charger_props[] = {
	POWER_SUPPLY_PROP_CHARGE_TYPE, /* Charger status output */
	POWER_SUPPLY_PROP_ONLINE, /* External power source */
};


static int bq2589x_usb_get_property(struct power_supply *psy,
				enum power_supply_property psp,
				union power_supply_propval *val)
{

	struct bq2589x *bq = container_of(psy, struct bq2589x, usb);
	u8 type = bq2589x_get_vbus_type(bq);
	u8 charging_status = bq2589x_get_charging_status(bq);

	switch (psp) {
	case POWER_SUPPLY_PROP_STATUS:
		if (charging_status == 0x0)
			val->intval = POWER_SUPPLY_STATUS_DISCHARGING;
		else if (charging_status == 0x01 || charging_status == 0x2)
			val->intval = POWER_SUPPLY_STATUS_CHARGING;
		else if (charging_status == 0x03)
			val->intval = POWER_SUPPLY_STATUS_FULL;
		break;
	case POWER_SUPPLY_PROP_ONLINE:
		if (bq2589x_charge_status(bq) > 1)
			val->intval = 1;
		else
			val->intval = 0 ;
		break;
	case POWER_SUPPLY_PROP_CHARGE_TYPE:
		val->intval = bq2589x_charge_status(bq);
		break;
	case POWER_SUPPLY_PROP_USB_OTG:
		if (type == BQ2589X_VBUS_OTG)
			val->intval = 1;
		else
			val->intval = 0;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int bq2589x_wall_get_property(struct power_supply *psy,
			enum power_supply_property psp,
			union power_supply_propval *val)
{

	struct bq2589x *bq = container_of(psy, struct bq2589x, wall);
	u8 type = bq2589x_get_vbus_type(bq);
	u8 charging_status = bq2589x_get_charging_status(bq);

	switch (psp) {
	case POWER_SUPPLY_PROP_STATUS:
		if (charging_status == 0x0)
			val->intval = POWER_SUPPLY_STATUS_DISCHARGING;
		else if (charging_status == 0x01 || charging_status == 0x2)
			val->intval = POWER_SUPPLY_STATUS_CHARGING;
		else if (charging_status == 0x03)
			val->intval = POWER_SUPPLY_STATUS_FULL;
		break;		
	case POWER_SUPPLY_PROP_ONLINE:
		if (bq2589x_charge_status(bq) > 1)
			val->intval = 1;
		else
			val->intval = 0 ;		
		break;
	case POWER_SUPPLY_PROP_CHARGE_TYPE:
		val->intval = bq2589x_charge_status(bq);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}
#if 0
// ======================================================================
static int bq2589x_battery_get_property(struct power_supply *psy,
				enum power_supply_property psp,
				union power_supply_propval *val)
{

	struct bq2589x *bq = container_of(psy, struct bq2589x, battery);
	u8 charging_status = bq2589x_get_charging_status(bq);

	switch (psp) {
		case POWER_SUPPLY_PROP_STATUS:
			if (charging_status == 0x0)
				val->intval = POWER_SUPPLY_STATUS_DISCHARGING;
			else if (charging_status == 0x01 || charging_status == 0x2)
				val->intval = POWER_SUPPLY_STATUS_CHARGING;
			else if (charging_status == 0x03)
				val->intval = POWER_SUPPLY_STATUS_FULL;
		break;

		case POWER_SUPPLY_PROP_HEALTH:
			if (bq2589x_power_good(bq) == 1)
				val->intval = POWER_SUPPLY_HEALTH_GOOD;
			else
				val->intval = POWER_SUPPLY_HEALTH_DEAD;
		break;
		
		case POWER_SUPPLY_PROP_TECHNOLOGY:	
			val->intval = POWER_SUPPLY_TECHNOLOGY_LION;
		break;

		default:
			return -EINVAL;
	}

	return 0;
}


static enum power_supply_property bq2589x_battery_properties[] = {
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_HEALTH,
	POWER_SUPPLY_PROP_TECHNOLOGY,		
};
// ======================================================================
#endif
static int bq2589x_psy_register(struct bq2589x *bq)
{
	int ret;

	bq->usb.name = "USB";
	bq->usb.type = POWER_SUPPLY_TYPE_USB;
	bq->usb.properties = bq2589x_charger_props;
	bq->usb.num_properties = ARRAY_SIZE(bq2589x_charger_props);
	bq->usb.get_property = bq2589x_usb_get_property;
	bq->usb.external_power_changed = NULL;

	ret = power_supply_register(bq->dev, &bq->usb);
	if (ret < 0) {
		dev_err(bq->dev, "%s:failed to register usb psy:%d\n", __func__, ret);
		return ret;
	}

	bq->wall.name = "Mains";
	bq->wall.type = POWER_SUPPLY_TYPE_MAINS;
	bq->wall.properties = bq2589x_charger_props;
	bq->wall.num_properties = ARRAY_SIZE(bq2589x_charger_props);
	bq->wall.get_property = bq2589x_wall_get_property;
	bq->wall.external_power_changed = NULL;

	ret = power_supply_register(bq->dev, &bq->wall);
	if (ret < 0) {
		dev_err(bq->dev, "%s:failed to register wall psy:%d\n", __func__, ret);
		goto fail_1;
	}
#if 0
	// ===============================================================
	bq->battery.name = "battery";
	bq->battery.type = POWER_SUPPLY_TYPE_BATTERY;
	bq->battery.properties = bq2589x_battery_properties;
	bq->battery.num_properties = ARRAY_SIZE(bq2589x_battery_properties);
	bq->battery.get_property = bq2589x_battery_get_property;

	ret = power_supply_register(bq->dev, &bq->battery);
	if (ret < 0) {
		dev_err(bq->dev, "%s:failed to register battery psy:%d\n", __func__, ret);
		goto fail_1;
	}
	// ===============================================================
#endif
	return 0;

fail_1:
	power_supply_unregister(&bq->usb);
	return ret;
}

static void bq2589x_psy_unregister(struct bq2589x *bq)
{
	power_supply_unregister(&bq->usb);
	power_supply_unregister(&bq->wall);
	//power_supply_unregister(&bq->battery);
}

static ssize_t bq2589x_show_registers(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	u8 addr;
	u8 val;
	int len;
	int idx = 0;
	int ret ;

	idx = snprintf(buf, PAGE_SIZE,"%s:\n", "Charger 1");
	for (addr = 0x0; addr <= 0x14; addr++) {
		ret = bq2589x_read_byte(g_bq1, &val, addr);
		if (ret == 0) {
			len = snprintf(&buf[idx], PAGE_SIZE - idx, "Reg[0x%.2x] = 0x%.2x\n", addr, val);
			idx += len;
		}
	}

	return idx;
}

static DEVICE_ATTR(registers, S_IRUGO, bq2589x_show_registers, NULL);

static struct attribute *bq2589x_attributes[] = {
	&dev_attr_registers.attr,
	NULL,
};

static const struct attribute_group bq2589x_attr_group = {
	.attrs = bq2589x_attributes,
};


static int bq2589x_parse_dt(struct device *dev, struct bq2589x *bq)
{
	int ret;
	struct device_node *np = dev->of_node;

	ret = of_property_read_u32(np, "ti,bq2589x,vbus-volt-high-level", &pe.high_volt_level);

	ret = of_property_read_u32(np, "ti,bq2589x,vbus-volt-low-level", &pe.low_volt_level);

	ret = of_property_read_u32(np, "ti,bq2589x,vbat-min-volt-to-tuneup", &pe.vbat_min_volt);

	bq->cfg.enable_auto_dpdm = of_property_read_bool(np, "ti,bq2589x,enable-auto-dpdm");
	bq->cfg.enable_term = of_property_read_bool(np, "ti,bq2589x,enable-termination");

	bq->cfg.enable_ico = of_property_read_bool(np, "ti,bq2589x,enable-ico");
	bq->cfg.enable_absolute_vindpm = of_property_read_bool(np, "ti,bq2589x,use-absolute-vindpm");

	ret = of_property_read_u32(np, "ti,bq2589x,charge-voltage",&bq->cfg.charge_voltage);

	ret = of_property_read_u32(np, "ti,bq2589x,charge-current",&bq->cfg.charge_current);

	ret = of_property_read_u32(np, "ti,bq2589x,term-current",&bq->cfg.term_current);

	return 0;
}

static int bq2589x_detect_device(struct bq2589x *bq)
{
	int ret;
	u8 data;

	ret = bq2589x_read_byte(bq, &data, BQ2589X_REG_14);

	if (ret == 0) {
		bq->part_no = (data & BQ2589X_PN_MASK) >> BQ2589X_PN_SHIFT;
		bq->revision = (data & BQ2589X_DEV_REV_MASK) >> BQ2589X_DEV_REV_SHIFT;
	}

	return ret;
}


static int bq2589x_read_batt_rsoc(struct bq2589x *bq)
{
	union power_supply_propval ret = {0,};

	if (!bq->batt_psy) 
		bq->batt_psy = power_supply_get_by_name("battery");

	if (bq->batt_psy) {
		bq->batt_psy->get_property(bq->batt_psy,POWER_SUPPLY_PROP_CAPACITY,&ret);
		return ret.intval;
	} else {
		return 50;
	}
}


static void bq2589x_adjust_absolute_vindpm(struct bq2589x *bq)
{
	u16 vbus_volt;
	u16 vindpm_volt;
	int ret;

	msleep(1000);
	vbus_volt = bq2589x_adc_read_vbus_volt(bq);
	if (vbus_volt < 6000)
		vindpm_volt = vbus_volt - 600;
	else
		vindpm_volt = vbus_volt - 1200;

	ret = bq2589x_set_input_volt_limit(bq, vindpm_volt);
	if (ret < 0)
		dev_err(bq->dev, "%s:Set absolute vindpm threshold %d Failed:%d\n", __func__, vindpm_volt, ret);
	else
		dev_info(bq->dev, "%s:Set absolute vindpm threshold %d successfully\n", __func__, vindpm_volt);

}

static void bq2589x_adapter_in_workfunc(struct  bq2589x *bq)
{
	int ret;
	u8 status = 0;
	//set input current limit to 3.25A

	
	//bq2589x_read_byte(bq, &status, BQ2589X_REG_0B);
	//status &= BQ2589X_VBUS_STAT_MASK;
	//status >>= BQ2589X_VBUS_STAT_SHIFT;

	if (otg_flag != 1) {
		//set input current limit to 3.00A
		ret = bq2589x_set_input_current_limit(bq, 3000);
		if (ret < 0) {
			dev_err(bq->dev, "%s:Failed to set input current:%d\n", __func__, ret);
			return ret;
		}
	}
	
 #if 0
	if (bq->vbus_type == BQ2589X_VBUS_MAXC) {
		dev_info(bq->dev, "%s:HVDCP or Maxcharge adapter plugged in\n", __func__);
 		//schedule_delayed_work(&bq->ico_work, 0);
	} else if (bq->vbus_type == BQ2589X_VBUS_USB_DCP) {/* DCP, let's check if it is PE adapter*/
		dev_info(bq->dev, "%s:usb dcp adapter plugged in\n", __func__);
 		//schedule_delayed_work(&bq->check_pe_tuneup_work, 0);
	} else {
		dev_info(bq->dev, "%s:other adapter plugged in,vbus_type is %d\n", __func__, bq->vbus_type);
 		//schedule_delayed_work(&bq->ico_work, 0);
	}

	//set input current limit to 3.25A
	ret = bq2589x_set_input_current_limit(bq, 3250);
	if (ret < 0) {
		dev_err(bq->dev, "%s:Failed to set input current:%d\n", __func__, ret);
		return ret;
	}	

	//if (bq->cfg.enable_absolute_vindpm) {
	//	bq2589x_adjust_absolute_vindpm(bq);
	//}

	//schedule_delayed_work(&bq->monitor_work, 0);
#endif
}

static void bq2589x_adapter_out_workfunc(struct bq2589x *bq)
{
	int ret;

	// Input current limit
	/*
	ret = bq2589x_set_input_current_limit(bq, 3250);
	if (ret < 0) {
		dev_err(bq->dev, "%s:Failed to set input current:%d\n", __func__, ret);
		return ret;
	}
	*/
}

static void check_adapter_type(struct bq2589x *bq)
{
	if (!bq->cfg.enable_auto_dpdm && bq2589x_force_dpdm(bq)) {
		dev_err(bq->dev,"failed to do force dpdm, vbus type is forced to DCP\n");
		bq->vbus_type = BQ2589X_VBUS_USB_DCP;
	} else{
		bq->vbus_type = bq2589x_get_vbus_type(bq);	
	}
}

static void bq2589x_charger1_irq_workfunc(struct work_struct *work)
{
	struct bq2589x *bq = container_of(work, struct bq2589x, irq_work);
	u8 status = 0;
	u8 fault = 0;
	int ret;
 
	msleep(5);

#if 0
	if (!(bq->status & BQ2589X_STATUS_PLUGIN)){
		//printk("[bq2589x_charger1_irq_workfunc] BQ2589X_STATUS_PLUGIN\n");
		check_adapter_type(bq);
	}
	else{
		//printk("[bq2589x_charger1_irq_workfunc] bq2589x_get_vbus_type\n");
		bq->vbus_type = bq2589x_get_vbus_type(bq);
	}
#else
	bq->vbus_type = bq2589x_get_vbus_type(bq);
#endif

	// debug 
	//printk("\n\n[bq2589x_charger1_irq_workfunc]vbus_type:%d\n\n", bq->vbus_type);


	/* Read STATUS and FAULT registers */
	ret = bq2589x_read_byte(bq, &status, BQ2589X_REG_0B);
	if (ret){
		printk("[bq2589x_charger1_irq_workfunc] Read STATUS error!!\n");
		return;
	}
	ret = bq2589x_read_byte(bq, &fault, BQ2589X_REG_0C);
	if (ret){
		printk("[bq2589x_charger1_irq_workfunc] Read FAULT error!!\n");
		return;
	}

	if ((status & BQ2589X_PG_STAT_MASK) && !(bq->status & BQ2589X_STATUS_PG))
		bq->status |= BQ2589X_STATUS_PG;
	else if (!(status & BQ2589X_PG_STAT_MASK) && (bq->status & BQ2589X_STATUS_PG))
		bq->status &= ~BQ2589X_STATUS_PG;

	if (fault && !(bq->status & BQ2589X_STATUS_FAULT))
		bq->status |= BQ2589X_STATUS_FAULT;
	else if (!fault && (bq->status & BQ2589X_STATUS_FAULT))
		bq->status &= ~BQ2589X_STATUS_FAULT;

	bq->interrupt = true;

	if (bq->vbus_type == BQ2589X_VBUS_NONE && !(bq->status & BQ2589X_STATUS_PG)) {
		dev_info(bq->dev, "%s:adapter removed\n", __func__); 
		printk("%s:adapter removed\n", __func__);
		bq->status &= ~BQ2589X_STATUS_PLUGIN;
		bq2589x_adapter_out_workfunc(bq);
	} else if (bq->vbus_type != BQ2589X_VBUS_NONE  && (bq->status & BQ2589X_STATUS_PG)) {
		dev_info(bq->dev, "%s:adapter plugged in\n", __func__); 
		printk("%s:adapter plugged in\n", __func__);
		bq->status |= BQ2589X_STATUS_PLUGIN;
		bq2589x_adapter_in_workfunc(bq);
	}

 	power_supply_changed(&bq->wall);
	power_supply_changed(&bq->usb);
}

static void bq2589x_detection_func(struct bq2589x *bq)
{
	u8 status = 0;
	u8 fault = 0;
	int ret;
	 
	msleep(5);
	
#if 0
	if (!(bq->status & BQ2589X_STATUS_PLUGIN)){
		check_adapter_type(bq);
	}
	else{
		bq->vbus_type = bq2589x_get_vbus_type(bq);
	}
#else
	bq->vbus_type = bq2589x_get_vbus_type(bq);
	//printk("\n\n[bq2589x_charger1_irq_workfunc]vbus_type:%d\n\n", bq->vbus_type);
#endif
	
	/* Read STATUS and FAULT registers */
	ret = bq2589x_read_byte(bq, &status, BQ2589X_REG_0B);
	if (ret){
		printk("[bq2589x_detection_func] Read STATUS error!!\n");
		return;
	}
	ret = bq2589x_read_byte(bq, &fault, BQ2589X_REG_0C);
	if (ret){
		printk("[bq2589x_detection_func] Read FAULT error!!\n");
		return;
	}
	
	if ((status & BQ2589X_PG_STAT_MASK) && !(bq->status & BQ2589X_STATUS_PG))
		bq->status |= BQ2589X_STATUS_PG;
	else if (!(status & BQ2589X_PG_STAT_MASK) && (bq->status & BQ2589X_STATUS_PG))
		bq->status &= ~BQ2589X_STATUS_PG;
	
	if (fault && !(bq->status & BQ2589X_STATUS_FAULT))
		bq->status |= BQ2589X_STATUS_FAULT;
	else if (!fault && (bq->status & BQ2589X_STATUS_FAULT))
		bq->status &= ~BQ2589X_STATUS_FAULT;

	if (bq->vbus_type == BQ2589X_VBUS_NONE && !(bq->status & BQ2589X_STATUS_PG)) {
		dev_info(bq->dev, "%s:adapter removed\n", __func__); 
		printk("%s:adapter removed\n", __func__);
		bq->status &= ~BQ2589X_STATUS_PLUGIN;
		bq2589x_adapter_out_workfunc(bq);
	} else if (bq->vbus_type != BQ2589X_VBUS_NONE  && (bq->status & BQ2589X_STATUS_PG)) {
		dev_info(bq->dev, "%s:adapter plugged in\n", __func__); 
		printk("%s:adapter plugged in\n", __func__);
		bq->status |= BQ2589X_STATUS_PLUGIN;
		bq2589x_adapter_in_workfunc(bq);
	}

	
	power_supply_changed(&bq->wall);
	power_supply_changed(&bq->usb);
}

static irqreturn_t bq2589x_charger1_interrupt(int irq, void *data)
{
	struct bq2589x *bq = data;
 
	schedule_work(&bq->irq_work);
	
	return IRQ_HANDLED;
}

int bq_suspend(struct device *dev)
{
	return 0;
}

int bq_resume(struct device *dev)
{
	//bq2589x_init_device(g_bq1);
	// check
	bq2589x_detection_func(g_bq1);
	return 0;
}

static int bq2589x_charger1_probe(struct i2c_client *client,
			   const struct i2c_device_id *id)
{
	struct bq2589x *bq;
	int irqn;

	int ret;
 
	bq = devm_kzalloc(&client->dev, sizeof(struct bq2589x), GFP_KERNEL);
	if (!bq) {
		dev_err(&client->dev, "%s: out of memory\n", __func__);
		return -ENOMEM;
	}

	bq->dev = &client->dev;
	bq->client = client;
	i2c_set_clientdata(client, bq);

	ret = bq2589x_detect_device(bq);
	
	if (!ret && bq->part_no == BQ25890) {
		bq->status |= BQ2589X_STATUS_EXIST;
		dev_info(bq->dev, "%s: charger device bq25890 detected, revision:%d\n", __func__, bq->revision);
	} 

	bq->batt_psy = power_supply_get_by_name("battery");

	g_bq1 = bq;

	if (client->dev.of_node)
		 bq2589x_parse_dt(&client->dev, g_bq1);

	ret = bq2589x_init_device(g_bq1);
	if (ret) {
		dev_err(bq->dev, "device init failure: %d\n", ret);
		goto err_0;
	}

	ret = gpio_request(IRQ_GPIO_NUM, "bq2589x irq pin");
	if (ret) {
		dev_err(bq->dev, "%s: %d gpio request failed\n", __func__, IRQ_GPIO_NUM);
		goto err_0;
	}
	gpio_direction_input(IRQ_GPIO_NUM);

	irqn = gpio_to_irq(IRQ_GPIO_NUM);
	if (irqn < 0) {
		dev_err(bq->dev, "%s:%d gpio_to_irq failed\n", __func__, irqn);
		ret = irqn;
		goto err_1;
	}
	client->irq = irqn;


	ret = bq2589x_psy_register(bq);
	if (ret)
		goto err_0;

	INIT_WORK(&bq->irq_work, bq2589x_charger1_irq_workfunc);

	ret = sysfs_create_group(&bq->dev->kobj, &bq2589x_attr_group);
	if (ret) {
		dev_err(bq->dev, "failed to register sysfs. err: %d\n", ret);
		goto err_irq;
	}

	ret = request_irq(client->irq, bq2589x_charger1_interrupt, IRQF_TRIGGER_FALLING | IRQF_ONESHOT, "bq2589x_charger1_irq", bq);
	if (ret) {
		dev_err(bq->dev, "%s:Request IRQ %d failed: %d\n", __func__, client->irq, ret);
		goto err_irq;
	} else {
		dev_info(bq->dev, "%s:irq = %d\n", __func__, client->irq);
	}

	pe.enable = true;

	/*in case of adapter has been in when power off*/
	//schedule_work(&bq->irq_work);
	bq2589x_detection_func(bq);

	// Debug
	// =============================================================================
	/*
	u8 addr;
	u8 val;
	for (addr = 0x0; addr <= 0x14; addr++) {
		ret = bq2589x_read_byte(g_bq1, &val, addr);
		if (ret == 0) {
			printk("Reg[0x%.2x] = 0x%.2x\n", addr, val);
		}
	}
	*/
	// =============================================================================
 	
	return 0;

err_irq:
	cancel_work_sync(&bq->irq_work);
err_1:
	gpio_free(IRQ_GPIO_NUM);
err_0:
	g_bq1 = NULL;
	return ret;
}

static void bq2589x_charger1_shutdown(struct i2c_client *client)
{
	struct bq2589x *bq = i2c_get_clientdata(client);

	//dev_info(bq->dev, "%s: shutdown\n", __func__);

	bq2589x_psy_unregister(bq);

	sysfs_remove_group(&bq->dev->kobj, &bq2589x_attr_group);
	cancel_work_sync(&bq->irq_work);

	gpio_free(IRQ_GPIO_NUM);

	g_bq1 = NULL;
}

static struct of_device_id bq2589x_charger1_match_table[] = {
	{.compatible = "ti,bq2589x-1",},
	{},
};


static const struct i2c_device_id bq2589x_charger1_id[] = {
	{ "bq2589x-1", BQ25890 },
	{},
};

static const struct dev_pm_ops bq2589x_pm_ops = {
	.suspend = bq_suspend,
	.resume = bq_resume,
};

MODULE_DEVICE_TABLE(i2c, bq2589x_charger1_id);

static struct i2c_driver bq2589x_charger1_driver = {
	.driver		= {
		.name	= "bq2589x-1",
		.of_match_table = bq2589x_charger1_match_table,
#ifdef CONFIG_PM
		.pm = &bq2589x_pm_ops,
#endif		
	},
	.id_table	= bq2589x_charger1_id,

	.probe		= bq2589x_charger1_probe,
	.shutdown   = bq2589x_charger1_shutdown,
};

module_i2c_driver(bq2589x_charger1_driver);

#if 0
static int __init bq2589x_charger_init(void)
{

	i2c_register_board_info(0, i2c_bq2589x_charger1, ARRAY_SIZE(i2c_bq2589x_charger1));
	//i2c_register_board_info(0, i2c_bq2589x_charger2, ARRAY_SIZE(i2c_bq2589x_charger2));
	/*
	if (i2c_add_driver(&bq2589x_charger2_driver))
		printk("%s, failed to register bq2589x_charger2_driver.\n", __func__);
	else
		printk("%s, bq2589x_charger2_driver register successfully!\n", __func__);
	*/

	if (i2c_add_driver(&bq2589x_charger1_driver))
		printk("%s, failed to register bq2589x_charger1_driver.\n", __func__);
	else
		printk("%s, bq2589x_charger1_driver register successfully!\n", __func__);

	return 0;
}

static void __exit bq2589x_charger_exit(void)
{
	i2c_del_driver(&bq2589x_charger1_driver);
	i2c_del_driver(&bq2589x_charger2_driver);
}

module_init(bq2589x_charger_init);
module_exit(bq2589x_charger_exit);
#endif

MODULE_DESCRIPTION("TI BQ2589x Dual Charger Driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Texas Instruments");
