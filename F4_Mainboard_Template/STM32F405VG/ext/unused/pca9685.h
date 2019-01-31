#ifndef PCA9685_H
#define PCA9685_H

#include <stm32f4xx.h>
#include <stm32f4xx_i2c.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>

//7-bit addresses
#define MASTER_ADDRESS  0x1C << 1	// =0011100X
#define SLAVE_ADDRESS   0x40 << 1	// =1000000X

#define SCL_CLK    100000 //100KHz

#define PRESCALER  0x11   //25MHz / (4096 * 333Hz) - 1 = 17

//register addresses
#define MODE1          0x00
#define PWM_BASE       0x08  // =LED0_OFF_L
#define ALL_LED_OFF_L  0xFC
#define PRE_SCALE      0xFE

//MODE1 flag bit
#define MODE1_AI       0x05
#define MODE1_SLEEP    0x04
#define MODE1_ALLCALL  0x00

//PWM constraints
#define MIN_PWM  1366
#define MED_PWM  2048
#define MAX_PWM  2730

void i2c_init(void);
u8 pca9685_init(void);
u8 pca9685_set_pwm(u8 servoID, u16 offVal);
u8 pca9685_reset_pwm(void);

#endif
