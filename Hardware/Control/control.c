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
#include "control.h"


void Control_Photosensitive(void){
	MPU6050_Get_Angles(&Pitch_Angle);
	QMC5883_Get_Yaw_Angles(&Yaw_Angle);
	Get_Can_ADC_Data();
//	if(Pitch_Angle > 80){
//		
//	}else{
//		Yaw_Motor_Rigth(YAW_SPEED);
		///偏航Yaw
		float Yaw_Sum = (ADC_Data1+ADC_Data3)-(ADC_Data2+ADC_Data4);
//		if(Yaw_Sum>0.5){
//			Yaw_Motor_Rigth(YAW_SPEED);
//		}else if(Yaw_Sum<-0.5){
//			Yaw_Motor_Left(YAW_SPEED);
//		}else{
//			Yaw_Motor_Stop();
//		}
		
		//俯仰Pitch
		
		float Pitch_Sum = (ADC_Data1+ADC_Data2)-(ADC_Data3+ADC_Data4);
//		if(Pitch_Sum>0.5){
//			Pitch_Motor_Rigth(Pitch_SPEED);
//		}else if(Pitch_Sum<-0.5){
//			Pitch_Motor_Left(Pitch_SPEED);
//		}else{
//			Pitch_Motor_Stop();
//		}
		//printf("ADC1=%.2f,ADC2=%.2f,ADC3=%.2f,ADC4=%.2f\n",ADC_Data1,ADC_Data2,ADC_Data3,ADC_Data4);
		printf("Yaw=%.2f, Pitch=%.2f\n",Yaw_Sum,Pitch_Sum);	
//   	}
}
void Control_Gps(void){
	uint8_t Yaw_stop = 2;
	uint8_t Pitch_stop = 2;
	//偏航Yaw
	if(Solar_Azimuth > Yaw_Angle + Yaw_stop){	Yaw_Motor_Rigth(Pitch_SPEED);}
	else if(Solar_Azimuth < Yaw_Angle - Yaw_stop){Yaw_Motor_Left(Pitch_SPEED);}
	else{Yaw_Motor_Stop();}
	//俯仰Pitch，&& Pitch_Angle > 26，&& Pitch_Angle < 79
	if(Solar_Altitude > Pitch_Angle + Pitch_stop ){Pitch_Motor_Rigth(Pitch_SPEED);
	}else if(Solar_Altitude < Pitch_Angle - Pitch_stop ){Pitch_Motor_Left(Pitch_SPEED);
	}else{Pitch_Motor_Stop();}
}
void Control_test(void){
	uint8_t Yaw_stop = 2;
	uint8_t Pitch_stop = 2;
	int Yaw_Motor_Stop_if = 0;
	int Pitch_Motor_Stop_if = 0;
	//偏航Yaw
	if(Solar_Azimuth > Yaw_Angle + Yaw_stop){
		Yaw_Motor_Rigth(Pitch_SPEED);
	}else if(Solar_Azimuth < Yaw_Angle - Yaw_stop){
		Yaw_Motor_Left(Pitch_SPEED);
	}else{
		Yaw_Motor_Stop();
		Yaw_Motor_Stop_if = 1;
	}
	
	//俯仰Pitch，&& Pitch_Angle > 26，&& Pitch_Angle < 79
	if(Solar_Altitude > Pitch_Angle + Pitch_stop && Pitch_Angle < 75.0){
		Pitch_Motor_Rigth(Pitch_SPEED);
	}else if(Solar_Altitude < Pitch_Angle - Pitch_stop && Pitch_Angle  > 26.0){
		Pitch_Motor_Left(Pitch_SPEED);
	}else{
		Pitch_Motor_Stop();
		Pitch_Motor_Stop_if = 1;
	}
	if(Pitch_Motor_Stop_if==1&&Yaw_Motor_Stop_if==1)
	{
		zg_return = 0;
	}
}
