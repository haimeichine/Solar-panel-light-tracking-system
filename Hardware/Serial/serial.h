#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "stdio.h"
#include "GPS.h"
#include "stm32f10x.h"
/*
USART1  TXD  RXD  | USART2  TXD  RXD   RTS  CK   |  USART3  TXD  RXD  CK   CTS   RTS
        A9   A10  |         A2   A3    A1   A4   |          B10  B11  B12  B13   B14 
*/
extern char Serial_RxPacket[];

void Serial_UART_Init(USART_TypeDef* USARTx,uint32_t BaudRate);

void MuDc_W_reg(uint8_t regadd,uint8_t data); 



#endif
