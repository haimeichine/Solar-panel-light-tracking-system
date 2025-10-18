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
#include "QMC_5883.h"
#define QMC_PORT GPIOA
#define QMC_CLK GPIO_Pin_6
#define QMC_SDA GPIO_Pin_7

///////////////////////////////////////////qmc5883
typedef enum{
	Standby = 0x00,
	Continuous = 0x01
}Mode_Selection;

typedef enum{
	Speed_10Hz = 0x00,
	Speed_50Hz = 0x04,
	Speed_100Hz = 0x08,
	Speed_200Hz = 0x0C
}ODR_Selection;

typedef enum{
	Full_Scale_2G = 0x00,
	Full_Scale_8G = 0x10
}RNG_Selection;

typedef enum{
	Over_Sample_Ratio_512 = 0x00,
	Over_Sample_Ratio_256 = 0x40,
	Over_Sample_Ratio_128 = 0x80,
	Over_Sample_Ratio_64 = 0xC0
}OSR_Selection;


typedef struct QMC5883_InitTpyeDef{
	Mode_Selection Mode;
	ODR_Selection ODR;
	OSR_Selection OSR;
	RNG_Selection RNG;
}QMC5883_InitTpyeDef;

uint8_t QMC5883_R_reg(uint8_t regadd){ //指定寄存器地址，读一个Byte
	uint8_t Data;
	MyI2C_Start(QMC_PORT,QMC_CLK,QMC_SDA);
	MyI2C_SendByte(QMC_PORT,QMC_CLK,QMC_SDA,MQC5883_ADDRESS << 1);
	MyI2C_ReceiveAck(QMC_PORT,QMC_CLK,QMC_SDA);
	MyI2C_SendByte(QMC_PORT,QMC_CLK,QMC_SDA,regadd);
	MyI2C_ReceiveAck(QMC_PORT,QMC_CLK,QMC_SDA);
	
	MyI2C_Start(QMC_PORT,QMC_CLK,QMC_SDA);
	MyI2C_SendByte(QMC_PORT,QMC_CLK,QMC_SDA,MQC5883_ADDRESS<<1 | 0x01);
	MyI2C_ReceiveAck(QMC_PORT,QMC_CLK,QMC_SDA);
	Data = MyI2C_ReceiveByte(QMC_PORT,QMC_CLK,QMC_SDA);
	MyI2C_SendAck(QMC_PORT,QMC_CLK,QMC_SDA,1);
	MyI2C_Stop(QMC_PORT,QMC_CLK,QMC_SDA);

	return Data;
}
void QMC5883_W_reg(uint8_t regadd,uint8_t data){ //指定寄存器地址，写一个Byte
	MyI2C_Start(QMC_PORT,QMC_CLK,QMC_SDA);
	MyI2C_SendByte(QMC_PORT,QMC_CLK,QMC_SDA,MQC5883_ADDRESS<<1);
	MyI2C_ReceiveAck(QMC_PORT,QMC_CLK,QMC_SDA);
	MyI2C_SendByte(QMC_PORT,QMC_CLK,QMC_SDA,regadd);
	MyI2C_ReceiveAck(QMC_PORT,QMC_CLK,QMC_SDA);
	MyI2C_SendByte(QMC_PORT,QMC_CLK,QMC_SDA,data);
	MyI2C_ReceiveAck(QMC_PORT,QMC_CLK,QMC_SDA);
	MyI2C_Stop(QMC_PORT,QMC_CLK,QMC_SDA);
}
void QMC5883_Get_Data(QMC5883* this){
	this->XMag = (QMC5883_R_reg(QMC5883_X_MSB) << 8) | QMC5883_R_reg(QMC5883_X_LSB);
	this->YMag = (QMC5883_R_reg(QMC5883_Y_MSB) << 8) | QMC5883_R_reg(QMC5883_Y_LSB);
	this->ZMag = (QMC5883_R_reg(QMC5883_Z_MSB) << 8) | QMC5883_R_reg(QMC5883_Z_LSB);
}
void QMC5883_ConfigInit(QMC5883_InitTpyeDef* this){
	uint8_t Data = this->Mode | this->ODR | this->OSR | this->RNG;
	QMC5883_W_reg(QMC5883_Control,Data);
}
void SET_RESET(void){
	QMC5883_W_reg(QMC5883_SET_RESET,0x01);	
}

uint8_t Status_Register(void){
	return QMC5883_R_reg(0x06);
}
uint8_t Chip_ID_Register(void){
	return QMC5883_R_reg(0x0D);
}

static int16_t calibration_x = 0;
static int16_t calibration_y = 0;

