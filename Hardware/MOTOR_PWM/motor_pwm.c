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
#include "motor_pwm.h"

void MOTOR_Yaw_Pitch_Init(void){
	Yaw_Pwm_Init();
	Pitch_Pwm_Init();
	EN_Motor_Init();
}

void Yaw_Pwm_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct; //定义GPIO结构体
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;	//定义TIM时基单元结构体
	TIM_TimeBaseStruct.TIM_Prescaler = 720 - 1;	//时基单元的预分频器（72/value）
	TIM_TimeBaseStruct.TIM_Period = 100;		//自动重装值
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式
	TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;	//TIM时钟不进行分频
	//TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;			//高级定时器中有用
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStruct);				//初始化咯
	
	TIM_OCInitTypeDef TIM_OCStruct;				//定义TIM输出比较结构体
	TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM1;	//PWM模式1
	TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;//输出状态使能
	//TIM_OCStruct.TIM_OutputNState = TIM_OutputNState_Enable; //互补输出使能
	TIM_OCStruct.TIM_Pulse = 0;//占空比我们以后自己手动调，所以现在给零
	TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_High;//输出极性：高电平有效
	//TIM_OCStruct.TIM_OCNPolarity = TIM_OCNPolarity_Low; // 互补输出极性：低电平有效
	
	TIM_OC1Init(TIM2,&TIM_OCStruct);//初始化咯
	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);//预装载寄存器使能
	
	TIM_OC2Init(TIM2,&TIM_OCStruct);//初始化咯
	TIM_OC2PreloadConfig(TIM2,TIM_OCPreload_Enable);//预装载寄存器使能
	
	TIM_Cmd(TIM2, ENABLE);
	 
	//启动主输出（必须调用，否则无法输出PWM）
    TIM_CtrlPWMOutputs(TIM2, ENABLE);
	// 确保你有如下配置：
	
}
void Pitch_Pwm_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct; //定义GPIO结构体
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;	//定义TIM时基单元结构体
	TIM_TimeBaseStruct.TIM_Prescaler = 720 - 1;	//时基单元的预分频器（72/value）
	TIM_TimeBaseStruct.TIM_Period = 100;		//自动重装值
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式
	TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;	//TIM时钟不进行分频
	//TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;			//高级定时器中有用
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStruct);				//初始化咯
	
	TIM_OCInitTypeDef TIM_OCStruct;				//定义TIM输出比较结构体
	TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM1;	//PWM模式1
	TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;//输出状态使能
	//TIM_OCStruct.TIM_OutputNState = TIM_OutputNState_Enable; //互补输出使能
	TIM_OCStruct.TIM_Pulse = 0;//占空比我们以后自己手动调，所以现在给零
	TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_High;//输出极性：高电平有效
	//TIM_OCStruct.TIM_OCNPolarity = TIM_OCNPolarity_Low; // 互补输出极性：低电平有效
	
	TIM_OC3Init(TIM4,&TIM_OCStruct);//初始化咯
	TIM_OC3PreloadConfig(TIM4,TIM_OCPreload_Enable);//预装载寄存器使能
	
	TIM_OC4Init(TIM4,&TIM_OCStruct);//初始化咯
	TIM_OC4PreloadConfig(TIM4,TIM_OCPreload_Enable);//预装载寄存器使能
	
	TIM_Cmd(TIM4, ENABLE);
	 
	//启动主输出（必须调用，否则无法输出PWM）
    TIM_CtrlPWMOutputs(TIM4, ENABLE);
	// 确保你有如下配置：
}

void EN_Motor_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
}
void Yaw_Motor_Pwm_1(uint16_t value){
	TIM_SetCompare1(TIM2,value);
}
void Yaw_Motor_Pwm_2(uint16_t value){
	TIM_SetCompare2(TIM2,value);
}
void Pitch_Motor_Pwm_1(uint16_t value){
	TIM_SetCompare3(TIM4,value);
}
void Pitch_Motor_Pwm_2(uint16_t value){
	TIM_SetCompare4(TIM4,value);
}
void EN_Yaw_Motor(BitAction  bit){
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,bit);
}
void EN_Pitch_Motor(BitAction  bit){
	GPIO_WriteBit(GPIOA,GPIO_Pin_5,bit);
}
/*
	偏航控制
*/
void Yaw_Motor_Left(uint8_t speed){
	EN_Yaw_Motor(Bit_RESET);
	Yaw_Motor_Pwm_1(speed);
	Yaw_Motor_Pwm_2(101);
}
void Yaw_Motor_Rigth(uint8_t speed){
	EN_Yaw_Motor(Bit_RESET);
	Yaw_Motor_Pwm_1(101);
	Yaw_Motor_Pwm_2(speed);
}
void Yaw_Motor_Stop(void){
	EN_Yaw_Motor(Bit_RESET);
	Yaw_Motor_Pwm_1(101);
	Yaw_Motor_Pwm_2(101);
}
void Yaw_Motor_Auto(void){
	EN_Yaw_Motor(Bit_SET);
	Yaw_Motor_Pwm_1(101);
	Yaw_Motor_Pwm_2(101);
}

/*
	俯仰控制
*/
void Pitch_Motor_Left(uint8_t speed){
	EN_Yaw_Motor(Bit_RESET);
	Pitch_Motor_Pwm_1(speed);
	Pitch_Motor_Pwm_2(101);
}
void Pitch_Motor_Rigth(uint8_t speed){
	EN_Yaw_Motor(Bit_RESET);
	Pitch_Motor_Pwm_1(101);
	Pitch_Motor_Pwm_2(speed);
}
void Pitch_Motor_Stop(void){
	EN_Yaw_Motor(Bit_RESET);
	Pitch_Motor_Pwm_1(101);
	Pitch_Motor_Pwm_2(101);
}
void Pitch_Motor_Auto(void){
	EN_Yaw_Motor(Bit_SET);
	Pitch_Motor_Pwm_1(101);
	Pitch_Motor_Pwm_2(101);
}







