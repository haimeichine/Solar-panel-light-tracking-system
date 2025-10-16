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







