#ifndef __QMC_MPU_I2C__
#define __QMC_MPU_I2C__
#include "stm32f10x.h"                  // Device header
#include "Delay.h"

#include "QMC_5883.h"
#include "my_mpu6050.h"

//////
void 	MyI2C_W_SCL(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_SCL,uint8_t BitValue);
void 	MyI2C_W_SDA(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_SDA,uint8_t BitValue);
u8 		MyI2C_R_SCL(void);
u8		MyI2C_R_SDA(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_SDA);
void 	MyI2C_Init(void);
void 	MyI2C_Start(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_SCL,uint16_t GPIO_Pin_SDA);
void 	MyI2C_Stop(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_SCL, uint16_t GPIO_Pin_SDA);
void 	MyI2C_SendByte(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_SCL, uint16_t GPIO_Pin_SDA,uint8_t Byte);
uint8_t MyI2C_ReceiveByte(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_SCL, uint16_t GPIO_Pin_SDA);
void 	MyI2C_SendAck(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_SCL, uint16_t GPIO_Pin_SDA, uint8_t AckBit);
uint8_t MyI2C_ReceiveAck(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_SCL, uint16_t GPIO_Pin_SDA);

#endif
