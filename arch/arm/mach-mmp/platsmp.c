/*
 * linux/arch/arm/mach-mmp/platsmp.c
 *
 * Copyright (C) 2002 ARM Ltd.
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/jiffies.h>
#include <linux/smp.h>
#include <linux/io.h>
#include <linux/irq.h>

#include <asm/cacheflush.h>
#include <mach/hardware.h>
#include <linux/irqchip/arm-gic.h>
#include <asm/mach-types.h>
//#include <asm/localtimer.h>
#include <asm/smp.h>
#include <asm/smp_scu.h>

#include <mach/irqs.h>
#include <mach/addr-map.h>

#include "common.h"
#include "reset.h"

/*
 * Write pen_release in a way that is guaranteed to be visible to all
 * observers, irrespective of whether they're taking part in coherency
 * or not. This is necessary for the hotplug code to work reliably.
 */
static void write_pen_release(int val)
{
	pen_release = val;
	smp_wmb();
	__cpuc_flush_dcache_area((void *)&pen_release, sizeof(pen_release));
	outer_clean_range(__pa(&pen_release), __pa(&pen_release + 1));
}

#ifdef CONFIG_HAVE_ARM_SCU
static void __iomem *scu_get_base_addr(void)
{
	return SCU_VIRT_BASE;
}
#endif

static inline unsigned int get_core_count(void)
{
	u32 ret = 1;
#ifdef CONFIG_HAVE_ARM_SCU
	ret = scu_get_core_count(scu_get_base_addr());
#endif

	return ret;
}

static DEFINE_SPINLOCK(boot_lock);

static void mmp_secondary_init(unsigned int cpu)
{
	/*
	 * let the primary processor know we're out of the
	 * pen, then head off into the C entry point
	 */
	write_pen_release(-1);

	/*
	 * Synchronise with the boot thread.
	 */
	spin_lock(&boot_lock);
	spin_unlock(&boot_lock);
}

static int mmp_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	unsigned long timeout;

	/*
	 * Avoid timer calibration on slave cpus. Use the value calibrated
	 * on master cpu. Referenced from tegra3
	 */
	preset_lpj = loops_per_jiffy;

	/*
	 * set synchronisation state between this boot processor
	 * and the secondary one
	 */
	spin_lock(&boot_lock);

	/*
	 * The secondary processor is waiting to be released from
	 * the holding pen - release it, then wait for it to flag
	 * that it has been released by resetting pen_release.
	 *
	 * Note that "pen_release" is the hardware CPU ID, whereas
	 * "cpu" is Linux's internal ID.
	 */
	write_pen_release(cpu);

	/* reset the cpu, let it branch to the kernel entry */
	mmp_cpu_power_up(cpu);

	timeout = jiffies + (1 * HZ);
	while (time_before(jiffies, timeout)) {
		smp_rmb();
		if (pen_release == -1)
			break;

		udelay(10);
	}

	/*
	 * now the secondary core is starting up let it run its
	 * calibrations, then wait for it to finish
	 */
	spin_unlock(&boot_lock);

	return pen_release != -1 ? -ENOSYS : 0;
}

/*
 * Initialise the CPU possible map early - this describes the CPUs
 * which may be present or become present in the system.
 */
static void __init mmp_smp_init_cpus(void)
{
	unsigned int i, ncores = get_core_count();

	if (ncores > nr_cpu_ids) {
		pr_warn("SMP: %u cores greater than maximum (%u), clipping\n",
		ncores, nr_cpu_ids);
		ncores = nr_cpu_ids;
	}

	for (i = 0; i < ncores; i++)
		set_cpu_possible(i, true);
}

static void __init mmp_smp_prepare_cpus(unsigned int max_cpus)
{
	int i;

	/*
	 * Initialise the present map, which describes the set of CPUs
	 * actually populated at the present time.
	 */
	for (i = 0; i < max_cpus; i++)
		set_cpu_present(i, true);

#ifdef CONFIG_HAVE_ARM_SCU
	scu_enable(scu_get_base_addr());
#endif
}

struct smp_operations mmp_smp_ops __initdata = {
	.smp_init_cpus = mmp_smp_init_cpus,
	.smp_prepare_cpus = mmp_smp_prepare_cpus,
	.smp_secondary_init = mmp_secondary_init,
	.smp_boot_secondary = mmp_boot_secondary,
};
