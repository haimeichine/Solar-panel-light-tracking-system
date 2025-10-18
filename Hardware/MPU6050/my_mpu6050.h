/////////////////////////////////////////////
//          太阳能追踪系统 (STS)           //
//                 用于                    //
//           太阳能应用领域                //
//                                         //
//              2025年5月26日              //
//                                         //
//   文件名: README.md										 //
//                                         //
//   作者: CLMN                            //
//   邮箱: <2181708694@qq.com>             //
//                                         //
//   电子信息工程学院                      //
//   柳州职业技术大学                      //
//   301实训室                             //
//   广西柳州, 中国                        //
/////////////////////////////////////////////

/////////////////////////////////////////////
//   使用方法请参考 STS.H 头文件           //
//                                         //
//   本代码基于NREL技术报告                //
//   《太阳能辐射应用的太阳位置算法》      //
//   作者: I. Reda & A. Andreas            //
//   并使用 SPA.C 进行太阳位置计算         //
/////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
//
//   MIT 许可证
//
//   版权所有 (c) 2025 CLMN, 柳州职业技术大学
//
//   特此免费授予任何获得本软件及相关文档文件（以下简称"软件"）副本的人不受限制地
//   处理本软件的权限，包括但不限于使用、复制、修改、合并、发布、分发、再许可
//   和/或销售本软件副本的权利，并允许接受本软件的人这样做，但需满足以下条件：
//
//   上述版权声明和本许可声明应包含在软件的所有副本或主要部分中。
//
//   本软件按"原样"提供，不附带任何明示或暗示的担保，包括但不限于对适销性、
//   特定用途的适用性和非侵权的担保。在任何情况下，作者或版权持有人均不对
//   因软件或使用或其他软件交易而产生的任何索赔、损害或其他责任负责，
//   无论是在合同诉讼、侵权行为还是其他方面。
//
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
//
//   系统描述：
//   本太阳能追踪系统利用NREL SPA算法实时计算太阳的方位角和高度角。
//   系统将这些计算出的角度与当前太阳能板朝向进行比较，并驱动电机以保持
//   与太阳的最佳对齐，从而实现最大能量收集。
//
//   主要特性：
//   1. 使用 SPA.C 进行实时太阳位置计算
//   2. 双轴追踪（方位角和高度角）
//   3. 面板定位电机控制
//   4. 误差校正和安全机制
//
//   修订历史：
//   2025-05-26 CLMN
//     - 初始版本创建
//     - 集成 SPA.C 太阳位置算法
//     - 实现电机控制逻辑
//
///////////////////////////////////////////////////////////////////////////////////////////////
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

#define MPU6050_INTBP_CFG_REG	  0X37	//中断寄存器
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
    float yaw;	//偏航角
    float roll;	//横滚角
    float pitch;//俯仰角
}MPU6050_Angle;


extern float Pitch_Angle;								//俯仰角度

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
