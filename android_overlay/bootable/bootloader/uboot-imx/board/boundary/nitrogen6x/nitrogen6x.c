/*
 * Copyright (C) 2010-2013 Freescale Semiconductor, Inc.
 * Copyright (C) 2013, Boundary Devices <info@boundarydevices.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/iomux.h>
#include <asm/arch/sys_proto.h>
#include <malloc.h>
#include <asm/arch/mx6-pins.h>
#include <asm/errno.h>
#include <asm/gpio.h>
#include <asm/imx-common/iomux-v3.h>
#include <asm/imx-common/mxc_i2c.h>
#include <asm/imx-common/sata.h>
#include <asm/imx-common/spi.h>
#include <asm/imx-common/boot_mode.h>
#include <asm/imx-common/video.h>
#include <mmc.h>
#include <fsl_esdhc.h>
#include <micrel.h>
#include <miiphy.h>
#include <netdev.h>
#include <asm/arch/crm_regs.h>
#include <asm/arch/mxc_hdmi.h>
#include <i2c.h>
#include <input.h>
#include <netdev.h>
#include <usb/ehci-fsl.h>

#ifdef CONFIG_UBOOT_LOGO_ENABLE
#include <asm/imx-common/imx_pwm.h>
#include <asm/imx-common/mxc_ipu.h>
#endif

#ifdef CONFIG_FSL_FASTBOOT
#include <fsl_fastboot.h>
#ifdef CONFIG_ANDROID_RECOVERY
#include <recovery.h>
#endif
#endif /*CONFIG_FSL_FASTBOOT*/

/* via sam */
extern int tc358778xbg_probe(void);
#define LOGO_BAT_X	490
#define LOGO_BAT_Y	316
#define LOGO_BAT_W	100
#define LOGO_BAT_H	230
#define LOGO_BMP_BASE	0x19000000
#define LOGO_MMC_BASE1	0x1C000
#define LOGO_MMC_BASE2	0x20000
#define LOGO_MMC_LEN	0x4000
#define LOGO_DELAY_MS	3000

// #define IMX_GPIO_NR(bank, nr)		(((bank) - 1) * 32 + (nr))
#define MX6Q_DISPRESET_GPIO6_11		IMX_GPIO_NR(6, 11)
#define MX6Q_DISPPWREN_GPIO6_14		IMX_GPIO_NR(6, 14)
#define MX6Q_DISPPWREN_GPIO6_16		IMX_GPIO_NR(6, 16)

extern int read_battery(void);
extern void lcd_set_logo_bmp_addr(unsigned int base);
extern void lcd_draw_boot_logo(unsigned int framebase, int x_resol, int y_resol, int pixelbyte);
extern void power_off(void);
/* via sam end */

DECLARE_GLOBAL_DATA_PTR;
#define GP_USB_OTG_PWR	IMX_GPIO_NR(3, 22)

#define UART_PAD_CTRL  (PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm |			\
	PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

#define USDHC_PAD_CTRL (PAD_CTL_PUS_47K_UP |			\
	PAD_CTL_SPEED_LOW | PAD_CTL_DSE_80ohm |			\
	PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

#define ENET_PAD_CTRL  (PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm | PAD_CTL_HYS)

#define SPI_PAD_CTRL (PAD_CTL_HYS | PAD_CTL_SPEED_MED |		\
	PAD_CTL_DSE_40ohm     | PAD_CTL_SRE_FAST)

#define BUTTON_PAD_CTRL (PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm | PAD_CTL_HYS)

#define I2C_PAD_CTRL	(PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm | PAD_CTL_HYS |	\
	PAD_CTL_ODE | PAD_CTL_SRE_FAST)

#define WEAK_PULLUP	(PAD_CTL_PUS_100K_UP |			\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm | PAD_CTL_HYS |	\
	PAD_CTL_SRE_SLOW)

#define WEAK_PULLDOWN	(PAD_CTL_PUS_100K_DOWN |		\
	PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm |			\
	PAD_CTL_HYS | PAD_CTL_SRE_SLOW)

#define OUTPUT_40OHM (PAD_CTL_SPEED_MED|PAD_CTL_DSE_40ohm)

#define GPIO_PSR	0x00000008	/* pad status register */

int dram_init(void)
{
	unsigned int sel = readl(GPIO2_BASE_ADDR + GPIO_PSR) & 0x3;

	gd->ram_size = ((ulong)CONFIG_DDR_MB * 1024 * 1024);

	switch (sel) {
	case 2:
		gd->ram_size = (2ul << 30);
		break;
	case 3:
		gd->ram_size = (1ul << 30);
		break;
	default:
		break;
	}

	return 0;
}

static iomux_v3_cfg_t const uart1_pads[] = {
	MX6_PAD_SD3_DAT6__UART1_RX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_SD3_DAT7__UART1_TX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
};

static iomux_v3_cfg_t const uart2_pads[] = {
	MX6_PAD_EIM_D26__UART2_TX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_EIM_D27__UART2_RX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL),
};

#define PC MUX_PAD_CTRL(I2C_PAD_CTRL)

/* I2C1, SGTL5000 */
static struct i2c_pads_info i2c_pad_info0 = {
	.scl = {
		.i2c_mode = MX6_PAD_EIM_D21__I2C1_SCL | PC,
		.gpio_mode = MX6_PAD_EIM_D21__GPIO3_IO21 | PC,
		.gp = IMX_GPIO_NR(3, 21)
	},
	.sda = {
		.i2c_mode = MX6_PAD_EIM_D28__I2C1_SDA | PC,
		.gpio_mode = MX6_PAD_EIM_D28__GPIO3_IO28 | PC,
		.gp = IMX_GPIO_NR(3, 28)
	}
};

/* I2C2 Camera, MIPI */
static struct i2c_pads_info i2c_pad_info1 = {
	.scl = {
		.i2c_mode = MX6_PAD_KEY_COL3__I2C2_SCL | PC,
		.gpio_mode = MX6_PAD_KEY_COL3__GPIO4_IO12 | PC,
		.gp = IMX_GPIO_NR(4, 12)
	},
	.sda = {
		.i2c_mode = MX6_PAD_KEY_ROW3__I2C2_SDA | PC,
		.gpio_mode = MX6_PAD_KEY_ROW3__GPIO4_IO13 | PC,
		.gp = IMX_GPIO_NR(4, 13)
	}
};

/* I2C3, J15 - RGB connector */
static struct i2c_pads_info i2c_pad_info2 = {
	.scl = {
		.i2c_mode = MX6_PAD_GPIO_3__I2C3_SCL | PC,
		.gpio_mode = MX6_PAD_GPIO_3__GPIO1_IO03 | PC,
		.gp = IMX_GPIO_NR(1, 3)
	},
	.sda = {
		.i2c_mode = MX6_PAD_GPIO_6__I2C3_SDA | PC,
		.gpio_mode = MX6_PAD_GPIO_6__GPIO1_IO06 | PC,
		.gp = IMX_GPIO_NR(1, 6)
	}
};

static iomux_v3_cfg_t const usdhc2_pads[] = {
	MX6_PAD_SD2_CLK__SD2_CLK   | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_CMD__SD2_CMD   | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT0__SD2_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT1__SD2_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT2__SD2_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD2_DAT3__SD2_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NANDF_D2__GPIO2_IO02    | MUX_PAD_CTRL(NO_PAD_CTRL), /* CD */
};

