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
#include "my_mpu6050.h"
#include "Delay.h"
#include "math.h"

///
typedef enum{//加速度陀螺仪滤波
	Band_256Hz = 0x00,
	Band_186Hz,
	Band_96Hz,
	Band_43Hz,
	Band_21Hz,
	Band_10Hz,
	Band_5Hz
}Filter_Typedef;

typedef enum{//陀螺仪配置寄存器
	gyro_250=0x00,
	gyro_500=0x08,
	gyro_1000=0x10,
	gyro_2000=0x18,
}GYRO_CONFIG_Typedef;
typedef enum{//陀螺仪配置寄存器
	acc_2g=0x00,
	acc_4g=0x08,
	acc_8g=0x10,
	acc_16g=0x18,
}ACC_CONFIG_Typedef;
typedef enum{//开启对应x,y,z寄存器
	FIFO_Disable,
	ACCEL_FIFO_EN = 0x08,
	ZG_FIFO_EN = 0x10,
	YG_FIFO_EN = 0x20,
	XG_FIFO_EN = 0x40,
	TEMP_FIFO_EN = 0x80
}FIFO_EN_Typedef;

typedef struct MY_MPU6050_InitTpyeDef{//分频系数
	uint8_t SMPLRT_DIV;
	Filter_Typedef Filter;
	GYRO_CONFIG_Typedef Gyro;
	ACC_CONFIG_Typedef Acc;
	FIFO_EN_Typedef Fifo;
}MY_MPU6050_InitTpyeDef;

/////////////////////////mpu6050
void MPU6050_W_reg(uint8_t regadd,uint8_t data){ //指定寄存器地址，写一个Byte
	MyI2C_Start(MPU_PORT,MPU_CLK,MPU_SDA);
	MyI2C_SendByte(MPU_PORT,MPU_CLK,MPU_SDA,MPU6050_ADDRESS<<1);
	MyI2C_ReceiveAck(MPU_PORT,MPU_CLK,MPU_SDA);
	MyI2C_SendByte(MPU_PORT,MPU_CLK,MPU_SDA,regadd);
	MyI2C_ReceiveAck(MPU_PORT,MPU_CLK,MPU_SDA);
	MyI2C_SendByte(MPU_PORT,MPU_CLK,MPU_SDA,data);
	MyI2C_ReceiveAck(MPU_PORT,MPU_CLK,MPU_SDA);
	MyI2C_Stop(MPU_PORT,MPU_CLK,MPU_SDA);
}
uint8_t MPU6050_R_reg(uint8_t regadd){ //指定寄存器地址，读一个Byte
	uint8_t Data;
	
	MyI2C_Start(MPU_PORT,MPU_CLK,MPU_SDA);
	MyI2C_SendByte(MPU_PORT,MPU_CLK,MPU_SDA,MPU6050_ADDRESS<<1);
	MyI2C_ReceiveAck(MPU_PORT,MPU_CLK,MPU_SDA);
	MyI2C_SendByte(MPU_PORT,MPU_CLK,MPU_SDA,regadd);
	MyI2C_ReceiveAck(MPU_PORT,MPU_CLK,MPU_SDA);
	
	MyI2C_Start(MPU_PORT,MPU_CLK,MPU_SDA);
	MyI2C_SendByte(MPU_PORT,MPU_CLK,MPU_SDA,(MPU6050_ADDRESS<<1)|0x01);
	MyI2C_ReceiveAck(MPU_PORT,MPU_CLK,MPU_SDA);
	Data = MyI2C_ReceiveByte(MPU_PORT,MPU_CLK,MPU_SDA);
	MyI2C_SendAck(MPU_PORT,MPU_CLK,MPU_SDA,1);
	MyI2C_Stop(MPU_PORT,MPU_CLK,MPU_SDA);

	return Data;
}

void MPU6050_Config_Init(MY_MPU6050_InitTpyeDef *this){
	MPU6050_W_reg(MPU6050_SMPLRT_DIV,this->SMPLRT_DIV);
	MPU6050_W_reg(MPU6050_CONFIG,this->Filter);
	MPU6050_W_reg(MPU6050_GYRO_CONFIG,this->Gyro);
	MPU6050_W_reg(MPU6050_ACCEL_CONFIG,this->Acc);
	MPU6050_W_reg(MPU6050_FIFO_EN,this->Fifo);
	MPU6050_W_reg(MPU6050_PWR_MGMT_1,0x00);
}

void MPU6050_Init(void){
	MyI2C_Init();
	MY_MPU6050_InitTpyeDef mpu6050init;
	mpu6050init.SMPLRT_DIV = 1;
	mpu6050init.Filter = Band_5Hz;
	mpu6050init.Gyro = gyro_250;
	mpu6050init.Acc = acc_2g;
	mpu6050init.Fifo = FIFO_Disable;
	MPU6050_Config_Init(&mpu6050init);
}

