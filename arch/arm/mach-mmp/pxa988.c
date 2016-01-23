/*
 *  linux/arch/arm/mach-mmp/pxa988.c
 *
 *  Code specific to PXA988
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/clk/mmp.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/irqchip/mmp.h>
#include <linux/platform_device.h>

#include <asm/hardware/cache-tauros2.h>
#include <asm/mach/time.h>
#include "addr-map.h"
#include "regs-apbc.h"
#include "cputype.h"
#include "irqs.h"
#include "mfp.h"
#include "devices.h"
#include "pm-pxa988.h"
#include "pxa988.h"

#include "common.h"

#define MFPR_VIRT_BASE	(APB_VIRT_BASE + 0x1e000)

static struct mfp_addr_map pxa988_mfp_addr_map[] __initdata =
{
	MFP_ADDR_X(GPIO0, GPIO54, 0xdc),
	MFP_ADDR_X(GPIO67, GPIO98, 0x1b8),
	MFP_ADDR_X(GPIO100, GPIO109, 0x238),

	MFP_ADDR(GPIO123, 0xcc),
	MFP_ADDR(GPIO124, 0xd0),

	MFP_ADDR(DF_IO0, 0x40),
	MFP_ADDR(DF_IO1, 0x3c),
	MFP_ADDR(DF_IO2, 0x38),
	MFP_ADDR(DF_IO3, 0x34),
	MFP_ADDR(DF_IO4, 0x30),
	MFP_ADDR(DF_IO5, 0x2c),
	MFP_ADDR(DF_IO6, 0x28),
	MFP_ADDR(DF_IO7, 0x24),
	MFP_ADDR(DF_IO8, 0x20),
	MFP_ADDR(DF_IO9, 0x1c),
	MFP_ADDR(DF_IO10, 0x18),
	MFP_ADDR(DF_IO11, 0x14),
	MFP_ADDR(DF_IO12, 0x10),
	MFP_ADDR(DF_IO13, 0xc),
	MFP_ADDR(DF_IO14, 0x8),
	MFP_ADDR(DF_IO15, 0x4),

	MFP_ADDR(DF_nCS0_SM_nCS2, 0x44),
	MFP_ADDR(DF_nCS1_SM_nCS3, 0x48),
	MFP_ADDR(SM_nCS0, 0x4c),
	MFP_ADDR(SM_nCS1, 0x50),
	MFP_ADDR(DF_WEn, 0x54),
	MFP_ADDR(DF_REn, 0x58),
	MFP_ADDR(DF_CLE_SM_OEn, 0x5c),
	MFP_ADDR(DF_ALE_SM_WEn, 0x60),
	MFP_ADDR(SM_SCLK, 0x64),
	MFP_ADDR(DF_RDY0, 0x68),
	MFP_ADDR(SM_BE0, 0x6c),
	MFP_ADDR(SM_BE1, 0x70),
	MFP_ADDR(SM_ADV, 0x74),
	MFP_ADDR(DF_RDY1, 0x78),
	MFP_ADDR(SM_ADVMUX, 0x7c),
	MFP_ADDR(SM_RDY, 0x80),

	MFP_ADDR_X(MMC1_DAT7, MMC1_WP, 0x84),

	MFP_ADDR_END,
};

void __init pxa988_init_irq(void)
{
	icu_init_irq();
#ifdef CONFIG_PM
	icu_irq_chip.irq_set_wake = pxa988_set_wake;
#endif
}

static int __init pxa988_init(void)
{
	if (cpu_is_pxa988()) {
#ifdef CONFIG_CACHE_TAUROS2
		tauros2_init(0);
#endif
		mfp_init_base(MFPR_VIRT_BASE);
		mfp_init_addr(pxa988_mfp_addr_map);
		pxa988_clk_init(APB_PHYS_BASE + 0x50000,
				AXI_PHYS_BASE + 0x82800,
				APB_PHYS_BASE + 0x15000,
				APB_PHYS_BASE + 0x3b000);
	}

	return 0;
}
postcore_initcall(pxa988_init);

/* system timer - clock enabled, 3.25MHz */
#define TIMER_CLK_RST	(APBC_APBCLK | APBC_FNCLK | APBC_FNCLKSEL(3))
#define APBC_TIMERS	APBC_REG(0x34)

void __init pxa988_timer_init(void)
{
	/* reset and configure */
	__raw_writel(APBC_APBCLK | APBC_RST, APBC_TIMERS);
	__raw_writel(TIMER_CLK_RST, APBC_TIMERS);

	timer_init(IRQ_PXA988_AP1_TIMER1);
}

/* on-chip devices */

/* NOTE: there are totally 3 UARTs on PXA988:
 *
 *   UART1   - Slow UART (can be used both by AP and CP)
 *   UART2/3 - Fast UART
 *
 * To be backward compatible with the legacy FFUART/BTUART/STUART sequence,
 * they are re-ordered as:
 *
 *   pxa988_device_uart1 - UART2 as FFUART
 *   pxa988_device_uart2 - UART3 as BTUART
 *
 * UART1 is not used by AP for the moment.
 */