static iomux_v3_cfg_t const usdhc3_pads[] = {
	MX6_PAD_SD3_CLK__SD3_CLK   | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_CMD__SD3_CMD   | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT0__SD3_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT1__SD3_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT2__SD3_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT3__SD3_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD3_DAT5__GPIO7_IO00    | MUX_PAD_CTRL(NO_PAD_CTRL), /* CD */
};

static iomux_v3_cfg_t const usdhc4_pads[] = {
	MX6_PAD_SD4_CLK__SD4_CLK   | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_CMD__SD4_CMD   | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT0__SD4_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT1__SD4_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT2__SD4_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT3__SD4_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT4__SD4_DATA4 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT5__SD4_DATA5 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT6__SD4_DATA6 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD4_DAT7__SD4_DATA7 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
};

static iomux_v3_cfg_t const enet_pads1[] = {
	MX6_PAD_ENET_MDIO__ENET_MDIO		| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET_MDC__ENET_MDC		| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII_TXC__RGMII_TXC	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII_TD0__RGMII_TD0	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII_TD1__RGMII_TD1	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII_TD2__RGMII_TD2	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII_TD3__RGMII_TD3	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII_TX_CTL__RGMII_TX_CTL	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET_REF_CLK__ENET_TX_CLK	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	/* pin 35 - 1 (PHY_AD2) on reset */
	MX6_PAD_RGMII_RXC__GPIO6_IO30		| MUX_PAD_CTRL(NO_PAD_CTRL),
	/* pin 32 - 1 - (MODE0) all */
	MX6_PAD_RGMII_RD0__GPIO6_IO25		| MUX_PAD_CTRL(NO_PAD_CTRL),
	/* pin 31 - 1 - (MODE1) all */
	MX6_PAD_RGMII_RD1__GPIO6_IO27		| MUX_PAD_CTRL(NO_PAD_CTRL),
	/* pin 28 - 1 - (MODE2) all */
	MX6_PAD_RGMII_RD2__GPIO6_IO28		| MUX_PAD_CTRL(NO_PAD_CTRL),
	/* pin 27 - 1 - (MODE3) all */
	MX6_PAD_RGMII_RD3__GPIO6_IO29		| MUX_PAD_CTRL(NO_PAD_CTRL),
	/* pin 33 - 1 - (CLK125_EN) 125Mhz clockout enabled */
	MX6_PAD_RGMII_RX_CTL__GPIO6_IO24	| MUX_PAD_CTRL(NO_PAD_CTRL),
	/* pin 42 PHY nRST */
	MX6_PAD_EIM_D23__GPIO3_IO23		| MUX_PAD_CTRL(NO_PAD_CTRL),
	MX6_PAD_ENET_RXD0__GPIO1_IO27		| MUX_PAD_CTRL(NO_PAD_CTRL),
};

static iomux_v3_cfg_t const enet_pads2[] = {
	MX6_PAD_RGMII_RXC__RGMII_RXC	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII_RD0__RGMII_RD0	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII_RD1__RGMII_RD1	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII_RD2__RGMII_RD2	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII_RD3__RGMII_RD3	| MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_RGMII_RX_CTL__RGMII_RX_CTL	| MUX_PAD_CTRL(ENET_PAD_CTRL),
};

static iomux_v3_cfg_t const misc_pads[] = {
	MX6_PAD_GPIO_1__USB_OTG_ID		| MUX_PAD_CTRL(WEAK_PULLUP),
	MX6_PAD_KEY_COL4__USB_OTG_OC		| MUX_PAD_CTRL(WEAK_PULLUP),
	MX6_PAD_EIM_D30__USB_H1_OC		| MUX_PAD_CTRL(WEAK_PULLUP),
	/* OTG Power enable */
	MX6_PAD_EIM_D22__GPIO3_IO22		| MUX_PAD_CTRL(OUTPUT_40OHM),
};

/* wl1271 pads on nitrogen6x */
static iomux_v3_cfg_t const wl12xx_pads[] = {
	(MX6_PAD_NANDF_CS1__GPIO6_IO14 & ~MUX_PAD_CTRL_MASK)
		| MUX_PAD_CTRL(WEAK_PULLDOWN),
	(MX6_PAD_NANDF_CS2__GPIO6_IO15 & ~MUX_PAD_CTRL_MASK)
		| MUX_PAD_CTRL(OUTPUT_40OHM),
	(MX6_PAD_NANDF_CS3__GPIO6_IO16 & ~MUX_PAD_CTRL_MASK)
		| MUX_PAD_CTRL(OUTPUT_40OHM),
};
#define WL12XX_WL_IRQ_GP	IMX_GPIO_NR(6, 14)
#define WL12XX_WL_ENABLE_GP	IMX_GPIO_NR(6, 15)
#define WL12XX_BT_ENABLE_GP	IMX_GPIO_NR(6, 16)

/* Button assignments for J14 */
static iomux_v3_cfg_t const button_pads[] = {
	/* Menu */
	MX6_PAD_NANDF_D1__GPIO2_IO01	| MUX_PAD_CTRL(BUTTON_PAD_CTRL),
	/* Back */
	MX6_PAD_NANDF_D2__GPIO2_IO02	| MUX_PAD_CTRL(BUTTON_PAD_CTRL),
	/* Labelled Search (mapped to Power under Android) */
	MX6_PAD_NANDF_D3__GPIO2_IO03	| MUX_PAD_CTRL(BUTTON_PAD_CTRL),
	/* Home */
	MX6_PAD_NANDF_D4__GPIO2_IO04	| MUX_PAD_CTRL(BUTTON_PAD_CTRL),
	/* Volume Down */
	MX6_PAD_GPIO_19__GPIO4_IO05	| MUX_PAD_CTRL(BUTTON_PAD_CTRL),
	/* Volume Up */
	MX6_PAD_GPIO_18__GPIO7_IO13	| MUX_PAD_CTRL(BUTTON_PAD_CTRL),
};

static void setup_iomux_enet(void)
{
	gpio_direction_output(IMX_GPIO_NR(3, 23), 0); /* SABRE Lite PHY rst */
	gpio_direction_output(IMX_GPIO_NR(1, 27), 0); /* Nitrogen6X PHY rst */
	gpio_direction_output(IMX_GPIO_NR(6, 30), 1);
	gpio_direction_output(IMX_GPIO_NR(6, 25), 1);
	gpio_direction_output(IMX_GPIO_NR(6, 27), 1);
	gpio_direction_output(IMX_GPIO_NR(6, 28), 1);
	gpio_direction_output(IMX_GPIO_NR(6, 29), 1);
	imx_iomux_v3_setup_multiple_pads(enet_pads1, ARRAY_SIZE(enet_pads1));
	gpio_direction_output(IMX_GPIO_NR(6, 24), 1);

	/* Need delay 10ms according to KSZ9021 spec */
	udelay(1000 * 10);
	gpio_set_value(IMX_GPIO_NR(3, 23), 1); /* SABRE Lite PHY reset */
	gpio_set_value(IMX_GPIO_NR(1, 27), 1); /* Nitrogen6X PHY reset */

	imx_iomux_v3_setup_multiple_pads(enet_pads2, ARRAY_SIZE(enet_pads2));
	udelay(100);	/* Wait 100 us before using mii interface */
}

