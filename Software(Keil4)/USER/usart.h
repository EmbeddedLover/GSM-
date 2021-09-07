#ifndef __USART_H__
#define __USART_H__

#include "stm32f10x.h"
#include "stm32f10x_usart.h"

#define uchar unsigned char
#define uint unsigned int

extern void GPIO_Configuration_USART(void);
extern void NVIC_Configuration(void);
extern void Send_Command(uchar Value[],uchar Len);
extern void GSM_Init(void);	
extern void Send_Message1(void);
extern void Send_Message2(void);
extern void Send_Message3(void);
extern void Send_Message4(void);
extern void display_Usart1(void);
extern void Read_Message(void);
#endif

