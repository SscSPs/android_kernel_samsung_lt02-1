/*
 * linux/arch/arm/mach-mmp/reset.c
 *
 * Author: Neil Zhang <zhangwm@xxxxxxxxxxx>
 * Copyright: (C) 2012 Marvell International Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/kernel.h>
#include <linux/smp.h>

#include <asm/io.h>
#include <asm/cacheflush.h>
#include <asm/mach/map.h>

#include <mach/addr-map.h>

#include "reset.h"

#define PMU_CC2_AP APMU_REG(0x0100)
#define CIU_CA9_WARM_RESET_VECTOR CIU_REG(0x00d8)

/*
 * This function is called from boot_secondary to bootup the secondary cpu.
 */
void mmp_cpu_power_up(u32 cpu)
{
	u32 tmp;

	BUG_ON(cpu == 0);

	tmp = readl(PMU_CC2_AP);
	if (tmp & CPU_CORE_RST(cpu)) {
		/* Release secondary core from reset */
		tmp &= ~(CPU_CORE_RST(cpu)
			| CPU_DBG_RST(cpu) | CPU_WDOG_RST(cpu));
		writel(tmp, PMU_CC2_AP);
	}
}

void mmp_set_entry_vector(u32 cpu, u32 addr)
{
	BUG_ON(cpu >= nr_cpu_ids);

	mmp_entry_vectors[cpu] = addr;
	smp_wmb();
	__cpuc_flush_dcache_area((void *)&mmp_entry_vectors[cpu],
	sizeof(mmp_entry_vectors[cpu]));
	outer_clean_range(__pa(&mmp_entry_vectors[cpu]),
			  __pa(&mmp_entry_vectors[cpu + 1]));
}

void __init mmp_entry_vector_init(void)
{
	int cpu;

	/* We will reset from DDR directly by default */
	writel(__pa(mmp_cpu_reset_entry), CIU_CA9_WARM_RESET_VECTOR);

	for (cpu = 1; cpu < nr_cpu_ids; cpu++)
		mmp_set_entry_vector(cpu, __pa(mmp_secondary_startup));
}
