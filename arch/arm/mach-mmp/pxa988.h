#ifndef __ASM_MACH_PXA988_H
#define __ASM_MACH_PXA988_H

extern void pxa988_timer_init(void);
extern void __init icu_init_irq(void);
extern void __init pxa988_init_irq(void);

#include <linux/i2c.h>
#include <linux/i2c/pxa-i2c.h>
#include <linux/platform_data/mtd-nand-pxa3xx.h>
#include <video/mmp_disp.h>

#include "devices.h"

extern struct pxa_device_desc pxa988_device_uart1;
extern struct pxa_device_desc pxa988_device_uart2;
extern struct pxa_device_desc pxa988_device_twsi0;
extern struct pxa_device_desc pxa988_device_twsi1;
extern struct pxa_device_desc pxa988_device_pwm1;
extern struct pxa_device_desc pxa988_device_pwm2;
extern struct pxa_device_desc pxa988_device_pwm3;
extern struct pxa_device_desc pxa988_device_pwm4;
extern struct pxa_device_desc pxa988_device_nand;
extern struct platform_device pxa168_device_u2o;
extern struct platform_device pxa168_device_u2ootg;
extern struct platform_device pxa168_device_u2oehci;
extern struct pxa_device_desc pxa988_device_disp;
extern struct pxa_device_desc pxa988_device_fb;
extern struct pxa_device_desc pxa988_device_panel;
extern struct platform_device pxa988_device_gpio;
extern struct platform_device pxa988_device_rtc;

static inline int pxa988_add_uart(int id)
{
	struct pxa_device_desc *d = NULL;

	switch (id) {
	case 1: d = &pxa988_device_uart1; break;
	case 2: d = &pxa988_device_uart2; break;
	}

	if (d == NULL)
		return -EINVAL;

	return pxa_register_device(d, NULL, 0);
}

static inline int pxa988_add_twsi(int id, struct i2c_pxa_platform_data *data,
				  struct i2c_board_info *info, unsigned size)
{
	struct pxa_device_desc *d = NULL;
	int ret;

	switch (id) {
	case 0: d = &pxa988_device_twsi0; break;
	case 1: d = &pxa988_device_twsi1; break;
	default:
		return -EINVAL;
	}

	ret = i2c_register_board_info(id, info, size);
	if (ret)
		return ret;

	return pxa_register_device(d, data, sizeof(*data));
}

static inline int pxa988_add_pwm(int id)
{
	struct pxa_device_desc *d = NULL;

	switch (id) {
	case 1: d = &pxa988_device_pwm1; break;
	case 2: d = &pxa988_device_pwm2; break;
	case 3: d = &pxa988_device_pwm3; break;
	case 4: d = &pxa988_device_pwm4; break;
	default:
		return -EINVAL;
	}

	return pxa_register_device(d, NULL, 0);
}

static inline int pxa988_add_nand(struct pxa3xx_nand_platform_data *info)
{
	return pxa_register_device(&pxa988_device_nand, info, sizeof(*info));
}
#endif /* __ASM_MACH_PXA988_H */
