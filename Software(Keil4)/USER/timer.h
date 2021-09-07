#ifndef __TIMER_H__
#define __TIMER_H__

#include "stm32f10x.h"
#include "stm32f10x_tim.h"

#define uchar unsigned char
#define uint unsigned int

extern void TIMER_Init(void);
extern void TIMER_PWM_Init(u16 arr,u16 psc,u16 width);
extern void NVIC_Configuration(void);
#endif

