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
#include "can.h"                  // Device header

//can 放送结构体数据
CanTxMsg TxMsg_ADC_DATA = {
	.StdId = 111,				
	.IDE = CAN_Id_Standard		//
};
CanRxMsg RxMsg;


void MyCAN_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	CAN_InitTypeDef CAN_InitStructure;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;//CAN_Mode_LoopBack/CAN_Mode_Normal
	CAN_InitStructure.CAN_Prescaler = 48;		//波特率 = 36M / 48 / (1 + 2 + 3) = 125K
	CAN_InitStructure.CAN_BS1 = CAN_BS1_2tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
	CAN_InitStructure.CAN_SJW = CAN_SJW_2tq;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_Init(CAN1, &CAN_InitStructure);
	
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
}


void MyCAN_Transmit(CanTxMsg *TxMessage)
{
	uint8_t TransmitMailbox = CAN_Transmit(CAN1, TxMessage);
	
	uint32_t Timeout = 0;
	while (CAN_TransmitStatus(CAN1, TransmitMailbox) != CAN_TxStatus_Ok)
	{
		Timeout ++;
		if (Timeout > 100000)
		{
			break;
		}
	}
}

uint8_t MyCAN_ReceiveFlag(void)
{
	if (CAN_MessagePending(CAN1, CAN_FIFO0) > 0)
	{
		return 1;
	}
	return 0;
}

void MyCAN_Receive(CanRxMsg *RxMessage)
{
	CAN_Receive(CAN1, CAN_FIFO0, RxMessage);
}

void ADC_Index_if(void){
	if		(TxMsg_ADC_DATA.StdId == 111){TxMsg_ADC_DATA.StdId = 222;}
	else if	(TxMsg_ADC_DATA.StdId == 222){TxMsg_ADC_DATA.StdId = 333;}
	else if	(TxMsg_ADC_DATA.StdId == 333){TxMsg_ADC_DATA.StdId = 444;}
	else if	(TxMsg_ADC_DATA.StdId == 444){TxMsg_ADC_DATA.StdId = 111;}
	else	{TxMsg_ADC_DATA.StdId = 111;}
}

void Get_Can_ADC_Data(void){
	for(int i =0;i<4;i++){
		ADC_Index_if();
		MyCAN_Transmit(&TxMsg_ADC_DATA);
		if (MyCAN_ReceiveFlag()){
				MyCAN_Receive(&RxMsg);
				if (RxMsg.RTR == CAN_RTR_Data)
				{
					switch(RxMsg.StdId){
						case 111:
							ADC_Data1 = (((uint16_t)RxMsg.Data[0]<<8)|RxMsg.Data[1]) / 4095.f *3.3;
							ADC_Data1 += (((uint16_t)RxMsg.Data[2]<<8)|RxMsg.Data[3]) / 4095.f *3.3;
							ADC_Data1 += (((uint16_t)RxMsg.Data[4]<<8)|RxMsg.Data[5]) / 4095.f *3.3;
							break;
						case 222:
							ADC_Data2 = (((uint16_t)RxMsg.Data[0]<<8)|RxMsg.Data[1]) / 4095.f *3.3;
							ADC_Data2 += (((uint16_t)RxMsg.Data[2]<<8)|RxMsg.Data[3]) / 4095.f *3.3;
							ADC_Data2 += (((uint16_t)RxMsg.Data[4]<<8)|RxMsg.Data[5]) / 4095.f *3.3;
							break;
						case 333:
							ADC_Data3 = (((uint16_t)RxMsg.Data[0]<<8)|RxMsg.Data[1]) / 4095.f *3.3;
							ADC_Data3 += (((uint16_t)RxMsg.Data[2]<<8)|RxMsg.Data[3]) / 4095.f *3.3;
							ADC_Data3 += (((uint16_t)RxMsg.Data[4]<<8)|RxMsg.Data[5]) / 4095.f *3.3;
							break;
						case 444:
							ADC_Data4 = (((uint16_t)RxMsg.Data[0]<<8)|RxMsg.Data[1]) / 4095.f *3.3;
							ADC_Data4 += (((uint16_t)RxMsg.Data[2]<<8)|RxMsg.Data[3]) / 4095.f *3.3;
							ADC_Data4 += (((uint16_t)RxMsg.Data[4]<<8)|RxMsg.Data[5]) / 4095.f *3.3;
							break;
					}
				}
			}
			//Delay_ms(100);
	}
}