/* on-chip devices */
PXA988_DEVICE(uart0, "pxa2xx-uart", 0, UART0, 0xd4036000, 0x30, 4, 5);
PXA988_DEVICE(uart1, "pxa2xx-uart", 1, UART1, 0xd4017000, 0x30, 21, 22);
PXA988_DEVICE(uart2, "pxa2xx-uart", 2, UART2, 0xd4018000, 0x30, 23, 24);
PXA988_DEVICE(keypad, "pxa27x-keypad", -1, KEYPAD, 0xd4012000, 0x4c);
PXA988_DEVICE(twsi0, "pxa910-i2c", 0, I2C0, 0xd4011000, 0x40);
PXA988_DEVICE(twsi1, "pxa910-i2c", 1, I2C1, 0xd4010800, 0x40);
PXA988_DEVICE(twsi2, "pxa910-i2c", 2, I2C2, 0xd4037000, 0x40);
PXA988_DEVICE(pwm1, "pxa910-pwm", 0, NONE, 0xd401a000, 0x10);
PXA988_DEVICE(pwm2, "pxa910-pwm", 1, NONE, 0xd401a400, 0x10);
PXA988_DEVICE(pwm3, "pxa910-pwm", 2, NONE, 0xd401a800, 0x10);
PXA988_DEVICE(pwm4, "pxa910-pwm", 3, NONE, 0xd401ac00, 0x10);
PXA988_DEVICE(sdh1, "sdhci-pxav3", 0, MMC, 0xd4280000, 0x120);
PXA988_DEVICE(sdh2, "sdhci-pxav3", 1, MMC, 0xd4280800, 0x120);
PXA988_DEVICE(sdh3, "sdhci-pxav3", 2, MMC, 0xd4281000, 0x120);
PXA988_DEVICE(ssp0, "pxa988-ssp", 0, SSP0, 0xd401b000, 0x90, 52, 53);
PXA988_DEVICE(ssp1, "pxa988-ssp", 1, SSP1, 0xd42a0c00, 0x90, 1, 2);
PXA988_DEVICE(ssp2, "pxa988-ssp", 2, SSP2, 0xd401C000, 0x90, 60, 61);
PXA988_DEVICE(gssp, "pxa988-ssp", 4, GSSP, 0xd4039000, 0x90, 6, 7);
//PXA988_DEVICE(asram, "asram", 0, NONE, SRAM_AUDIO_BASE, SRAM_AUDIO_SIZE);
//PXA988_DEVICE(isram, "isram", 1, NONE, SRAM_VIDEO_BASE, SRAM_VIDEO_SIZE);
PXA988_DEVICE(fb, "pxa168-fb", 0, LCD, 0xd420b000, 0x1fc);
PXA988_DEVICE(fb_ovly, "pxa168fb_ovly", 0, LCD, 0xd420b000, 0x1fc);
PXA988_DEVICE(fb_tv, "pxa168-fb", 1, LCD, 0xd420b000, 0x1fc);
PXA988_DEVICE(fb_tv_ovly, "pxa168fb_ovly", 1, LCD, 0xd420b000, 0x1fc);
PXA988_DEVICE(camera, "mmp-camera", 0, CI, 0xd420a000, 0xfff);
PXA988_DEVICE(thermal, "thermal", -1, DRO_SENSOR, 0xd4013200, 0x34);


struct resource pxa988_resource_gpio[] = {
	{
		.start	= 0xd4019000,
		.end	= 0xd4019fff,
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= IRQ_PXA910_AP_GPIO,
		.end	= IRQ_PXA910_AP_GPIO,
		.name	= "gpio_mux",
		.flags	= IORESOURCE_IRQ,
	},
};

struct platform_device pxa988_device_gpio = {
	.name		= "mmp-gpio",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(pxa988_resource_gpio),
	.resource	= pxa988_resource_gpio,
};

static struct resource pxa988_resource_rtc[] = {
	{
		.start	= 0xd4010000,
		.end	= 0xd401003f,
		.flags	= IORESOURCE_MEM,
	}, {
		.start	= IRQ_PXA988_RTC_INT,
		.end	= IRQ_PXA988_RTC_INT,
		.name	= "rtc 1Hz",
		.flags	= IORESOURCE_IRQ,
	}, {
		.start	= IRQ_PXA988_RTC_ALARM,
		.end	= IRQ_PXA988_RTC_ALARM,
		.name	= "rtc alarm",
		.flags	= IORESOURCE_IRQ,
	},
};

struct platform_device pxa988_device_rtc = {
	.name		= "sa1100-rtc",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(pxa988_resource_rtc),
	.resource	= pxa988_resource_rtc,
};