void MPU6050_Get_Raws(MPU6050_raw* this){
	this->AccX 	= ((int16_t)MPU6050_R_reg(MPU6050_ACCEL_XOUT_H))<<8 | MPU6050_R_reg(MPU6050_ACCEL_XOUT_L);
	this->AccY 	= ((int16_t)MPU6050_R_reg(MPU6050_ACCEL_YOUT_H))<<8 | MPU6050_R_reg(MPU6050_ACCEL_YOUT_L);
	this->AccZ 	= ((int16_t)MPU6050_R_reg(MPU6050_ACCEL_ZOUT_H))<<8 | MPU6050_R_reg(MPU6050_ACCEL_ZOUT_L);
	this->GyroX = ((int16_t)MPU6050_R_reg(MPU6050_GYRO_XOUT_H))<<8 | MPU6050_R_reg(MPU6050_GYRO_XOUT_L);
	this->GyroY = ((int16_t)MPU6050_R_reg(MPU6050_GYRO_YOUT_H))<<8 | MPU6050_R_reg(MPU6050_GYRO_YOUT_L);
	this->GyroZ = ((int16_t)MPU6050_R_reg(MPU6050_GYRO_ZOUT_H))<<8 | MPU6050_R_reg(MPU6050_GYRO_ZOUT_L);
	this->Temp 	= ((int16_t)MPU6050_R_reg(MPU6050_TEMP_OUT_H))<<8 | MPU6050_R_reg(MPU6050_TEMP_OUT_L);
}
float MY_MPU6050_GetTemp(void){
	uint8_t temp = ((int16_t)MPU6050_R_reg(MPU6050_TEMP_OUT_H))<<8 | MPU6050_R_reg(MPU6050_TEMP_OUT_L);
	float temperature = (float)temp/340 + 36.53;
	return temperature;
}

//MPU6050_Angle* this
void MPU6050_Get_Angles(float* Pitch_Angle){
	int16_t temp=0;
	float Ax,Ay,Az=0;
	float Gx,Gy,Gz=0;
	static float Gyroscope_roll=0;
	static float Gyroscope_pitch=0;
    const static float dt=0.005;
    const static float weight = 0.95;//权重

    //static int roll_befor = 0;
    //static int pitch_befor = 0;
	
	temp = ((int16_t)MPU6050_R_reg(MPU6050_ACCEL_XOUT_H))<<8 | MPU6050_R_reg(MPU6050_ACCEL_XOUT_L);
	Ax = temp * 16.0/32768;
	
	temp = ((int16_t)MPU6050_R_reg(MPU6050_ACCEL_YOUT_H))<<8 | MPU6050_R_reg(MPU6050_ACCEL_YOUT_L);
	Ay = temp * 16.0/32768;
	
	temp = ((int16_t)MPU6050_R_reg(MPU6050_ACCEL_ZOUT_H))<<8 | MPU6050_R_reg(MPU6050_ACCEL_ZOUT_L);
	Az = temp * 16.0/32768;
	
	temp = ((int16_t)MPU6050_R_reg(MPU6050_GYRO_XOUT_H))<<8 | MPU6050_R_reg(MPU6050_GYRO_XOUT_L);
	Gx = temp *dt*0.0174533;
	
	temp = ((int16_t)MPU6050_R_reg(MPU6050_GYRO_YOUT_H))<<8 | MPU6050_R_reg(MPU6050_GYRO_YOUT_L);
	Gy = temp *dt*0.0174533;
	
	temp = ((int16_t)MPU6050_R_reg(MPU6050_GYRO_ZOUT_H))<<8 | MPU6050_R_reg(MPU6050_GYRO_ZOUT_L);
	Gz = temp *dt*0.0174533;
	

	Gyroscope_roll+=Gy;
	Gyroscope_pitch+=Gx;
	*Pitch_Angle=(weight * atan2(Ax,Az)/3.1415926 * 180 + (1-weight) * Gyroscope_pitch);
	
	(void)Ay;
	(void)Gz;
	//this->roll=weight * atan2(Ay,Az)/3.1415926 * 180 + (1-weight) * Gyroscope_roll;
	//this->pitch=-(weight * atan2(Ax,Az)/3.1415926 * 180 + (1-weight) * Gyroscope_pitch);
	//this->yaw += Gz*20+0.0157;//减小零飘

    //this->roll = (this->roll+roll_befor)/2;
    //this->pitch = (this->pitch+pitch_befor)/2;
    //roll_befor = this->roll;
    //pitch_befor = this->pitch;
}

