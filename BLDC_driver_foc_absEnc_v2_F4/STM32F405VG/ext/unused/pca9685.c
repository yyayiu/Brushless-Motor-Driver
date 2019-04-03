/**
  ******************************************************************************
  * _____  ________________   ________      ______                           
  * __  / / /_  ___/__  __/   ___  __ \________  /__________________________ 
  * _  / / /_____ \__  /      __  /_/ /  __ \_  __ \  __ \  ___/  __ \_  __ \
  * / /_/ / ____/ /_  /       _  _, _// /_/ /  /_/ / /_/ / /__ / /_/ /  / / /
  * \____/  /____/ /_/        /_/ |_| \____//_.___/\____/\___/ \____//_/ /_/ 
  *                                                                
  * @file    pca9685.c
  * @author  Pang Hong Wing
  * @version V0.1.0
  * @date    14 Mar, 2016
  * @brief   Library for controlling the adaFruit PCA9685 servo PWM controller.
  * @warning I2C protocol will be used for communication between mainboard and
             controller board. For mainboards without a corresponding I2C port,
             the UART3 port (SCL -> RX, SDA -> TX) will be used instead. 
  ******************************************************************************
  */

#include "pca9685.h"

#define Timed(x) Timeout = 0xFFFF; while (x) { if (Timeout-- == 0) goto errReturn;}

/**
  * @brief  Initialize I2C protocol.
  * @param  None
  * @retval None
  */
void i2c_init() {
  GPIO_InitTypeDef GPIO_InitStructure;
  I2C_InitTypeDef  I2C_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  
  GPIO_StructInit(&GPIO_InitStructure);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, ENABLE);
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, DISABLE);
  
  I2C_StructInit(&I2C_InitStructure);
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = MASTER_ADDRESS;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = SCL_CLK;

  I2C_Init(I2C2, &I2C_InitStructure);
  I2C_Cmd(I2C2, ENABLE);
}

/**
  * @brief  Initializes communication, through setting PWM and relevant flags.
  * @param  None
  * @retval 0 = success initialization; 1 = error occurred
  */
u8 pca9685_init() {
  __IO uint32_t Timeout = 0;
  
  //Set device to sleep mode (in order to set PWM)
  Timed(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));
  I2C_GenerateSTART(I2C2, ENABLE);
  Timed(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
  I2C_Send7bitAddress(I2C2, SLAVE_ADDRESS, I2C_Direction_Transmitter);
  Timed(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  I2C_SendData(I2C2, MODE1);
  Timed(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  I2C_SendData(I2C2, (1 << MODE1_SLEEP));
  Timed(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  I2C_GenerateSTOP(I2C2, ENABLE);
  
  //Set PWM to 333Hz
  Timed(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));
  I2C_GenerateSTART(I2C2, ENABLE);
  Timed(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
  I2C_Send7bitAddress(I2C2, SLAVE_ADDRESS, I2C_Direction_Transmitter);
  Timed(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  I2C_SendData(I2C2, PRE_SCALE);
  Timed(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  I2C_SendData(I2C2, PRESCALER);
  Timed(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  I2C_GenerateSTOP(I2C2, ENABLE);

  //Set AI and ALLCALL flags; reset SLEEP flag to wake device up
  Timed(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));
  I2C_GenerateSTART(I2C2, ENABLE);
  Timed(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
  I2C_Send7bitAddress(I2C2, SLAVE_ADDRESS, I2C_Direction_Transmitter);
  Timed(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  I2C_SendData(I2C2, MODE1);
  Timed(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  I2C_SendData(I2C2, ((1 << MODE1_AI) | (1 << MODE1_ALLCALL)));
  Timed(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

  I2C_GenerateSTOP(I2C2, ENABLE);

  return 0;
  errReturn:
  return 1;
}

/**
  * @brief  Set PWM pulse width for a specific servo. Input for val cannot
            exceed range defined in pca9685.h
  * @param  servoID: pin no. of servo to be set (0-15)
  * @param  val: PWM pulse width to be set (0-4096)
  * @retval 0 = success initialization; 1 = error occurred
  */
u8 pca9685_set_pwm(u8 servoID, u16 val) {
  val = ((val <= MAX_PWM) ? ((val >= MIN_PWM) ? val : MIN_PWM) : MAX_PWM);
  
  __IO uint32_t Timeout = 0;
  
  Timed(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));
  I2C_GenerateSTART(I2C2, ENABLE);
  Timed(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
  I2C_Send7bitAddress(I2C2, SLAVE_ADDRESS, I2C_Direction_Transmitter);
  Timed(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
  I2C_SendData(I2C2, PWM_BASE + (servoID * 4));
  Timed(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  I2C_SendData(I2C2, (val & 0xFF));
  Timed(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  I2C_SendData(I2C2, ((val >> 8) & 0x0F));
  Timed(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  I2C_GenerateSTOP(I2C2, ENABLE);
  
  return 0;
  errReturn:
  return 1;
}

/**
  * @brief  Reset all servo positions to mechanical zero position.
  * @param  None
  * @retval 0 = success transmission; 1 = error occurred
  */
u8 pca9685_reset_pwm() {
  __IO uint32_t Timeout = 0;
  
  Timed(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));
  I2C_GenerateSTART(I2C2, ENABLE);
  Timed(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));
  I2C_Send7bitAddress(I2C2, SLAVE_ADDRESS, I2C_Direction_Transmitter);
  Timed(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
  I2C_SendData(I2C2, ALL_LED_OFF_L);
  Timed(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  I2C_SendData(I2C2, (MED_PWM & 0xFF));
  Timed(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  I2C_SendData(I2C2, ((MED_PWM >> 8) & 0x0F));
  Timed(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  I2C_GenerateSTOP(I2C2, ENABLE);
  
  return 0;
  errReturn:
  return 1;  
}