static iomux_v3_cfg_t const usb_pads[] = {
	MX6_PAD_GPIO_17__GPIO7_IO12 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

static void setup_iomux_uart(void)
{
	imx_iomux_v3_setup_multiple_pads(uart1_pads, ARRAY_SIZE(uart1_pads));
	imx_iomux_v3_setup_multiple_pads(uart2_pads, ARRAY_SIZE(uart2_pads));
}

#ifdef CONFIG_USB_EHCI_MX6
int board_ehci_hcd_init(int port)
{
	imx_iomux_v3_setup_multiple_pads(usb_pads, ARRAY_SIZE(usb_pads));

	/* Reset USB hub */
	gpio_direction_output(IMX_GPIO_NR(7, 12), 0);
	mdelay(2);
	gpio_set_value(IMX_GPIO_NR(7, 12), 1);

	return 0;
}

int board_ehci_power(int port, int on)
{
	if (port)
		return 0;
	gpio_set_value(GP_USB_OTG_PWR, on);
	return 0;
}

#endif

#ifdef CONFIG_FSL_ESDHC
static struct fsl_esdhc_cfg usdhc_cfg[2] = {
	{USDHC4_BASE_ADDR},
	{USDHC2_BASE_ADDR},
};

int board_mmc_getcd(struct mmc *mmc)
{
	struct fsl_esdhc_cfg *cfg = (struct fsl_esdhc_cfg *)mmc->priv;

	if (cfg->esdhc_base == USDHC4_BASE_ADDR)
		return 1; /* non-removable */
	else if (cfg->esdhc_base != USDHC2_BASE_ADDR)
		return 0; /* invalid */

	int gp_cd = IMX_GPIO_NR(2, 2);

	gpio_direction_input(gp_cd);
	return !gpio_get_value(gp_cd);
}

int board_mmc_init(bd_t *bis)
{
	int ret;
	u32 index = 0;

	usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC4_CLK);
	usdhc_cfg[1].sdhc_clk = mxc_get_clock(MXC_ESDHC2_CLK);

	usdhc_cfg[0].max_bus_width = 8;
	usdhc_cfg[1].max_bus_width = 4;

	for (index = 0; index < CONFIG_SYS_FSL_USDHC_NUM; ++index) {
		switch (index) {
		case 0:
			imx_iomux_v3_setup_multiple_pads(
				usdhc4_pads, ARRAY_SIZE(usdhc4_pads));
			break;
		case 1:
		       imx_iomux_v3_setup_multiple_pads(
			       usdhc2_pads, ARRAY_SIZE(usdhc2_pads));
		       break;
		default:
		       printf("Warning: you configured more USDHC controllers"
			       "(%d) then supported by the board (%d)\n",
			       index + 1, CONFIG_SYS_FSL_USDHC_NUM);
		       return -EINVAL;
		}

		ret = fsl_esdhc_initialize(bis, &usdhc_cfg[index]);
		if (ret)
			return ret;
	}

	return 0;
}
#endif

#ifdef CONFIG_MXC_SPI
int board_spi_cs_gpio(unsigned bus, unsigned cs)
{
	return (bus == 0 && cs == 0) ? (IMX_GPIO_NR(3, 19)) : -1;
}

static iomux_v3_cfg_t const ecspi1_pads[] = {
	/* SS1 */
	MX6_PAD_EIM_D19__GPIO3_IO19  | MUX_PAD_CTRL(NO_PAD_CTRL),
	MX6_PAD_EIM_D17__ECSPI1_MISO | MUX_PAD_CTRL(SPI_PAD_CTRL),
	MX6_PAD_EIM_D18__ECSPI1_MOSI | MUX_PAD_CTRL(SPI_PAD_CTRL),
	MX6_PAD_EIM_D16__ECSPI1_SCLK | MUX_PAD_CTRL(SPI_PAD_CTRL),
};

static void setup_spi(void)
{
	imx_iomux_v3_setup_multiple_pads(ecspi1_pads,
					 ARRAY_SIZE(ecspi1_pads));
}
#endif

int board_phy_config(struct phy_device *phydev)
{
	/* min rx data delay */
	ksz9021_phy_extended_write(phydev,
			MII_KSZ9021_EXT_RGMII_RX_DATA_SKEW, 0x0);
	/* min tx data delay */
	ksz9021_phy_extended_write(phydev,
			MII_KSZ9021_EXT_RGMII_TX_DATA_SKEW, 0x0);
	/* max rx/tx clock delay, min rx/tx control */
	ksz9021_phy_extended_write(phydev,
			MII_KSZ9021_EXT_RGMII_CLOCK_SKEW, 0xf0f0);
	if (phydev->drv->config)
		phydev->drv->config(phydev);

	return 0;
}

int board_eth_init(bd_t *bis)
{
	uint32_t base = IMX_FEC_BASE;
	struct mii_dev *bus = NULL;
	struct phy_device *phydev = NULL;
	int ret;

	setup_iomux_enet();

#ifdef CONFIG_FEC_MXC
	bus = fec_get_miibus(base, -1);
	if (!bus)
		return 0;
	/* scan phy 4,5,6,7 */
	phydev = phy_find_by_mask(bus, (0xf << 4), PHY_INTERFACE_MODE_RGMII);
	if (!phydev) {
		free(bus);
		return 0;
	}
	printf("using phy at %d\n", phydev->addr);
	ret  = fec_probe(bis, -1, base, bus, phydev);
	if (ret) {
		printf("FEC MXC: %s:failed\n", __func__);
		free(phydev);
		free(bus);
	}
#endif

#ifdef CONFIG_CI_UDC
	/* For otg ethernet*/
	usb_eth_initialize(bis);
#endif
	return 0;
}

static void setup_buttons(void)
{
	imx_iomux_v3_setup_multiple_pads(button_pads,
					 ARRAY_SIZE(button_pads));
}

#if defined(CONFIG_VIDEO_IPUV3)

static iomux_v3_cfg_t const backlight_pads[] = {
	/* Backlight on RGB connector: J15 */
	MX6_PAD_SD1_DAT3__GPIO1_IO21 | MUX_PAD_CTRL(NO_PAD_CTRL),
#define RGB_BACKLIGHT_GP IMX_GPIO_NR(1, 21)

	/* Backlight on LVDS connector: J6 */
	MX6_PAD_SD1_CMD__GPIO1_IO18 | MUX_PAD_CTRL(NO_PAD_CTRL),
#define LVDS_BACKLIGHT_GP IMX_GPIO_NR(1, 18)
};

static iomux_v3_cfg_t const rgb_pads[] = {
	MX6_PAD_DI0_DISP_CLK__IPU1_DI0_DISP_CLK,
	MX6_PAD_DI0_PIN15__IPU1_DI0_PIN15,
	MX6_PAD_DI0_PIN2__IPU1_DI0_PIN02,
	MX6_PAD_DI0_PIN3__IPU1_DI0_PIN03,
	MX6_PAD_DI0_PIN4__GPIO4_IO20,
	MX6_PAD_DISP0_DAT0__IPU1_DISP0_DATA00,
	MX6_PAD_DISP0_DAT1__IPU1_DISP0_DATA01,
	MX6_PAD_DISP0_DAT2__IPU1_DISP0_DATA02,
	MX6_PAD_DISP0_DAT3__IPU1_DISP0_DATA03,
	MX6_PAD_DISP0_DAT4__IPU1_DISP0_DATA04,
	MX6_PAD_DISP0_DAT5__IPU1_DISP0_DATA05,
	MX6_PAD_DISP0_DAT6__IPU1_DISP0_DATA06,
	MX6_PAD_DISP0_DAT7__IPU1_DISP0_DATA07,
	MX6_PAD_DISP0_DAT8__IPU1_DISP0_DATA08,
	MX6_PAD_DISP0_DAT9__IPU1_DISP0_DATA09,
	MX6_PAD_DISP0_DAT10__IPU1_DISP0_DATA10,
	MX6_PAD_DISP0_DAT11__IPU1_DISP0_DATA11,
	MX6_PAD_DISP0_DAT12__IPU1_DISP0_DATA12,
	MX6_PAD_DISP0_DAT13__IPU1_DISP0_DATA13,
	MX6_PAD_DISP0_DAT14__IPU1_DISP0_DATA14,
	MX6_PAD_DISP0_DAT15__IPU1_DISP0_DATA15,
	MX6_PAD_DISP0_DAT16__IPU1_DISP0_DATA16,
	MX6_PAD_DISP0_DAT17__IPU1_DISP0_DATA17,
	MX6_PAD_DISP0_DAT18__IPU1_DISP0_DATA18,
	MX6_PAD_DISP0_DAT19__IPU1_DISP0_DATA19,
	MX6_PAD_DISP0_DAT20__IPU1_DISP0_DATA20,
	MX6_PAD_DISP0_DAT21__IPU1_DISP0_DATA21,
	MX6_PAD_DISP0_DAT22__IPU1_DISP0_DATA22,
	MX6_PAD_DISP0_DAT23__IPU1_DISP0_DATA23,
};

