#ifndef __DS18B20_H
#define __DS18B20_H 
#include "stm32f10x.h"   
//IO????
#define DS18B20_IO_IN()  {GPIOB->CRL&=0XFFFFFFF0;GPIOB->CRL|=8<<0;}      //����Ϊ����
#define DS18B20_IO_OUT() {GPIOB->CRL&=0XFFFFFFF0;GPIOB->CRL|=3<<0;}      //����Ϊ���
////IO????                                             
#define DS18B20_DQ_OUT PBout(0) //PB0 ���
#define DS18B20_DQ_IN  PBin(0)  //PB0 ����

extern int temperature;									//ȫ�ֱ��� �¶�
extern int Set_temp;									//�����¶�
extern char Fuhao;

extern void delay_us(u32 i);
u8 DS18B20_Init(void);          //???DS18B20
int DS18B20_Get_Temp(void);   //????
void DS18B20_Start(void);       //??????
void DS18B20_Write_Byte(u8 dat);//??????
u8 DS18B20_Read_Byte(void);     //??????
u8 DS18B20_Read_Bit(void);      //?????
u8 DS18B20_Check(void);         //??????DS18B20
void DS18B20_Rst(void);         //??DS18B20    
#endif




