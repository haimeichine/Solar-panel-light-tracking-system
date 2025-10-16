#ifndef __GPS_EXTI_H__
#define __GPS_EXTI_H__

#include "stm32f10x.h"                  // Device header

extern uint8_t ifEXTI;
extern uint8_t ifEXTI_index;
void GPS_EXTI_Init(void);

#endif