#ifdef CONFIG_IMX_HDMI
static void do_enable_hdmi(struct display_info_t const *dev)
{
	imx_enable_hdmi_phy();
}
#endif

static int detect_i2c(struct display_info_t const *dev)
{
	return ((0 == i2c_set_bus_num(dev->bus))
		&&
		(0 == i2c_probe(dev->addr)));
}

static void enable_lvds(struct display_info_t const *dev)
{
	struct iomuxc *iomux = (struct iomuxc *)
				IOMUXC_BASE_ADDR;
	u32 reg = readl(&iomux->gpr[2]);
	reg |= IOMUXC_GPR2_DATA_WIDTH_CH0_24BIT;
	writel(reg, &iomux->gpr[2]);
	gpio_direction_output(LVDS_BACKLIGHT_GP, 1);
}

static void enable_lvds_jeida(struct display_info_t const *dev)
{
	struct iomuxc *iomux = (struct iomuxc *)
				IOMUXC_BASE_ADDR;
	u32 reg = readl(&iomux->gpr[2]);
	reg |= IOMUXC_GPR2_DATA_WIDTH_CH0_24BIT
	     |IOMUXC_GPR2_BIT_MAPPING_CH0_JEIDA;
	writel(reg, &iomux->gpr[2]);
	gpio_direction_output(LVDS_BACKLIGHT_GP, 1);
}

static void enable_rgb(struct display_info_t const *dev)
{
	imx_iomux_v3_setup_multiple_pads(
		rgb_pads,
		 ARRAY_SIZE(rgb_pads));
	gpio_direction_output(RGB_BACKLIGHT_GP, 1);
}

struct display_info_t const displays[] = {{
#ifdef CONFIG_IMX_HDMI
	.bus	= 1,
	.addr	= 0x50,
	.pixfmt	= IPU_PIX_FMT_RGB24,
	.detect	= detect_i2c,
	.enable	= do_enable_hdmi,
	.mode	= {
		.name           = "HDMI",
		.refresh        = 60,
		.xres           = 1024,
		.yres           = 768,
		.pixclock       = 15385,
		.left_margin    = 220,
		.right_margin   = 40,
		.upper_margin   = 21,
		.lower_margin   = 7,
		.hsync_len      = 60,
		.vsync_len      = 10,
		.sync           = FB_SYNC_EXT,
		.vmode          = FB_VMODE_NONINTERLACED
} }, {
#endif
	.bus	= 0,
	.addr	= 0,
	.pixfmt	= IPU_PIX_FMT_RGB24,
	.detect	= NULL,
	.enable	= enable_lvds_jeida,
	.mode	= {
		.name           = "LDB-WXGA",
		.refresh        = 60,
		.xres           = 1280,
		.yres           = 800,
		.pixclock       = 14065,
		.left_margin    = 40,
		.right_margin   = 40,
		.upper_margin   = 3,
		.lower_margin   = 80,
		.hsync_len      = 10,
		.vsync_len      = 10,
		.sync           = FB_SYNC_EXT,
		.vmode          = FB_VMODE_NONINTERLACED
} }, {
	.bus	= 0,
	.addr	= 0,
	.pixfmt	= IPU_PIX_FMT_RGB24,
	.detect	= NULL,
	.enable	= enable_lvds,
	.mode	= {
		.name           = "LDB-WXGA-S",
		.refresh        = 60,
		.xres           = 1280,
		.yres           = 800,
		.pixclock       = 14065,
		.left_margin    = 40,
		.right_margin   = 40,
		.upper_margin   = 3,
		.lower_margin   = 80,
		.hsync_len      = 10,
		.vsync_len      = 10,
		.sync           = FB_SYNC_EXT,
		.vmode          = FB_VMODE_NONINTERLACED
} }, {
	.bus	= 2,
	.addr	= 0x4,
	.pixfmt	= IPU_PIX_FMT_LVDS666,
	.detect	= detect_i2c,
	.enable	= enable_lvds,
	.mode	= {
		.name           = "Hannstar-XGA",
		.refresh        = 60,
		.xres           = 1024,
		.yres           = 768,
		.pixclock       = 15385,
		.left_margin    = 220,
		.right_margin   = 40,
		.upper_margin   = 21,
		.lower_margin   = 7,
		.hsync_len      = 60,
		.vsync_len      = 10,
		.sync           = FB_SYNC_EXT,
		.vmode          = FB_VMODE_NONINTERLACED
} }, {
	.bus	= 0,
	.addr	= 0,
	.pixfmt	= IPU_PIX_FMT_LVDS666,
	.detect	= NULL,
	.enable	= enable_lvds,
	.mode	= {
		.name           = "LG-9.7",
		.refresh        = 60,
		.xres           = 1024,
		.yres           = 768,
		.pixclock       = 15385, /* ~65MHz */
		.left_margin    = 480,
		.right_margin   = 260,
		.upper_margin   = 16,
		.lower_margin   = 6,
		.hsync_len      = 250,
		.vsync_len      = 10,
		.sync           = FB_SYNC_EXT,
		.vmode          = FB_VMODE_NONINTERLACED
} }, {
	.bus	= 2,
	.addr	= 0x38,
	.pixfmt	= IPU_PIX_FMT_LVDS666,
	.detect	= detect_i2c,
	.enable	= enable_lvds,
	.mode	= {
		.name           = "wsvga-lvds",
		.refresh        = 60,
		.xres           = 1024,
		.yres           = 600,
		.pixclock       = 15385,
		.left_margin    = 220,
		.right_margin   = 40,
		.upper_margin   = 21,
		.lower_margin   = 7,
		.hsync_len      = 60,
		.vsync_len      = 10,
		.sync           = FB_SYNC_EXT,
		.vmode          = FB_VMODE_NONINTERLACED
} }, {
	.bus	= 2,
	.addr	= 0x10,
	.pixfmt	= IPU_PIX_FMT_RGB666,
	.detect	= detect_i2c,
	.enable	= enable_rgb,
	.mode	= {
		.name           = "fusion7",
		.refresh        = 60,
		.xres           = 800,
		.yres           = 480,
		.pixclock       = 33898,
		.left_margin    = 96,
		.right_margin   = 24,
		.upper_margin   = 3,
		.lower_margin   = 10,
		.hsync_len      = 72,
		.vsync_len      = 7,
		.sync           = 0x40000002,
		.vmode          = FB_VMODE_NONINTERLACED
} }, {
	.bus	= 0,
	.addr	= 0,
	.pixfmt	= IPU_PIX_FMT_RGB666,
	.detect	= NULL,
	.enable	= enable_rgb,
	.mode	= {
		.name           = "svga",
		.refresh        = 60,
		.xres           = 800,
		.yres           = 600,
		.pixclock       = 15385,
		.left_margin    = 220,
		.right_margin   = 40,
		.upper_margin   = 21,
		.lower_margin   = 7,
		.hsync_len      = 60,
		.vsync_len      = 10,
		.sync           = 0,
		.vmode          = FB_VMODE_NONINTERLACED
} }, {
	.bus	= 2,
	.addr	= 0x41,
	.pixfmt	= IPU_PIX_FMT_LVDS666,
	.detect	= detect_i2c,
	.enable	= enable_lvds,
	.mode	= {
		.name           = "amp1024x600",
		.refresh        = 60,
		.xres           = 1024,
		.yres           = 600,
		.pixclock       = 15385,
		.left_margin    = 220,
		.right_margin   = 40,
		.upper_margin   = 21,
		.lower_margin   = 7,
		.hsync_len      = 60,
		.vsync_len      = 10,
		.sync           = FB_SYNC_EXT,
		.vmode          = FB_VMODE_NONINTERLACED
} }, {
	.bus	= 0,
	.addr	= 0,
	.pixfmt	= IPU_PIX_FMT_LVDS666,
	.detect	= 0,
	.enable	= enable_lvds,
	.mode	= {
		.name           = "wvga-lvds",
		.refresh        = 57,
		.xres           = 800,
		.yres           = 480,
		.pixclock       = 15385,
		.left_margin    = 220,
		.right_margin   = 40,
		.upper_margin   = 21,
		.lower_margin   = 7,
		.hsync_len      = 60,
		.vsync_len      = 10,
		.sync           = FB_SYNC_EXT,
		.vmode          = FB_VMODE_NONINTERLACED
} }, {
	.bus	= 2,
	.addr	= 0x48,
	.pixfmt	= IPU_PIX_FMT_RGB666,
	.detect	= detect_i2c,
	.enable	= enable_rgb,
	.mode	= {
		.name           = "wvga-rgb",
		.refresh        = 57,
		.xres           = 800,
		.yres           = 480,
		.pixclock       = 37037,
		.left_margin    = 40,
		.right_margin   = 60,
		.upper_margin   = 10,
		.lower_margin   = 10,
		.hsync_len      = 20,
		.vsync_len      = 10,
		.sync           = 0,
		.vmode          = FB_VMODE_NONINTERLACED
} }, {
	.bus	= 0,
	.addr	= 0,
	.pixfmt	= IPU_PIX_FMT_RGB24,
	.detect	= NULL,
	.enable	= enable_rgb,
	.mode	= {
		.name           = "qvga",
		.refresh        = 60,
		.xres           = 320,
		.yres           = 240,
		.pixclock       = 37037,
		.left_margin    = 38,
		.right_margin   = 37,
		.upper_margin   = 16,
		.lower_margin   = 15,
		.hsync_len      = 30,
		.vsync_len      = 3,
		.sync           = 0,
		.vmode          = FB_VMODE_NONINTERLACED
} } };
size_t display_count = ARRAY_SIZE(displays);

