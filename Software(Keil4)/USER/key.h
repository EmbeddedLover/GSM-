#ifndef __KEY_H__
#define __KEY_H__

#include "stm32f10x.h"
#include <ds18b20.h>
#define uchar unsigned char
#define uint unsigned int

extern void GPIO_Configuration_KEY(void);
extern void KEY(void);
extern uchar Adjust;
extern uchar Steal_flag;
extern uchar Switch_flag;
extern uchar Count;

#endif

