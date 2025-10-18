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
#include "stm32f10x.h"                  // Device header
#include <stdarg.h>
#include "serial.h"
#include <string.h>
#include <stdlib.h>
#include "can.h"
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

static uint8_t Stare_Index = 0;
static char Usart2_GPS_Data_Buff[100];

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
				GPS_zhuanhuang(Latitude,Longitude);	// 计算经度分秒纬度转换成火星坐标系
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
				spa_calculate(&spa); 	//调用计算函数技术按方位角
				Solar_Azimuth = spa.azimuth;				//太阳方位角
				Solar_Altitude = 90.0 - spa.zenith;			//太阳高度角
				
//				sprintf(date_buff,"20%d",(int)atof(date)%100);
//				printf("UTC:a=%d,b=%d,c=%d h=%d,m=%d,s=%d\n",(int)atof(date_buff),(int)atof(date)/100%100,(int)atof(date)/10000,(int)atof(Utc_Time)/10000,(int)atof(Utc_Time)/100%100,(int)atof(Utc_Time)%100);
//				printf("GCJ-02经度: %f\n", GPS_E);
//				printf("GCJ-02纬度: %f\n", GPS_N);
//				printf("方位角：%f\n",Solar_Azimuth);
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

