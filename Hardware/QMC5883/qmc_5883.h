#ifndef _QMC_5883_h
#define _QMC_5883_h

#include "stm32f10x.h"  
#include "stdio.h"
#include "Delay.h"
#include "math.h"
#include "GPS.h" //ÄÇ¸öpi
#include "QMC_MPU_I2C.h"

#define QMC_PORT GPIOA
#define QMC_CLK GPIO_Pin_6
#define QMC_SDA GPIO_Pin_7

#define MQC5883_ADDRESS		      0x0D//i2c address

#define QMC5883_X_LSB			0x00
#define QMC5883_X_MSB			0x01
#define QMC5883_Y_LSB			0x02
#define QMC5883_Y_MSB			0x03
#define QMC5883_Z_LSB			0x04
#define QMC5883_Z_MSB			0x05
#define QMC5883_Status_Register 0x06
#define QMC5883_Emperature_LSB	0x07
#define QMC5883_Emperature_MSB	0x08
#define QMC5883_Control			0x09
//#define QMC5883_Z_MSB			0x0A
#define QMC5883_SET_RESET 		0x0B
//#define QMC5883_Z_MSB			0x0C


typedef struct QMC5883{
	int16_t XMag;
	int16_t YMag;
	int16_t ZMag;
}QMC5883;
extern float Yaw_Angle;
extern float OldAngle;
extern float NewAngle;
extern float azimuth;
extern uint8_t NotGPS_index;
extern uint16_t HMC5983_Init_index;

//////////////////////////////qmc5883
void QMC5883_Get_Data(QMC5883* MyQMC5883);
void QMC5883_Init(void);
uint8_t Status_Register(void);
uint8_t Chip_ID_Register(void);
float HMC5983_Get_Angle(void);
float QMC5883_Get_Emperature(void);
void QMC5883_Get_Yaw_Angles(float *yaw);
void Magnetometer_Calibration(void);

void ZNZ_Selection(uint8_t Selection);
#endif
