#include "heartbeat.h"


void HeartBeat(void){
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);			//开启TIM2的时钟
	
	/*配置时钟源*/
	TIM_InternalClockConfig(TIM3);		//选择TIM2为内部时钟，若不调用此函数，TIM默认也为内部时钟
	
	/*时基单元初始化*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				//定义结构体变量
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//计数器模式，选择向上计数
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200- 1;				//预分频器，即PSC的值
	TIM_TimeBaseInitStructure.TIM_Period = 10000 - 1;				//计数周期，即ARR的值
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;			//重复计数器，高级定时器才会用到
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);				//将结构体变量交给TIM_TimeBaseInit，配置TIM2的时基单元	
	
	/*中断输出配置*/
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);						//清除定时器更新标志位
																//TIM_TimeBaseInit函数末尾，手动产生了更新事件
																//若不清除此标志位，则开启中断后，会立刻进入一次中断
																//如果不介意此问题，则不清除此标志位也可
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);					//开启TIM2的更新中断
	
	/*NVIC中断分组*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				//配置NVIC为分组2
																//即抢占优先级范围：0~3，响应优先级范围：0~3
																//此分组配置在整个工程中仅需调用一次
																//若有多个中断，可以把此代码放在main函数内，while循环之前
																//若调用多次配置分组的代码，则后执行的配置会覆盖先执行的配置
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;						//定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;				//选择配置NVIC的TIM2线
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//指定NVIC线路的抢占优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//指定NVIC线路的响应优先级为1
	NVIC_Init(&NVIC_InitStructure);								//将结构体变量交给NVIC_Init，配置NVIC外设
	
	/*TIM使能*/
	TIM_Cmd(TIM3, ENABLE);			//使能TIM2，定时器开始运行
}
#include "qmc_5883.h"
#include "my_mpu6050.h"
#include "can.h"
#include "motor_pwm.h"
#include "control.h"
#include <string.h>
#include <stdlib.h>
static char mytestbuff[100] = {"$GNRMC,021700.000,A,2424.35173,N,10931.91035,E,15.96,225.84,230725,,,A*4A"};//学校
int my_h = 6;
int my_m = 0;
int zg_return = 0;
/* 定时器中断函数*/
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != 0)
	{	
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		///
		if(zg_return){
			my_h = 6;
			my_m = 0;
		}else{
			if(my_m == 60){
				my_m = 0;
				if(my_h >= 18){
					my_h = 6;
					zg_return = 1;
				}
				if (my_h>24){
					my_h = 0;
				}else{
					my_h++;
				}
			}else{
				my_m += 5;
			}
		}
		/*获取数据*/
		MPU6050_Get_Angles(&Pitch_Angle); 				//获取当前太阳能板的俯仰，方位
		QMC5883_Get_Yaw_Angles(&Yaw_Angle);
//		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	//开启中断获取GPS高度角，方位角
		
		
		
		/*控制*/
//		Control_Gps();
//		Get_Can_ADC_Data();
		char copy[100];
		strcpy(copy, mytestbuff);
		char *fields[20];  				// 存储每一段
		int i = 0;
		char *token = strtok(copy, ",");
		while (token != NULL && i < 20) {
			fields[i++] = token;
			token = strtok(NULL, ",");
		}
		Utc_Time = fields[1];     		// 时间
		GPS_Status = fields[2];       	// 定位状态
		Latitude = fields[3];     	// 纬度
		ns = fields[4];           	// 南北
		Longitude = fields[5];    	// 经度
		ew = fields[6];           	// 东西
		date = fields[9];         	// 日期
		GPS_zhuanhuang();			// 计算经纬度
		//220725
		char date_buff[10];
		sprintf(date_buff,"20%d",(int)atof(date)%100);
		spa.year = (int)atof(date_buff);
		spa.month = (int)atof(date)/100%100;
		spa.day = (int)atof(date)/10000;
		spa.hour = my_h;
		spa.minute = my_m;
		spa.second = (int)atof(Utc_Time)%100;
		spa.timezone = 8; 		//时区偏移UTC+8
		spa.latitude = GPS_N;   //纬度北纬为正
		spa.longitude = GPS_E; 	//经度东经为正
		spa_calculate(&spa); 	//计算太阳方位角，高度角
		Solar_Azimuth = spa.azimuth;		//太阳方位角
		Solar_Altitude = 90.0 - spa.zenith;	//太阳高度角
		
		
		
		
		/*变量*/
		Yaw_Angle = fmod((int)Yaw_Angle+270,360); 	//当前偏航
		Pitch_Angle = Pitch_Angle;					//当前俯仰
		Solar_Azimuth = Solar_Azimuth;				//目标偏航
		Solar_Altitude = Solar_Altitude;			//目标俯仰
		
		
		
		
		
		Control_test();
		
		/*监视*/
		printf("目标高度：%-2d°----目标方位角度：%d°\n",(int)Solar_Altitude,(int)Solar_Azimuth);
		printf("当前高度：%-2d°----当前方位角度：%d°\n",(int)Pitch_Angle,(int)Yaw_Angle);
//		printf("%s\n",Utc_Time);
//		printf("%d/%d/%d\n",spa.day,spa.month,spa.year);
		printf("%d:%d\n",(int)spa.hour,(int)spa.minute);//6:26/6:56
//		printf("GCJ-02经度: %f\n", GPS_E);
//		printf("GCJ-02纬度: %f\n", GPS_N);
		printf("\n");
//		printf("当前方位角度：%d°\n",(int)Yaw_Angle);
		
		/*调试*/
//		printf("ADC1=%.2f,ADC2=%.2f,ADC3=%.2f,ADC4=%.2f\n",ADC_Data1,ADC_Data2,ADC_Data3,ADC_Data4);
//    Control_Photosensitive();
//		Pitch_Motor_Rigth(80);
//		Pitch_Motor_Left(Pitch_SPEED);
		//Yaw_Motor_Rigth(5);
		//Yaw_Motor_Left(Pitch_SPEED);
		
	}
}


