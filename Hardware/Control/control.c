#include "control.h"


void Control_Photosensitive(void){
	MPU6050_Get_Angles(&Pitch_Angle);
	QMC5883_Get_Yaw_Angles(&Yaw_Angle);
	Get_Can_ADC_Data();
//	if(Pitch_Angle > 80){
//		
//	}else{
//		Yaw_Motor_Rigth(YAW_SPEED);
		///Æ«º½Yaw
		float Yaw_Sum = (ADC_Data1+ADC_Data3)-(ADC_Data2+ADC_Data4);
//		if(Yaw_Sum>0.5){
//			Yaw_Motor_Rigth(YAW_SPEED);
//		}else if(Yaw_Sum<-0.5){
//			Yaw_Motor_Left(YAW_SPEED);
//		}else{
//			Yaw_Motor_Stop();
//		}
		
		//¸©ÑöPitch
		
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
	//Æ«º½Yaw
	if(Solar_Azimuth > Yaw_Angle + Yaw_stop){	Yaw_Motor_Rigth(Pitch_SPEED);}
	else if(Solar_Azimuth < Yaw_Angle - Yaw_stop){Yaw_Motor_Left(Pitch_SPEED);}
	else{Yaw_Motor_Stop();}
	//¸©ÑöPitch£¬&& Pitch_Angle > 26£¬&& Pitch_Angle < 79
	if(Solar_Altitude > Pitch_Angle + Pitch_stop ){Pitch_Motor_Rigth(Pitch_SPEED);
	}else if(Solar_Altitude < Pitch_Angle - Pitch_stop ){Pitch_Motor_Left(Pitch_SPEED);
	}else{Pitch_Motor_Stop();}
}
void Control_test(void){
	uint8_t Yaw_stop = 2;
	uint8_t Pitch_stop = 2;
	int Yaw_Motor_Stop_if = 0;
	int Pitch_Motor_Stop_if = 0;
	//Æ«º½Yaw
	if(Solar_Azimuth > Yaw_Angle + Yaw_stop){
		Yaw_Motor_Rigth(Pitch_SPEED);
	}else if(Solar_Azimuth < Yaw_Angle - Yaw_stop){
		Yaw_Motor_Left(Pitch_SPEED);
	}else{
		Yaw_Motor_Stop();
		Yaw_Motor_Stop_if = 1;
	}
	
	//¸©ÑöPitch£¬&& Pitch_Angle > 26£¬&& Pitch_Angle < 79
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
