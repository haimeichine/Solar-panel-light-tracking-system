#ifndef __CAN_H__
#define __CAN_H__

#include "stm32f10x.h"  
#include "stdio.h"
#include "Delay.h"

extern float ADC_Data1,ADC_Data2,ADC_Data3,ADC_Data4;

void ADC_Index_if(void);

void MyCAN_Init(void);
void MyCAN_Transmit(CanTxMsg *TxMessage);
uint8_t MyCAN_ReceiveFlag(void);
void MyCAN_Receive(CanRxMsg *RxMessage);
void Get_Can_ADC_Data(void);
#endif