int board_cfb_skip(void)
{
	return NULL != getenv("novideo");
}

static void setup_display(void)
{
	struct mxc_ccm_reg *mxc_ccm = (struct mxc_ccm_reg *)CCM_BASE_ADDR;
	struct iomuxc *iomux = (struct iomuxc *)IOMUXC_BASE_ADDR;
	int reg;

	enable_ipu_clock();
#ifdef CONFIG_IMX_HDMI
	imx_setup_hdmi();
#endif
	/* Turn on LDB0,IPU,IPU DI0 clocks */
	reg = __raw_readl(&mxc_ccm->CCGR3);
	reg |=  MXC_CCM_CCGR3_LDB_DI0_MASK;
	writel(reg, &mxc_ccm->CCGR3);

	/* set LDB0, LDB1 clk select to 011/011 */
	reg = readl(&mxc_ccm->cs2cdr);
	reg &= ~(MXC_CCM_CS2CDR_LDB_DI0_CLK_SEL_MASK
		 |MXC_CCM_CS2CDR_LDB_DI1_CLK_SEL_MASK);
	reg |= (3<<MXC_CCM_CS2CDR_LDB_DI0_CLK_SEL_OFFSET)
	      |(3<<MXC_CCM_CS2CDR_LDB_DI1_CLK_SEL_OFFSET);
	writel(reg, &mxc_ccm->cs2cdr);

	reg = readl(&mxc_ccm->cscmr2);
	reg |= MXC_CCM_CSCMR2_LDB_DI0_IPU_DIV;
	writel(reg, &mxc_ccm->cscmr2);

	reg = readl(&mxc_ccm->chsccdr);
	reg |= (CHSCCDR_CLK_SEL_LDB_DI0
		<<MXC_CCM_CHSCCDR_IPU1_DI0_CLK_SEL_OFFSET);
	writel(reg, &mxc_ccm->chsccdr);

	reg = IOMUXC_GPR2_BGREF_RRMODE_EXTERNAL_RES
	     |IOMUXC_GPR2_DI1_VS_POLARITY_ACTIVE_HIGH
	     |IOMUXC_GPR2_DI0_VS_POLARITY_ACTIVE_LOW
	     |IOMUXC_GPR2_BIT_MAPPING_CH1_SPWG
	     |IOMUXC_GPR2_DATA_WIDTH_CH1_18BIT
	     |IOMUXC_GPR2_BIT_MAPPING_CH0_SPWG
	     |IOMUXC_GPR2_DATA_WIDTH_CH0_18BIT
	     |IOMUXC_GPR2_LVDS_CH1_MODE_DISABLED
	     |IOMUXC_GPR2_LVDS_CH0_MODE_ENABLED_DI0;
	writel(reg, &iomux->gpr[2]);

	reg = readl(&iomux->gpr[3]);
	reg = (reg & ~(IOMUXC_GPR3_LVDS0_MUX_CTL_MASK
			|IOMUXC_GPR3_HDMI_MUX_CTL_MASK))
	    | (IOMUXC_GPR3_MUX_SRC_IPU1_DI0
	       <<IOMUXC_GPR3_LVDS0_MUX_CTL_OFFSET);
	writel(reg, &iomux->gpr[3]);

	/* backlights off until needed */
	imx_iomux_v3_setup_multiple_pads(backlight_pads,
					 ARRAY_SIZE(backlight_pads));
	gpio_direction_input(LVDS_BACKLIGHT_GP);
	gpio_direction_input(RGB_BACKLIGHT_GP);
}
#endif

