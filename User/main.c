#include "stm32f10x.h"                  		// Device header
#include "Delay.h"
#include "serial.h"
#include "control.h"
#include "qmc_5883.h"
#include "my_mpu6050.h"
#include "qmc_mpu_i2c.h"
#include "motor_pwm.h"
#include "can.h"
#include "heartbeat.h"
#include "spa.h"  // 引入NOAA的头文件
#include <stdio.h>
//全局变量
char *Latitude;    								//存储纬度的字符串
char *Longitude;   								//存储经度的字符串
char *Utc_Time;     							//时间
char *GPS_Status;       						//定位状态
char *ns;           							//南北
char *ew;           							//东西
char *date;        								//日期
double GPS_E = 109.535554,GPS_N = 24.401311;	//火星坐标
double Solar_Azimuth;							//太阳方位角
double Solar_Altitude;							//太阳高度角
float Yaw_Angle;								//偏航角度
float Pitch_Angle;								//俯仰角度
uint8_t GPS_If;
spa_data spa;
/////adc
float ADC_Data1,ADC_Data2,ADC_Data3,ADC_Data4;
int main(void)
{
	Serial_UART_Init(USART1,9600);		//调试——USART
	Serial_UART_Init(USART2,9600);		//GPS——获取数据
	MyI2C_Init();						//初始化I2C
	QMC5883_Init();						//指南针初始化
	MPU6050_Init();						//陀螺仪初始化
	MOTOR_Yaw_Pitch_Init();				//直流电机初始化
	MyCAN_Init();						//CAN初始化
	HeartBeat();						//心跳    
	while (1)
	{
		//Control_Gps();
		//Get_Can_ADC_Data();
		//printf("ADC1=%.2f,ADC2=%.2f,ADC3=%.2f,ADC4=%.2f\n",ADC_Data1,ADC_Data2,ADC_Data3,ADC_Data4);
		//Control_Photosensitive();
		
		//Pitch_Motor_Rigth(80);
		//Pitch_Motor_Left(80);
		//Yaw_Motor_Rigth(5);
		//Yaw_Motor_Left(10);
	}
}
