#ifndef __CONTROL_H__
#define __CONTROL_H__

#include "stm32f10x.h"
#include "GPS.h"
#include "motor_pwm.h"
#include "can.h"
#include "QMC_5883.h"
#include "my_mpu6050.h"

#define YAW_SPEED 50
#define Pitch_SPEED 20
extern int zg_return;
void Control_Photosensitive(void);
void Control_Gps(void);
void Control_test(void);
#endif
