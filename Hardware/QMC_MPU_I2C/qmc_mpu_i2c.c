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