#ifdef CONFIG_UBOOT_LOGO_ENABLE
#ifdef IPU_OUTPUT_MODE_LCD
static void ipu_iomux_config(void)
{
	iomux_v3_cfg_t display_pads[] = {
		MX6_PAD_DI0_DISP_CLK__IPU1_DI0_DISP_CLK | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6_PAD_DI0_PIN15__IPU1_DI0_PIN15 | MUX_PAD_CTRL(NO_PAD_CTRL), 	/* DE */
		MX6_PAD_DI0_PIN2__IPU1_DI0_PIN02 | MUX_PAD_CTRL(NO_PAD_CTRL),		/* HSync */
		MX6_PAD_DI0_PIN3__IPU1_DI0_PIN03 | MUX_PAD_CTRL(NO_PAD_CTRL),		/* VSync */
		MX6_PAD_DI0_PIN4__IPU1_DI0_PIN04 | MUX_PAD_CTRL(NO_PAD_CTRL),
		// MX6_PAD_DI0_PIN4__GPIO4_IO20 | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6_PAD_DISP0_DAT0__IPU1_DISP0_DATA00 | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6_PAD_DISP0_DAT1__IPU1_DISP0_DATA01 | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6_PAD_DISP0_DAT2__IPU1_DISP0_DATA02 | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6_PAD_DISP0_DAT3__IPU1_DISP0_DATA03 | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6_PAD_DISP0_DAT4__IPU1_DISP0_DATA04 | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6_PAD_DISP0_DAT5__IPU1_DISP0_DATA05 | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6_PAD_DISP0_DAT6__IPU1_DISP0_DATA06 | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6_PAD_DISP0_DAT7__IPU1_DISP0_DATA07 | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6_PAD_DISP0_DAT8__IPU1_DISP0_DATA08 | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6_PAD_DISP0_DAT9__IPU1_DISP0_DATA09 | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6_PAD_DISP0_DAT10__IPU1_DISP0_DATA10 | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6_PAD_DISP0_DAT11__IPU1_DISP0_DATA11 | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6_PAD_DISP0_DAT12__IPU1_DISP0_DATA12 | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6_PAD_DISP0_DAT13__IPU1_DISP0_DATA13 | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6_PAD_DISP0_DAT14__IPU1_DISP0_DATA14 | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6_PAD_DISP0_DAT15__IPU1_DISP0_DATA15 | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6_PAD_DISP0_DAT16__IPU1_DISP0_DATA16 | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6_PAD_DISP0_DAT17__IPU1_DISP0_DATA17 | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6_PAD_DISP0_DAT18__IPU1_DISP0_DATA18 | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6_PAD_DISP0_DAT19__IPU1_DISP0_DATA19 | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6_PAD_DISP0_DAT20__IPU1_DISP0_DATA20 | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6_PAD_DISP0_DAT21__IPU1_DISP0_DATA21 | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6_PAD_DISP0_DAT22__IPU1_DISP0_DATA22 | MUX_PAD_CTRL(NO_PAD_CTRL),
		MX6_PAD_DISP0_DAT23__IPU1_DISP0_DATA23 | MUX_PAD_CTRL(NO_PAD_CTRL),
	};

	imx_iomux_v3_setup_multiple_pads(display_pads,
			ARRAY_SIZE(display_pads));
}
static void setup_lcd_iomux(void)
{
	struct pwm_device pwm = {
		.pwm_id = 0,
		.pwmo_invert = 0,
	};

	imx_pwm_config(pwm, 25000, 50000);
	imx_pwm_enable(pwm);

	/* LCD Power */
	imx_iomux_v3_setup_pad(MX6_PAD_NANDF_CS3__GPIO6_IO16 | MUX_PAD_CTRL(NO_PAD_CTRL));
	imx_iomux_v3_setup_pad(MX6_PAD_NANDF_CS1__GPIO6_IO14 | MUX_PAD_CTRL(NO_PAD_CTRL));	
	/* LCD reset */
	imx_iomux_v3_setup_pad(MX6_PAD_NANDF_CS0__GPIO6_IO11 | MUX_PAD_CTRL(NO_PAD_CTRL));
	/* Backlight */
	imx_iomux_v3_setup_pad(MX6_PAD_GPIO_9__PWM1_OUT | MUX_PAD_CTRL(NO_PAD_CTRL));

	
	/* display VCC power enable */
//	gpio_request(MX6Q_DISPPWREN_GPIO6_16, "gpio6_16");
	gpio_direction_output(MX6Q_DISPPWREN_GPIO6_16, 1);	/* display power enable */	
//	gpio_request(MX6Q_DISPPWREN_GPIO6_14, "gpio6_14");	
	gpio_direction_output(MX6Q_DISPPWREN_GPIO6_14, 1);	/* display reset pin */ 
//	gpio_request(MX6Q_DISPRESET_GPIO6_11, "gpio6_11");	
	gpio_direction_output(MX6Q_DISPRESET_GPIO6_11, 0);	
	mdelay(1);	
	gpio_direction_output(MX6Q_DISPRESET_GPIO6_11, 1);
	/* backlight */
	gpio_direction_output(IMX_GPIO_NR(6, 9), 1);	
	{
		extern int tc358778xbg_check_ID(void);
	
		tc358778xbg_check_ID();
	}

}
#endif
#endif

static iomux_v3_cfg_t const init_pads[] = {
	/* SGTL5000 sys_mclk */
	NEW_PAD_CTRL(MX6_PAD_GPIO_0__CCM_CLKO1, OUTPUT_40OHM),

	/* J5 - Camera MCLK */
	NEW_PAD_CTRL(MX6_PAD_GPIO_3__CCM_CLKO2, OUTPUT_40OHM),

	/* wl1271 pads on nitrogen6x */
	/* WL12XX_WL_IRQ_GP */
	NEW_PAD_CTRL(MX6_PAD_NANDF_CS1__GPIO6_IO14, WEAK_PULLDOWN),
	/* WL12XX_WL_ENABLE_GP */
	NEW_PAD_CTRL(MX6_PAD_NANDF_CS2__GPIO6_IO15, OUTPUT_40OHM),
	/* WL12XX_BT_ENABLE_GP */
	NEW_PAD_CTRL(MX6_PAD_NANDF_CS3__GPIO6_IO16, OUTPUT_40OHM),
	/* USB otg power */
	NEW_PAD_CTRL(MX6_PAD_EIM_D22__GPIO3_IO22, OUTPUT_40OHM),
	NEW_PAD_CTRL(MX6_PAD_NANDF_D5__GPIO2_IO05, OUTPUT_40OHM),
	NEW_PAD_CTRL(MX6_PAD_NANDF_WP_B__GPIO6_IO09, OUTPUT_40OHM),
	NEW_PAD_CTRL(MX6_PAD_GPIO_8__GPIO1_IO08, OUTPUT_40OHM),
	NEW_PAD_CTRL(MX6_PAD_GPIO_6__GPIO1_IO06, OUTPUT_40OHM),
};

#define WL12XX_WL_IRQ_GP	IMX_GPIO_NR(6, 14)

static unsigned gpios_out_low[] = {
	/* Disable wl1271 */
	IMX_GPIO_NR(6, 15),	/* disable wireless */
	IMX_GPIO_NR(6, 16),	/* disable bluetooth */
	IMX_GPIO_NR(3, 22),	/* disable USB otg power */
	IMX_GPIO_NR(2, 5),	/* ov5640 mipi camera reset */
	IMX_GPIO_NR(1, 8),	/* ov5642 reset */
};

static unsigned gpios_out_high[] = {
	IMX_GPIO_NR(1, 6),	/* ov5642 powerdown */
	IMX_GPIO_NR(6, 9),	/* ov5640 mipi camera power down */
};

static void set_gpios(unsigned *p, int cnt, int val)
{
	int i;

	for (i = 0; i < cnt; i++)
		gpio_direction_output(*p++, val);
}

int board_early_init_f(void)
{
	setup_iomux_uart();

	set_gpios(gpios_out_high, ARRAY_SIZE(gpios_out_high), 1);
	set_gpios(gpios_out_low, ARRAY_SIZE(gpios_out_low), 0);
	gpio_direction_input(WL12XX_WL_IRQ_GP);

	imx_iomux_v3_setup_multiple_pads(wl12xx_pads, ARRAY_SIZE(wl12xx_pads));
	imx_iomux_v3_setup_multiple_pads(init_pads, ARRAY_SIZE(init_pads));
	setup_buttons();

#if defined(CONFIG_VIDEO_IPUV3)
	setup_display();
#endif
	return 0;
}

