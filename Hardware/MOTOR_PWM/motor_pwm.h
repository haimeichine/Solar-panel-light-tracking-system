#ifndef __MOTOR_PWM_H__
#define __MOTOR_PWM_H__

#include "stm32f10x.h"  
void MOTOR_Yaw_Pitch_Init(void);
void Yaw_Pwm_Init(void);
void Pitch_Pwm_Init(void);
void EN_Motor_Init(void);
	
void Yaw_Motor_Pwm_1(uint16_t value);
void Yaw_Motor_Pwm_2(uint16_t value);
void Pitch_Motor_Pwm_1(uint16_t value);
void Pitch_Motor_Pwm_2(uint16_t value);
void EN_Pitch_Motor(BitAction  bit);
void EN_Yaw_Motor(BitAction  bit);

/*Æ«º½¿ØÖÆ*/
void Yaw_Motor_Left(uint8_t speed);
void Yaw_Motor_Rigth(uint8_t speed);
void Yaw_Motor_Stop(void);
void Yaw_Motor_Auto(void);

/*¸©Ñö¿ØÖÆ*/
void Pitch_Motor_Left(uint8_t speed);
void Pitch_Motor_Rigth(uint8_t speed);
void Pitch_Motor_Stop(void);
void Pitch_Motor_Auto(void);


#endif
