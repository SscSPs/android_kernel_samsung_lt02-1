#ifndef _MFS_GPIO_I2C_H_
#define _MFS_GPIO_I2C_H_

/* Functions */

#undef SCL_LOW_CHECK
#ifdef SCL_LOW_CHECK
unsigned char mfs_gpio_i2c_check_scl_hold(void);
#endif

unsigned char mfs_gpio_i2c_write(unsigned char ucAddress,
				 unsigned char *pucData, int nLength);
unsigned char mfs_gpio_i2c_read(unsigned char ucAddress,
				unsigned char *pucData, int nLength);


#endif
