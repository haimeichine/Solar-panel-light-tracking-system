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
#include "QMC_MPU_I2C.h"

//////////////////////////////////////////
void MyI2C_W_SCL(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_SCL,uint8_t BitValue)//SCL写bit
{
	GPIO_WriteBit(GPIOx, GPIO_Pin_SCL, (BitAction)BitValue);
	Delay_us(10);
}

void MyI2C_W_SDA(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_SDA,uint8_t BitValue)//SDA写bit
{
	GPIO_WriteBit(GPIOx, GPIO_Pin_SDA, (BitAction)BitValue);
	Delay_us(10);
}

uint8_t MyI2C_R_SDA(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_SDA)//SDA读bit
{
	uint8_t BitValue;
	BitValue = GPIO_ReadInputDataBit(GPIOx, GPIO_Pin_SDA);
	Delay_us(10);
	return BitValue;
}

void MyI2C_Start(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_SCL, uint16_t GPIO_Pin_SDA)//开始时许
{
	MyI2C_W_SDA(GPIOx,GPIO_Pin_SDA,1);
	MyI2C_W_SCL(GPIOx,GPIO_Pin_SCL,1);
	MyI2C_W_SDA(GPIOx,GPIO_Pin_SDA,0);
	MyI2C_W_SCL(GPIOx,GPIO_Pin_SCL,0);
}

void MyI2C_Stop(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_SCL, uint16_t GPIO_Pin_SDA)//结束时许
{
	MyI2C_W_SDA(GPIOx,GPIO_Pin_SDA,0);
	MyI2C_W_SCL(GPIOx,GPIO_Pin_SCL,1);
	MyI2C_W_SDA(GPIOx,GPIO_Pin_SDA,1);
}

void MyI2C_SendByte(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_SCL, uint16_t GPIO_Pin_SDA,uint8_t Byte)//发送bit
{
	uint8_t i;
	for (i = 0; i < 8; i++){
		MyI2C_W_SDA(GPIOx,GPIO_Pin_SDA,!!(Byte & (0x80 >> i)));
		MyI2C_W_SCL(GPIOx,GPIO_Pin_SCL,1);
		MyI2C_W_SCL(GPIOx,GPIO_Pin_SCL,0);
	}
}

uint8_t MyI2C_ReceiveByte(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_SCL, uint16_t GPIO_Pin_SDA)//获取bit
{
	uint8_t i, Byte = 0x00;
	MyI2C_W_SDA(GPIOx,GPIO_Pin_SDA,1);
	for (i = 0; i < 8; i++){
		MyI2C_W_SCL(GPIOx,GPIO_Pin_SCL,1);
		if (MyI2C_R_SDA(GPIOx,GPIO_Pin_SDA)){Byte |= (0x80 >> i);}
		MyI2C_W_SCL(GPIOx,GPIO_Pin_SCL,0);
	}
	return Byte;
}

void MyI2C_SendAck(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_SCL, uint16_t GPIO_Pin_SDA, uint8_t AckBit)//发送应答
{
	MyI2C_W_SDA(GPIOx,GPIO_Pin_SDA,AckBit);
	MyI2C_W_SCL(GPIOx,GPIO_Pin_SCL,1);
	MyI2C_W_SCL(GPIOx,GPIO_Pin_SCL,0);
}

uint8_t MyI2C_ReceiveAck(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_SCL, uint16_t GPIO_Pin_SDA)//接收应答
{
	uint8_t AckBit;
	MyI2C_W_SDA(GPIOx,GPIO_Pin_SDA,1);
	MyI2C_W_SCL(GPIOx,GPIO_Pin_SCL,1);
	AckBit = MyI2C_R_SDA(GPIOx,GPIO_Pin_SDA);
	MyI2C_W_SCL(GPIOx,GPIO_Pin_SCL,0);
	return AckBit;
}
void MyI2C_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//时钟使能
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin = QMC_CLK | QMC_SDA;//qmc5883——pin
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(QMC_PORT, &GPIO_InitStructure);
	GPIO_SetBits(QMC_PORT, QMC_CLK | QMC_SDA);
	
	GPIO_InitStructure.GPIO_Pin = MPU_CLK | MPU_SDA;//mpu6050——pin
	GPIO_Init(MPU_PORT, &GPIO_InitStructure);
	GPIO_SetBits(MPU_PORT, MPU_CLK | MPU_SDA);
}