#if 0
/*
 * Do not overwrite the console
 * Use always serial for U-Boot console
 */
int overwrite_console(void)
{
	return 1;
}
#endif

int board_init(void)
{
	struct iomuxc *const iomuxc_regs = (struct iomuxc *)IOMUXC_BASE_ADDR;

	clrsetbits_le32(&iomuxc_regs->gpr[1],
			IOMUXC_GPR1_OTG_ID_MASK,
			IOMUXC_GPR1_OTG_ID_GPIO1);

	imx_iomux_v3_setup_multiple_pads(misc_pads, ARRAY_SIZE(misc_pads));

	/* address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;

#ifdef CONFIG_MXC_SPI
	setup_spi();
#endif
	imx_iomux_v3_setup_multiple_pads(
		usdhc2_pads, ARRAY_SIZE(usdhc2_pads));
	setup_i2c(0, CONFIG_SYS_I2C_SPEED, 0x7f, &i2c_pad_info0);
	setup_i2c(1, CONFIG_SYS_I2C_SPEED, 0x7f, &i2c_pad_info1);
	setup_i2c(2, CONFIG_SYS_I2C_SPEED, 0x7f, &i2c_pad_info2);

#ifdef CONFIG_CMD_SATA
	setup_sata();
#endif

	return 0;
}

int checkboard(void)
{
	if (gpio_get_value(WL12XX_WL_IRQ_GP))
		puts("Board: Nitrogen6X\n");
	else
		puts("Board: SABRE Lite\n");

	return 0;
}

struct button_key {
	char const	*name;
	unsigned	gpnum;
	char		ident;
};

static struct button_key const buttons[] = {
	{"back",	IMX_GPIO_NR(2, 2),	'B'},
	{"home",	IMX_GPIO_NR(2, 4),	'H'},
	{"menu",	IMX_GPIO_NR(2, 1),	'M'},
	{"search",	IMX_GPIO_NR(2, 3),	'S'},
	{"volup",	IMX_GPIO_NR(7, 13),	'V'},
	{"voldown",	IMX_GPIO_NR(4, 5),	'v'},
};

/*
 * generate a null-terminated string containing the buttons pressed
 * returns number of keys pressed
 */
static int read_keys(char *buf)
{
	int i, numpressed = 0;
	for (i = 0; i < ARRAY_SIZE(buttons); i++) {
		if (!gpio_get_value(buttons[i].gpnum))
			buf[numpressed++] = buttons[i].ident;
	}
	buf[numpressed] = '\0';
	return numpressed;
}

static int do_kbd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char envvalue[ARRAY_SIZE(buttons)+1];
	int numpressed = read_keys(envvalue);
	setenv("keybd", envvalue);
	return numpressed == 0;
}

U_BOOT_CMD(
	kbd, 1, 1, do_kbd,
	"Tests for keypresses, sets 'keybd' environment variable",
	"Returns 0 (true) to shell if key is pressed."
);

#ifdef CONFIG_PREBOOT
static char const kbd_magic_prefix[] = "key_magic";
static char const kbd_command_prefix[] = "key_cmd";

static void preboot_keys(void)
{
	int numpressed;
	char keypress[ARRAY_SIZE(buttons)+1];
	numpressed = read_keys(keypress);
	if (numpressed) {
		char *kbd_magic_keys = getenv("magic_keys");
		char *suffix;
		/*
		 * loop over all magic keys
		 */
		for (suffix = kbd_magic_keys; *suffix; ++suffix) {
			char *keys;
			char magic[sizeof(kbd_magic_prefix) + 1];
			sprintf(magic, "%s%c", kbd_magic_prefix, *suffix);
			keys = getenv(magic);
			if (keys) {
				if (!strcmp(keys, keypress))
					break;
			}
		}
		if (*suffix) {
			char cmd_name[sizeof(kbd_command_prefix) + 1];
			char *cmd;
			sprintf(cmd_name, "%s%c", kbd_command_prefix, *suffix);
			cmd = getenv(cmd_name);
			if (cmd) {
				setenv("preboot", cmd);
				return;
			}
		}
	}
}
#endif

#ifdef CONFIG_LDO_BYPASS_CHECK
void ldo_mode_set(int ldo_bypass)
{
	if (ldo_bypass)
		printf("nitrogen6x.c: ldo_mode_set was not implemented!\n");
}
#endif

#ifdef CONFIG_CMD_BMODE
static const struct boot_mode board_boot_modes[] = {
	/* 4 bit bus width */
	{"mmc0",	MAKE_CFGVAL(0x40, 0x38, 0x00, 0x00)},
	{"mmc1",	MAKE_CFGVAL(0x40, 0x28, 0x00, 0x00)},
	{NULL,		0},
};
#endif

int mmc_get_env_devno(void)
{
	u32 soc_sbmr = readl(SRC_BASE_ADDR + 0x4);
	u32 dev_no;
	u32 bootsel;

	bootsel = (soc_sbmr & 0x000000FF) >> 6 ;

	/* If not boot from sd/mmc, use default value */
	if (bootsel != 1)
		return 0; // CONFIG_SYS_MMC_ENV_DEV;

	/* BOOT_CFG2[3] and BOOT_CFG2[4] */
	dev_no = (soc_sbmr & 0x00001800) >> 11;

	/* need ubstract 1 to map to the mmc device id
	 * see the comments in board_mmc_init function
	 */

	dev_no--;

	return dev_no;
}

void fill_rectangle(unsigned int addr, unsigned int x,
	unsigned int y, unsigned int w, unsigned int h, unsigned int color)
{
	unsigned int line_pxl, fb_pxl, fb_xoft, fb_yoft;
	unsigned int *rgb_ptr;
	unsigned int i, j;
	unsigned int size;

	// printf("w %d,h %d,x %d,y %d,color 0x%x\n", w, h, x, y, color);

	fb_pxl = DISPLAY_WIDTH;
	line_pxl = DISPLAY_WIDTH;
	fb_xoft = 0;
	fb_yoft = 0;

	rgb_ptr = (unsigned int *) (addr + (fb_xoft + x) * 4 +
		(fb_yoft + y) * line_pxl * 4);
	// printf("base 0x%x, addr 0x%x\n", addr, rgb_ptr);
	
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++)
			*(rgb_ptr + line_pxl * i + j) = color;
	}
	size = DISPLAY_WIDTH * DISPLAY_HEIGHT * DISPLAY_BPP / 8;
	flush_dcache_range(CONFIG_FB_BASE, CONFIG_FB_BASE + size);	
}

void draw_battery_level(int battery_level)
{
	int i, level;
	int y, h;
	unsigned int addr = CONFIG_FB_BASE;

	if (battery_level > 100)
		battery_level = 100;

	level = battery_level;
	for (i = 0; i < 4; i++) {
		h = 2 * ((level >= 25) ? 25 : level);
		
		if (level < 25) {
			int y1;

			y1 = LOGO_BAT_Y + LOGO_BAT_H - (60 * (i + 1)) + 10;
			fill_rectangle(addr, LOGO_BAT_X, y1, LOGO_BAT_W, 50 - h, 0x0);
		}
		
		if (level > 0) {
			y = LOGO_BAT_Y + LOGO_BAT_H - (60 * i) - h;
			fill_rectangle(addr, LOGO_BAT_X, y, LOGO_BAT_W, h, 0xff00);
		}
		level -= (level > 25) ? 25 : level;
	}
}

