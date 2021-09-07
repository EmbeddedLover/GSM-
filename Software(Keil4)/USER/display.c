#include "display.h"
#include "GPIOLIKE51.h"

#define rs BIT_ADDR(GPIOC_ODR_Addr,13) 
#define rw BIT_ADDR(GPIOC_ODR_Addr,14)
#define e  BIT_ADDR(GPIOC_ODR_Addr,15)

uchar table3[]="                ";			 	//清屏内容
uchar table4[]="GSM Module Alarm";				//初始化显示内容
uchar table5[]="SIM Initialize!!";				//初始化显示内容
//=============================================================================
//文件名称：GPIO_Configuration
//功能概要：LCD GPIO初始化
//参数说明：无
//函数返回：无
//=============================================================================
void GPIO_Configuration_LCD(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;												

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE); 			//使能GPIOB APB2外设时钟			 
		 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	//选中管脚0 1 10，即GPIOB0
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;					//最高输出50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 					//推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure);								//以上设结构体置应用到GPIOB

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE); 			//使能GPIOA APB2外设时钟			 
		 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;//选中管脚
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;					//最高输出50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	 				//推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);								//以上设结构体置应用到GPIOA

}

void delay_LCD(uint z)			  				//延时函数
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=500;y>0;y--);
}

void write_com(uchar com)			 			//LCD1602写指令函数，根据数据手册的时序
{
	rw=0;
	delay_LCD(5);
	rs=0;
	delay_LCD(5);
	e=1;
	delay_LCD(5);
//	GPIO_Write(GPIOA,com);						//把指令赋给GPIO口
	GPIOA->ODR=GPIOA->ODR&0xff00|com;            //低八位
	delay_LCD(5);
	e=0;
	delay_LCD(5);	
}

void write_date(uchar date)						//LCD1602写数据函数，根据数据手册的时序
{									
	rw=0;
	delay_LCD(5);
	rs=1;
	delay_LCD(5);
	e=1;
	delay_LCD(5);
//	GPIO_Write(GPIOA,date);									//把数据赋给P0口
    GPIOA->ODR=GPIOA->ODR&0xff00|date;                      //低八位
	delay_LCD(5);
	e=0;
	delay_LCD(5);	
}

void LCD_Initinal(void)						    //LCD1602和定时器初始化函数
{	
	uchar num;
	e=0;						    //时序表e初始为0
	write_com(0x38);		        //设置为16*2显示，5*7点阵，8位数据接口
	write_com(0x0c);				//设置光标
	write_com(0x06);				//光标自动加1,光标输入方式
	write_com(0x01);				//清屏
	write_com(0x80);				//设置初始显示位置
	for(num=0;num<16;num++)			//显示第一行内容
	{
		write_date(table4[num]);
		delay_LCD(5);
	}
	write_com(0x80+0x40);
	for(num=0;num<16;num++)			//显示第二行内容
	{
		write_date(table5[num]);
		delay_LCD(5);
	}		
} 

void Write_Value(uchar add,uchar date)	 	//括号里：第一个：参数的地址；第二个：参数的内容
{
	uchar shi,ge;
	shi=date/10;
	ge=date%10;
	write_com(0x80+add);
	write_date(0x30+shi);				    
	write_date(0x30+ge);				    
}
void Write_4Value(uchar add,uint date)	 	//括号里：第一个：参数的地址；第二个：参数的内容
{
	uchar qian,bai,shi,ge;
	qian=date/1000;
	bai=date%1000/100;
	shi=date%100/10;
	ge=date%10;
	write_com(0x80+add);
	write_date(0x30+qian);
	write_date(0x30+bai);
	write_date(0x30+shi);				    
	write_date(0x30+ge);				    
}
			 
void LCD_Display_String(unsigned char line,unsigned char *string)
{ 										//液晶屏显示内容，把要显示的内容写到对应的位置
	unsigned char i;
	unsigned char address=0;
	if(line==1)
	{
		address=0x80;	 				//0X80是第1行的第1个位置  ,0x81第2位
	}
	else if(line==2)
	{
		address=0x80+0x40;  			//0X80+0x40是第2行的第1个位置  ,0X80+0x40+1是第2行第2位
	}
	
	for(i=0;i<16;i++)
	{
		write_com(address);
		write_date(string[i]);
		address++;
	} 
}
void LCD_Display_PhoneNumber(unsigned char line,unsigned char *string)
{ 										//液晶屏显示内容，把要显示的内容写到对应的位置
	unsigned char i;
	unsigned char address=0;
	if(line==1)
	{
		address=0x80+2;	 				//0X80是第1行的第1个位置  ,0x81第2位
	}
	else if(line==2)
	{
		address=0x80+0x40+2;  			//0X80+0x40是第2行的第1个位置  ,0X80+0x40+1是第2行第2位
	}
	
	for(i=0;i<11;i++)
	{
		write_com(address);
		write_date(string[i]);
		address++;
	} 
}

