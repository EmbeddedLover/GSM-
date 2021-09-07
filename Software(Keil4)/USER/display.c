#include "display.h"
#include "GPIOLIKE51.h"

#define rs BIT_ADDR(GPIOC_ODR_Addr,13) 
#define rw BIT_ADDR(GPIOC_ODR_Addr,14)
#define e  BIT_ADDR(GPIOC_ODR_Addr,15)

uchar table3[]="                ";			 	//��������
uchar table4[]="GSM Module Alarm";				//��ʼ����ʾ����
uchar table5[]="SIM Initialize!!";				//��ʼ����ʾ����
//=============================================================================
//�ļ����ƣ�GPIO_Configuration
//���ܸ�Ҫ��LCD GPIO��ʼ��
//����˵������
//�������أ���
//=============================================================================
void GPIO_Configuration_LCD(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;												

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE); 			//ʹ��GPIOB APB2����ʱ��			 
		 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	//ѡ�йܽ�0 1 10����GPIOB0
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;					//������50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 					//�������
	GPIO_Init(GPIOC, &GPIO_InitStructure);								//������ṹ����Ӧ�õ�GPIOB

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE); 			//ʹ��GPIOA APB2����ʱ��			 
		 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;//ѡ�йܽ�
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;					//������50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	 				//�������
	GPIO_Init(GPIOA, &GPIO_InitStructure);								//������ṹ����Ӧ�õ�GPIOA

}

void delay_LCD(uint z)			  				//��ʱ����
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=500;y>0;y--);
}

void write_com(uchar com)			 			//LCD1602дָ��������������ֲ��ʱ��
{
	rw=0;
	delay_LCD(5);
	rs=0;
	delay_LCD(5);
	e=1;
	delay_LCD(5);
//	GPIO_Write(GPIOA,com);						//��ָ���GPIO��
	GPIOA->ODR=GPIOA->ODR&0xff00|com;            //�Ͱ�λ
	delay_LCD(5);
	e=0;
	delay_LCD(5);	
}

void write_date(uchar date)						//LCD1602д���ݺ��������������ֲ��ʱ��
{									
	rw=0;
	delay_LCD(5);
	rs=1;
	delay_LCD(5);
	e=1;
	delay_LCD(5);
//	GPIO_Write(GPIOA,date);									//�����ݸ���P0��
    GPIOA->ODR=GPIOA->ODR&0xff00|date;                      //�Ͱ�λ
	delay_LCD(5);
	e=0;
	delay_LCD(5);	
}

void LCD_Initinal(void)						    //LCD1602�Ͷ�ʱ����ʼ������
{	
	uchar num;
	e=0;						    //ʱ���e��ʼΪ0
	write_com(0x38);		        //����Ϊ16*2��ʾ��5*7����8λ���ݽӿ�
	write_com(0x0c);				//���ù��
	write_com(0x06);				//����Զ���1,������뷽ʽ
	write_com(0x01);				//����
	write_com(0x80);				//���ó�ʼ��ʾλ��
	for(num=0;num<16;num++)			//��ʾ��һ������
	{
		write_date(table4[num]);
		delay_LCD(5);
	}
	write_com(0x80+0x40);
	for(num=0;num<16;num++)			//��ʾ�ڶ�������
	{
		write_date(table5[num]);
		delay_LCD(5);
	}		
} 

void Write_Value(uchar add,uchar date)	 	//�������һ���������ĵ�ַ���ڶ���������������
{
	uchar shi,ge;
	shi=date/10;
	ge=date%10;
	write_com(0x80+add);
	write_date(0x30+shi);				    
	write_date(0x30+ge);				    
}
void Write_4Value(uchar add,uint date)	 	//�������һ���������ĵ�ַ���ڶ���������������
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
{ 										//Һ������ʾ���ݣ���Ҫ��ʾ������д����Ӧ��λ��
	unsigned char i;
	unsigned char address=0;
	if(line==1)
	{
		address=0x80;	 				//0X80�ǵ�1�еĵ�1��λ��  ,0x81��2λ
	}
	else if(line==2)
	{
		address=0x80+0x40;  			//0X80+0x40�ǵ�2�еĵ�1��λ��  ,0X80+0x40+1�ǵ�2�е�2λ
	}
	
	for(i=0;i<16;i++)
	{
		write_com(address);
		write_date(string[i]);
		address++;
	} 
}
void LCD_Display_PhoneNumber(unsigned char line,unsigned char *string)
{ 										//Һ������ʾ���ݣ���Ҫ��ʾ������д����Ӧ��λ��
	unsigned char i;
	unsigned char address=0;
	if(line==1)
	{
		address=0x80+2;	 				//0X80�ǵ�1�еĵ�1��λ��  ,0x81��2λ
	}
	else if(line==2)
	{
		address=0x80+0x40+2;  			//0X80+0x40�ǵ�2�еĵ�1��λ��  ,0X80+0x40+1�ǵ�2�е�2λ
	}
	
	for(i=0;i<11;i++)
	{
		write_com(address);
		write_date(string[i]);
		address++;
	} 
}

