#include <linux/reboot.h>
#define ARRAY_AND_SIZE(x)	(x), ARRAY_SIZE(x)

extern struct smp_operations mmp_smp_ops;

extern void timer_init(int irq);

extern void __init mmp_map_io(void);
extern void mmp_restart(enum reboot_mode, const char *);
extern void __init pxa168_clk_init(void);
extern void __init pxa910_clk_init(void);
extern void __init mmp2_clk_init(void);
