#ifndef _SERVO_H_
#define _SERVO_H_

/**
	This file is to control analog servo (or some digital servos).
	It generates PWM signal with 20ms period (Frequency 50Hz)
	Normal servo would have their middle point at 1.5ms high time (CCR = 1500)
	And both ends around +- 0.6ms (Read datasheet)
	
	Rex Cheng
*/

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"	  
#include "stm32f4xx_tim.h"
#include "gpio.h"

#define SERVO_INIT_TABLE \
X(SERVO1, TIM4, GPIO_AF_TIM4, RCC_APB1Periph_TIM4, 1, PD12, 1) \
X(SERVO2, TIM4, GPIO_AF_TIM4, RCC_APB1Periph_TIM4, 1, PD13, 2) \
X(SERVO3, TIM4, GPIO_AF_TIM4, RCC_APB1Periph_TIM4, 1, PD14, 3) \
X(SERVO4, TIM4, GPIO_AF_TIM4, RCC_APB1Periph_TIM4, 1, PD15, 4) \
X(SERVO5, TIM11, GPIO_AF_TIM11, RCC_APB2Periph_TIM11, 2, PF7, 1) \
X(SERVO6, TIM13, GPIO_AF_TIM13, RCC_APB1Periph_TIM13, 1, PF8, 1) \

//ServoID, min ccr, max ccr, min deg(Scaled by 10), max deg(Scaled by 10)
#define ServoConfigs_TABLE \
C(SERVO1, 900, 2100, 0, 1800) \
C(SERVO2, 900, 2100, 0, 1800) \
C(SERVO3, 900, 2100, 0, 1800) \
C(SERVO4, 900, 2100, 0, 1800) \
C(SERVO5, 900, 2100, 0, 1800) \
C(SERVO6, 900, 2100, 0, 1800) \

typedef struct {
	TIM_TypeDef* tim;
	const uint8_t tim_af;
	const uint32_t tim_rcc;
	const uint8_t rcc_line;
	const GPIO* gpio;
	const uint8_t channel;
} ServoStruct;

typedef struct{
	const uint16_t min_ccr;
	const uint16_t max_ccr;
	const int16_t min_deg;
	const int16_t max_deg;
} ServoConfig;

#define X(a, b, c, d, e, f, g) a, 
typedef enum {
  SERVO_INIT_TABLE
} ServoID;
#undef X

#define X(a, b, c, d, e, f, g) {b, c, d, e, &f, g},
static const ServoStruct ServoPorts[] = {SERVO_INIT_TABLE};
#undef X

#define C(a, b, c, d, e) {b, c, d, e},
static const ServoConfig ServoConfigs[] = {ServoConfigs_TABLE};
#undef C

#define SERVO_SIZE (sizeof(ServoPorts)/sizeof(ServoStruct))

//Init all servo
void servo_init(void);

/**
  * @brief  Control the ccr value of the servo
  * @param  servo_id: The servo id to be used
  * @param  val: Any value from 0~20000.
  * @retval None
  */
void servo_ccr_control(ServoID servo_id , uint32_t ccr_val);

/**
  * @brief  Control the degree of the servo
  * @param  servo_id: The servo id to be used
  * @param  val: Any value from MIN_DEG to MAX_DEG (defined in @ServoConfigs_TABLE)
  * @retval None
  */
void servo_deg_control(ServoID servo_id , int32_t degree);

#endif
