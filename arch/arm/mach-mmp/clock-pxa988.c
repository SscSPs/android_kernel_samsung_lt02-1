#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/io.h>
#include <linux/clk.h>
#include <linux/clk/mmp.h>

#include "addr-map.h"

#include "common.h"
#include "clock.h"

/*
 * APB Clock register offsets for PXA988
 */
#define APBC_UART0	APBC_REG(0x000)
#define APBC_UART1	APBC_REG(0x004)
#define APBC_GPIO	APBC_REG(0x008)
#define APBC_PWM1	APBC_REG(0x00c)
#define APBC_PWM2	APBC_REG(0x010)
#define APBC_PWM3	APBC_REG(0x014)
#define APBC_PWM4	APBC_REG(0x018)
#define APBC_SSP1	APBC_REG(0x01c)
#define APBC_SSP2	APBC_REG(0x020)
#define APBC_IPC	APBC_REG(0x024)
#define APBC_RTC	APBC_REG(0x028)
#define APBC_TWSI0	APBC_REG(0x02c)
#define APBC_KPC	APBC_REG(0x030)
#define APBC_TIMERS	APBC_REG(0x034)
#define APBC_TBROT	APBC_REG(0x038)
#define APBC_AIB	APBC_REG(0x03c)
#define APBC_SW_JTAG	APBC_REG(0x040)
#define APBC_TIMERS1	APBC_REG(0x044)
#define APBC_ONEWIRE	APBC_REG(0x048)
#define APBC_ASFAR	APBC_REG(0x050)
#define  AKEY_ASFAR		0xbaba
#define APBC_ASSAR	APBC_REG(0x054)
#define  AKEY_ASSAR		0xeb10
#define APBC_DROTS	APBC_REG(0x058)
#define APBC_I2CIO	APBC_REG(0x05c)
#define APBC_TWSI1	APBC_REG(0x060)

#define APBC_UART0	APBCP_REG(0x01c)
#define APBC_PWRTWSI	APBCP_REG(0x028)
#define APBC_RIPC	APBCP_REG(0x038)
/* GBS: clock for GSSP */
#define APBC_GBS	CPBC_REG(0x00c)
#define APBC_GCER	CPBC_REG(0x034)

#define APMU_NAND	APMU_REG(0x060)
#define APMU_USB	APMU_REG(0x05c)
#define APMU_MCK4_CTL	APMU_REG(0x0e8)

static APBC_CLK(uart0, UART0, 1, 14745600);
static APBC_CLK(uart1, UART1, 1, 14745600);
static APBC_CLK(uart2, UART2, 1, 14745600);
static APBC_CLK(twsi0, TWSI0, 0, 33000000);
static APBC_CLK(twsi1, TWSI1, 0, 33000000);
static APBC_CLK(pwm1, PWM1, 0, 13000000);
static APBC_CLK(pwm2, PWM2, 0, 13000000);
static APBC_CLK(pwm3, PWM3, 0, 13000000);
static APBC_CLK(pwm4, PWM4, 0, 13000000);
static APBC_CLK(gpio, GPIO, 0, 13000000);
static APBC_CLK(rtc, RTC, 0, 32000);

static APMU_CLK(nand, NAND, 0x19b, 156000000);
static APMU_CLK(u2o, USB, 0x05c, 480000000);

/* device and clock bindings */
static struct clk_lookup pxa988_clkregs[] = {
	INIT_CLKREG(&clk_uart0, "pxa2xx-uart.0", NULL),
	INIT_CLKREG(&clk_uart1, "pxa2xx-uart.1", NULL),
	INIT_CLKREG(&clk_uart2, "pxa2xx-uart.2", NULL);
	INIT_CLKREG(&clk_twsi0, "pxa2xx-i2c.0", NULL),
	INIT_CLKREG(&clk_twsi1, "pxa2xx-i2c.1", NULL),
	INIT_CLKREG(&clk_pwm1, "pxa910-pwm.0", NULL),
	INIT_CLKREG(&clk_pwm2, "pxa910-pwm.1", NULL),
	INIT_CLKREG(&clk_pwm3, "pxa910-pwm.2", NULL),
	INIT_CLKREG(&clk_pwm4, "pxa910-pwm.3", NULL),
	INIT_CLKREG(&clk_nand, "pxa3xx-nand", NULL),
	INIT_CLKREG(&clk_gpio, "mmp-gpio", NULL),
	INIT_CLKREG(&clk_u2o, NULL, "U2OCLK"),
	INIT_CLKREG(&clk_rtc, "sa1100-rtc", NULL),
};

void __init pxa988_clk_init(phys_addr_t mpmu_phys, phys_addr_t apmu_phys,
			    phys_addr_t apbc_phys, phys_addr_t apbcp_phys)
{
	clkdev_add_table(ARRAY_AND_SIZE(pxa988_clkregs));
}