float HMC5983_Get_Angle(void){
	QMC5883 HMC;
	float angle=0;
	QMC5883_Get_Data(&HMC);
	float raw_angle = (float)(HMC.YMag - calibration_y)/(float)(HMC.XMag -calibration_x);
	if(HMC.XMag > 0 && HMC.YMag > 0)angle = atan(raw_angle)*180 / 3.141592f;
	else if(HMC.XMag > 0 && HMC.YMag < 0)angle = 360 + atan(raw_angle)*180 / 3.141592f;
	else if(HMC.XMag == 0 && HMC.YMag > 0)angle = 90;
	else if(HMC.XMag == 0 && HMC.YMag < 0)angle = 270;
	else if(HMC.XMag < 0 )angle = 180 + atan(raw_angle)*180/3.141592f;
	if(angle>360)angle=360;
	if(angle<0)angle=0;
	return angle;//如果返回了999那么是错误数据
}
void QMC5883_Init(void){
	MyI2C_Init();
	QMC5883_InitTpyeDef MyQMC5883_InitTpyeDef;
	MyQMC5883_InitTpyeDef.Mode = Continuous;
	MyQMC5883_InitTpyeDef.ODR = Speed_200Hz;
	MyQMC5883_InitTpyeDef.OSR = Over_Sample_Ratio_512;
	MyQMC5883_InitTpyeDef.RNG = Full_Scale_2G;
	QMC5883_ConfigInit(&MyQMC5883_InitTpyeDef);
	SET_RESET();
//	Magnetometer_Calibration();
}

float QMC5883_Get_Emperature(void){
	int16_t Data = (QMC5883_R_reg(QMC5883_Emperature_MSB)<<8) | QMC5883_R_reg(QMC5883_Emperature_LSB);
	return (float)Data/128 + 25;
} 


/////////////////////////////////new
typedef struct {
    float offset_x, offset_y, offset_z;
    float scale_x, scale_y, scale_z;
} CalibParams;
CalibParams params;
QMC5883 my5883;
void Magnetometer_Calibration(void)
{
	static int16_t min_x;									//用于保存边缘的值
	static int16_t max_x;
	static int16_t min_y;
	static int16_t max_y;
	static int16_t min_z;
	static int16_t max_z;
	
	for (int i = 0; i < 500; i++) 
	{
		QMC5883_Get_Data(&my5883);		//获得新的数据
        
        // 更新各轴最大最小值
        if (my5883.XMag < min_x) min_x = my5883.XMag;
        if (my5883.XMag > max_x) max_x = my5883.XMag;
        if (my5883.YMag < min_y) min_y = my5883.YMag;
        if (my5883.YMag > max_y) max_y = my5883.YMag;
        if (my5883.ZMag < min_z) min_z = my5883.ZMag;
        if (my5883.ZMag > max_z) max_z = my5883.ZMag;
    }
	
    params.offset_x = (max_x + min_x) / 2.0f;
    params.offset_y = (max_y + min_y) / 2.0f;
    params.offset_z = (max_z + min_z) / 2.0f;
    
    // 计算软铁比例因子
    float scale_x = (max_x - min_x) / 2.0f;
    float scale_y = (max_y - min_y) / 2.0f;
    float scale_z = (max_z - min_z) / 2.0f;
    
    // 找到最大范围轴
    float max_scale = scale_x;
    if (scale_y > max_scale) max_scale = scale_y;
    if (scale_z > max_scale) max_scale = scale_z;
    
    // 归一化比例因子
    params.scale_x = scale_x / max_scale;
    params.scale_y = scale_y / max_scale;
    params.scale_z = scale_z / max_scale;
	

}

//获得校正后的磁力计数据
void QMC5883_Get_CalibrationData(float *hx,float *hy,float *hz)
{
	QMC5883_Get_Data(&my5883);											//获取磁力数据
//	*hx = ((float)my5883.XMag - params.offset_x) * params.scale_x;								//获得校正后的数据
//	*hy = ((float)my5883.YMag - params.offset_y) * params.scale_y;
//	*hz = ((float)my5883.ZMag - params.offset_z) * params.scale_z;
	*hx = ((float)my5883.XMag - (-1902.500000)) * (1.000000);								//获得校正后的数据
	*hy = ((float)my5883.YMag - (-871.500000)) * (0.970281);
	*hz = ((float)my5883.ZMag - (-2306.000000)) * (0.486036);
//	printf("%f,%f,%f\r\n",params.offset_x,params.offset_y,params.offset_z);					//串口输出三轴角度
//	printf("%f,%f,%f\r\n",params.scale_x,params.scale_y,params.scale_z);					//串口输出三轴角度
}

void QMC5883_Get_Yaw_Angles(float *yaw)
{
	static float temp_hx = 0;
	static float temp_hy = 0;
	static float temp_hz = 0;
	QMC5883_Get_CalibrationData(&temp_hx,&temp_hy,&temp_hz);	//获得校准过的磁力计数据
	float heading_rad = atan2(temp_hy, temp_hx);				//使用磁力计信息获得偏航角
    float heading_deg = heading_rad * (180.0f / pi);			//转换为角度
	if (heading_deg < 0) 
	{
        heading_deg += 360.0f;
    }
	*yaw = heading_deg;
}
















