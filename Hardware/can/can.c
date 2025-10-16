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

