#include "stm32f10x.h"                  // Device header
#include <stdarg.h>
#include "serial.h"

char Serial_RxPacket[100];				//"@MSG\r\n"
#define USE_printf 1

#if USE_printf 
#pragma import(__use_no_semihosting)
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
};

void _ttywrch(int ch)
{
       // 空实现，避免半主机模式相关的引用错误
}

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){};
    USART_SendData(USART1, (uint8_t)ch);
	return ch;
}
#else 
int fputc(int ch, FILE *f){
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, (uint8_t)ch);
	return ch;
}
#endif 


void Serial_UART_Init(USART_TypeDef* uartx,uint32_t BaudRate)
{
	if(uartx==USART1){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		USART_InitTypeDef USART_InitStructure;
		USART_InitStructure.USART_BaudRate = BaudRate;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_Init(USART1, &USART_InitStructure);
		
//		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
//		
//		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		
//		NVIC_InitTypeDef NVIC_InitStructure;
//		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//		NVIC_Init(&NVIC_InitStructure);
		
		USART_Cmd(USART1, ENABLE);
	}else if(uartx==USART2){
		// 使能 USART2 和 GPIOA 的时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // USART2 在 APB1 总线上
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // GPIOA 在 APB2 总线上

		// 配置 USART2_TX (PA2) 为复用推挽输出
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;       // USART2_TX (PA2)
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		// 配置 USART2_RX (PA3) 为浮空输入或上拉输入
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   // 上拉输入
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;       // USART2_RX (PA3)
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		// 配置 USART2 参数
		USART_InitTypeDef USART_InitStructure;
		USART_InitStructure.USART_BaudRate = BaudRate;                      // 设置波特率
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控制
		USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;     // 启用发送和接收模式
		USART_InitStructure.USART_Parity = USART_Parity_No;                 // 无校验
		USART_InitStructure.USART_StopBits = USART_StopBits_1;              // 1 位停止位
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;         // 8 位数据长度
		USART_Init(USART2, &USART_InitStructure);

		// 使能 USART2 接收中断
		USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);

		// 配置 NVIC（嵌套向量中断控制器）
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置优先级分组

		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;                   // USART2 中断通道
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                     // 使能中断
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;          // 抢占优先级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;                 // 子优先级
		NVIC_Init(&NVIC_InitStructure);

		// 启用 USART2
		USART_Cmd(USART2, ENABLE);
	}
}
//char Serial3_RxPacket[75] = {"$GNRMC,084852.000,A,2424.8014, N,10932.1154, E,0.53,292.44,141216,,,A*7"};
//char Serial3_RxPacket[75] = {"$GNRMC,013033.000,A,2424.29797,N,10931.90133,E,0.00,204.81,040625,,,A*78"};
static uint8_t Stare_Index = 0;
static char Usart2_GPS_Data_Buff[100];
//static char mytestbuff[100] = {"$GNRMC,21700.000,A,2424.35173,N,10931.91035,E,15.96,225.84,230725,,,A*4A"};//学校
//static char mytestbuff[100] = {"$GNRMC,081713.000,A,2417.27638,N,10928.24800,E,2.83,53.36,230725,,,A*43"};//厂
//

#include <string.h>
#include <stdlib.h>
#include "can.h"
void USART2_IRQHandler(void){
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET){
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		uint8_t UsartData = USART_ReceiveData(USART2);
		if(UsartData == '\n'){
			char copy[100];
			strcpy(copy, Usart2_GPS_Data_Buff);
			char *fields[20];  				// 存储每一段
			int i = 0;
			char *token = strtok(copy, ",");
			while (token != NULL && i < 20) {
				fields[i++] = token;
				token = strtok(NULL, ",");
			}
			Utc_Time = fields[1];     		// 时间
			GPS_Status = fields[2];       	// 定位状态
			if(strcmp(GPS_Status, "V") == 0){
				printf("Not_GPS\n");
				GPS_If = 0;
			}else{
				GPS_If = 1;
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
				spa.hour = (int)atof(Utc_Time)/10000+8;
				spa.minute = (int)atof(Utc_Time)/100%100;
				spa.second = (int)atof(Utc_Time)%100;
				spa.timezone = 8; 		//时区偏移UTC+8
				spa.latitude = GPS_N;   //纬度北纬为正
				spa.longitude = GPS_E; 	//经度东经为正
				spa_calculate(&spa); 	//调用计算函数
				Solar_Azimuth = spa.azimuth;				//太阳方位角
				Solar_Altitude = 90.0 - spa.zenith;			//太阳高度角
				
//				sprintf(date_buff,"20%d",(int)atof(date)%100);
//				printf("UTC:a=%d,b=%d,c=%d h=%d,m=%d,s=%d\n",(int)atof(date_buff),(int)atof(date)/100%100,(int)atof(date)/10000,(int)atof(Utc_Time)/10000,(int)atof(Utc_Time)/100%100,(int)atof(Utc_Time)%100);
//				printf("GCJ-02经度: %f\n", GPS_E);
//				printf("GCJ-02纬度: %f\n", GPS_N);
//				printf("方位角：%f\n",(Solar_Azimuth));
//				printf("高度角：%f\n",Solar_Altitude);
			}
			USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
			Stare_Index=0;
			(void)Usart2_GPS_Data_Buff;
		}else {
			Usart2_GPS_Data_Buff[Stare_Index++] = UsartData;
		}
	}
}
//void USART2_IRQHandler(void){
//	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET){
//		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
//		uint8_t UsartData = USART_ReceiveData(USART2);
//		
//	}
//}
