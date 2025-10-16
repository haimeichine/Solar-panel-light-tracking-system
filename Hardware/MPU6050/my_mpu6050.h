#ifndef MY_MPU6050_H
#define MY_MPU6050_H
#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "QMC_MPU_I2C.h"

#define MPU_PORT GPIOB
#define MPU_CLK GPIO_Pin_13
#define MPU_SDA GPIO_Pin_12

#define MPU6050_ADDRESS		      0x68//i2c address


//**************************** 6050 register below
#define	MPU6050_SMPLRT_DIV		  0x19
#define	MPU6050_CONFIG			  0x1A
#define	MPU6050_GYRO_CONFIG		  0x1B
#define	MPU6050_ACCEL_CONFIG	  0x1C
#define MPU6050_FIFO_EN           0x23

#define MPU6050_INTBP_CFG_REG	  0X37	//ÖÐ¶Ï¼Ä´æÆ÷
#define MPU6050_INT_ENABLE        0x38

#define	MPU6050_ACCEL_XOUT_H	  0x3B
#define	MPU6050_ACCEL_XOUT_L	  0x3C
#define	MPU6050_ACCEL_YOUT_H	  0x3D
#define	MPU6050_ACCEL_YOUT_L	  0x3E
#define	MPU6050_ACCEL_ZOUT_H	  0x3F
#define	MPU6050_ACCEL_ZOUT_L	  0x40
#define	MPU6050_TEMP_OUT_H		  0x41
#define	MPU6050_TEMP_OUT_L		  0x42
#define	MPU6050_GYRO_XOUT_H		  0x43
#define	MPU6050_GYRO_XOUT_L		  0x44
#define	MPU6050_GYRO_YOUT_H		  0x45
#define	MPU6050_GYRO_YOUT_L		  0x46
#define	MPU6050_GYRO_ZOUT_H		  0x47
#define	MPU6050_GYRO_ZOUT_L		  0x48
#define MPU6050_SIGNAL_PATH_RESET 0x68

#define MPU6050_USER_CTRL         0x6A
#define	MPU6050_PWR_MGMT_1		  0x6B
#define	MPU6050_WHO_AM_I		  0x75

////////// 6050
typedef struct MPU6050_raw{
	int16_t AccX;
	int16_t AccY;
	int16_t AccZ;
	int16_t GyroX;
	int16_t GyroY;
	int16_t GyroZ;
	int16_t Temp;
}MPU6050_raw;

typedef struct MPU6050_Angle
{
    float yaw;	//Æ«º½½Ç
    float roll;	//ºá¹ö½Ç
    float pitch;//¸©Ñö½Ç
}MPU6050_Angle;


extern float Pitch_Angle;								//¸©Ñö½Ç¶È

////iic

///mpu6050
void	MPU6050_W_reg(uint8_t regadd,uint8_t data);
uint8_t MPU6050_R_reg(uint8_t regadd);
void 	MPU6050_Init(void);
void 	MPU6050_Get_Raws(MPU6050_raw* this);
float MY_MPU6050_GetTemp(void);
void MPU6050_Get_Angles(float* Pitch_Angle);
//void MPU6050_Get_Angles(MPU6050_Angle* this);
#endif
