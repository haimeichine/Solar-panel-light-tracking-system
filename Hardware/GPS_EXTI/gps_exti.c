#include "GPS_EXTI.h"
#include "QMC_5883.h"


void GPS_EXTI_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitTypeDef My_GPIO_Init;
	My_GPIO_Init.GPIO_Mode = GPIO_Mode_IPU;
	My_GPIO_Init.GPIO_Pin = GPIO_Pin_5;
	My_GPIO_Init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&My_GPIO_Init);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource5);
	
	EXTI_InitTypeDef GPS_EXTI;
	GPS_EXTI.EXTI_Line = EXTI_Line5;
	GPS_EXTI.EXTI_LineCmd = ENABLE;
	GPS_EXTI.EXTI_Mode = EXTI_Mode_Interrupt;
	GPS_EXTI.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&GPS_EXTI);
	
	NVIC_InitTypeDef My_NVIC;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	My_NVIC.NVIC_IRQChannel = EXTI9_5_IRQn;
	My_NVIC.NVIC_IRQChannelCmd = ENABLE;
	My_NVIC.NVIC_IRQChannelPreemptionPriority = 2;
	My_NVIC.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&My_NVIC);
}

void EXTI9_5_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line5) != 0){
		EXTI_ClearITPendingBit(EXTI_Line5);
        if (ifEXTI == 1){
			ifEXTI = 0;
			USART_Cmd(USART2, ENABLE);
		}
		else{
			ifEXTI_index=0;
			Yaw_Angle++;
		}
    }
}

