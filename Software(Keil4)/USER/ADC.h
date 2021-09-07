#ifndef __ADC_H__
#define __ADC_H__

#include "stm32f10x.h"
#include "stm32f10x_adc.h"

#define uchar unsigned char
#define uint unsigned int

extern void GPIO_Configuration_ADC(void);
extern void Set_ADC_Init(void);

extern int Smoke;									//ÑÌÎíÅ¨¶È
extern int Set_smoke;								//ÉèÖÃÑÌÎíÅ¨¶È
#endif

