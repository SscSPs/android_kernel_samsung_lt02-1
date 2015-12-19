/*
 *  linux/arch/arm/mach-mmp/mmp2-dt.c
 *
 *  Copyright (C) 2012 Marvell Technology Group Ltd.
 *  Author: Haojian Zhuang <haojian.zhuang@marvell.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  publishhed by the Free Software Foundation.
 */

#include <linux/io.h>
#include <linux/irqchip.h>
#include <linux/of_platform.h>
#include <linux/clk-provider.h>
#include <linux/clocksource.h>
#include <asm/mach/arch.h>
#include <asm/mach/time.h>
#include <asm/hardware/cache-l2x0.h>
#include <asm/hardware/cache-tauros2.h>

#include "common.h"
#include "reset.h"

extern void __init mmp_dt_init_timer(void);

static void __init mmp_init_time(void)
{
#ifdef CONFIG_CACHE_TAUROS2
	tauros2_init(0);
#endif
	mmp_dt_init_timer();
	of_clk_init(NULL);
}

#if 1
/* should be elsewhere TODO */
static void __init pxa988_dt_init_early(void)
{
	mmp_entry_vector_init();
}

static void __init pxa988_dt_init_machine(void)
{
	l2x0_of_init(0x30800000, 0xFE7FFFFF);

	pxa910_clk_init();
}
#endif

static const char *const mmp2_dt_board_compat[] __initconst = {
	"mrvl,mmp2-brownstone",
	NULL,
};

static const char *const pxa988_dt_board_compat[] __initconst = {
	"mrvl,pxa988-dkb",
	NULL,
};

DT_MACHINE_START(MMP2_DT, "Marvell MMP2 (Device Tree Support)")
	.map_io		= mmp_map_io,
	.init_time	= mmp_init_time,
	.dt_compat	= mmp2_dt_board_compat,
MACHINE_END

DT_MACHINE_START(PXA988_DT, "Marvell PXA988 (Device Tree Support)")
	/*smp		= smp_ops(mmp_smp_ops),*/
	.map_io		= mmp_map_io,
	.init_early	= pxa988_dt_init_early,
	.init_irq	= irqchip_init,
	.init_time	= mmp_init_time,
	.init_machine	= pxa988_dt_init_machine,
	.dt_compat	= pxa988_dt_board_compat,
MACHINE_END