int misc_init_r(void)
{
#ifdef CONFIG_UBOOT_LOGO_ENABLE
	unsigned int size = DISPLAY_WIDTH * DISPLAY_HEIGHT * (DISPLAY_BPP / 8);
	unsigned char * pData;
	int i, bmpReady = 0;
	int mmc_dev = mmc_get_env_devno();
	struct mmc *mmc = find_mmc_device(mmc_dev);
	int battery_level;
	/* ulong poweroff_level, bat_logo_delay; */

	pData = (unsigned char *)CONFIG_FB_BASE;

	if (mmc)	{
		if (mmc_init(mmc) == 0) {
			mmc->block_dev.block_read(mmc_dev, LOGO_MMC_BASE1, LOGO_MMC_LEN, LOGO_BMP_BASE);
			bmpReady = 1;
			lcd_set_logo_bmp_addr(LOGO_BMP_BASE);
			lcd_draw_boot_logo(CONFIG_FB_BASE, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_BPP / 8);
			battery_level = read_battery();
			draw_battery_level(battery_level);
		}
	}

	if (bmpReady == 0) {
		fill_rectangle(pData,0,0,1080,1920/3,0xFF);
		fill_rectangle(pData,0,1920/3,1080,1920/3,0xFF00);
		fill_rectangle(pData,0,(1920*2)/3,1080,1920/3,0xFF0000);
	}

#ifndef CONFIG_SYS_DCACHE_OFF
	flush_dcache_range((u32)pData, (u32)(pData + DISPLAY_WIDTH * DISPLAY_HEIGHT * (DISPLAY_BPP / 8)));
#endif

#ifdef IPU_OUTPUT_MODE_LVDS
	setup_lvds_iomux();
#endif

#ifdef IPU_OUTPUT_MODE_LCD
	ipu_iomux_config();
	setup_lcd_iomux();
#endif

#ifdef IPU_OUTPUT_MODE_HDMI
	setup_hdmi_iomux();
#endif

	ipu_display_setup();
	tc358778xbg_probe();

	/* check battery status */
	/* printf("check battery status, if too low then power off\n"); */
	/* bat_logo_delay = getenv_hex("low_bat_delay", 0); */
	/* poweroff_level = getenv_hex("low_bat", 0); */
	if (read_battery() < /*poweroff_level*/ 3) {
		mdelay(/*bat_logo_delay*/ 2000);
		/* power off */
		/* avoid value too large, push button to jump that */
		if (gpio_get_value(IMX_GPIO_NR(3, 29)))
			power_off();
	}

	if (mmc)	{
		mdelay(LOGO_DELAY_MS);
		mmc->block_dev.block_read(mmc_dev, LOGO_MMC_BASE2, LOGO_MMC_LEN, LOGO_BMP_BASE);
		lcd_set_logo_bmp_addr(LOGO_BMP_BASE);
		lcd_draw_boot_logo(CONFIG_FB_BASE, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_BPP / 8);
	}
#endif

#ifdef CONFIG_PREBOOT
	preboot_keys();
#endif

#ifdef CONFIG_CMD_BMODE
	add_board_boot_modes(board_boot_modes);
#endif
	setenv_hex("reset_cause", get_imx_reset_cause());
	return 0;
}

#ifdef CONFIG_FSL_FASTBOOT

void board_fastboot_setup(void)
{
	if (!getenv("bspinstcmd"))
		setenv("bspinstcmd", "fatload mmc 1 20000000 bspinst/bspinst.img; boota 20000000");

	switch (get_boot_device()) {
#if defined(CONFIG_FASTBOOT_STORAGE_MMC)
	case SD1_BOOT:
	case MMC1_BOOT:
	    if (!getenv("fastboot_dev"))
			setenv("fastboot_dev", "mmc0");
	    if (!getenv("bootcmd"))
			setenv("bootcmd", "boota mmc0");
	    break;
	case SD2_BOOT:
	case MMC2_BOOT:
	    if (!getenv("fastboot_dev"))
			setenv("fastboot_dev", "mmc1");
	    if (!getenv("bootcmd"))
			setenv("bootcmd", "run bspinstcmd; boota mmc1");
	    break;
	case SD3_BOOT:
	case MMC3_BOOT:
	    if (!getenv("fastboot_dev"))
			setenv("fastboot_dev", "mmc2");
	    if (!getenv("bootcmd"))
			setenv("bootcmd", "boota mmc2");
	    break;
	case MMC4_BOOT:
	    if (!getenv("fastboot_dev"))
			setenv("fastboot_dev", "mmc3");
	    if (!getenv("bootcmd"))
			setenv("bootcmd", "boota mmc3");
	    break;
#endif /*CONFIG_FASTBOOT_STORAGE_MMC*/
	case SPI_NOR_BOOT:
	    if (!getenv("fastboot_dev"))
			setenv("fastboot_dev", "mmc0");
	    if (!getenv("bootcmd"))
			setenv("bootcmd", "run bspinstcmd; boota mmc0");
	    break;
	default:
		printf("unsupported boot devices\n");
		break;
	}

}

#ifdef CONFIG_ANDROID_RECOVERY

#define GPIO_VOL_DN_KEY IMX_GPIO_NR(1, 5)
iomux_v3_cfg_t const recovery_key_pads[] = {
	(MX6_PAD_GPIO_5__GPIO1_IO05 | MUX_PAD_CTRL(NO_PAD_CTRL)),
};

int check_recovery_cmd_file(void)
{
    int button_pressed = 0;
    int recovery_mode = 0;

    recovery_mode = recovery_check_and_clean_flag();

#if 0
    /* Check Recovery Combo Button press or not. */
	imx_iomux_v3_setup_multiple_pads(recovery_key_pads,
			ARRAY_SIZE(recovery_key_pads));

    gpio_direction_input(GPIO_VOL_DN_KEY);

    if (gpio_get_value(GPIO_VOL_DN_KEY) == 0) { /* VOL_DN key is low assert */
		button_pressed = 1;
		printf("Recovery key pressed\n");
    }
#endif

    return recovery_mode || button_pressed;
}

void board_recovery_setup(void)
{
	int bootdev = get_boot_device();

	switch (bootdev) {
#if defined(CONFIG_FASTBOOT_STORAGE_MMC)
	case SD1_BOOT:
	case MMC1_BOOT:
		if (!getenv("bootcmd_android_recovery"))
			setenv("bootcmd_android_recovery",
				"boota mmc0 recovery");
		break;
	case SD2_BOOT:
	case MMC2_BOOT:
		if (!getenv("bootcmd_android_recovery"))
			setenv("bootcmd_android_recovery",
				"boota mmc1 recovery");
		break;
	case SD3_BOOT:
	case MMC3_BOOT:
		if (!getenv("bootcmd_android_recovery"))
			setenv("bootcmd_android_recovery",
				"boota mmc2 recovery");
		break;
	case MMC4_BOOT:
		if (!getenv("bootcmd_android_recovery"))
			setenv("bootcmd_android_recovery",
				"boota mmc3 recovery");
		break;
#endif /*CONFIG_FASTBOOT_STORAGE_MMC*/
	case SPI_NOR_BOOT:
		if (!getenv("bootcmd_android_recovery"))
			setenv("bootcmd_android_recovery",
				"boota mmc0 recovery");
		break;
	default:
		printf("Unsupported bootup device for recovery: dev: %d\n",
			bootdev);
		return;
	}

	printf("setup env for recovery..\n");
	setenv("bootcmd", "run bootcmd_android_recovery");
}

#endif /*CONFIG_ANDROID_RECOVERY*/

#endif /*CONFIG_FSL_FASTBOOT*/
