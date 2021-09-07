#ifndef __DISPLY_H__
#define __DISPLA_H__

#include "stm32f10x.h"
#define uchar unsigned char
#define uint unsigned int

extern void GPIO_Configuration_LCD(void);
extern void delay(uint z);
extern void write_com(uchar com);
extern void write_date(uchar date);
extern void LCD_Initinal(void);
extern void Write_Value(uchar add,uchar date);
extern void Write_4Value(uchar add,uint date);
extern void LCD_Display_String(unsigned char line,unsigned char *string);
extern void LCD_Display_PhoneNumber(unsigned char line,unsigned char *string);
#endif

