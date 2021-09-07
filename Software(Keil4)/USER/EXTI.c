#include "EXTI.h"
#include "GPIOLIKE51.h"
#include "display.h"
//https://blog.csdn.net/u010834669/article/details/79253850


void GPIO_Configuration_EXTI(void)               //B8
{   
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);//打开GPIO AFIO的时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;                                //选择GPIO_Pin_8
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;                            //选择上拉输入模式
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
		
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource8);	//PB  为GPIOC的PIN8 选择EXTI信号源
	EXTI_InitStructure.EXTI_Line= EXTI_Line8; 					//PB8，中断线选择
	EXTI_InitStructure.EXTI_Mode= EXTI_Mode_Interrupt;          //EXTI为中断模式
	EXTI_InitStructure.EXTI_Trigger= EXTI_Trigger_Falling;   	//中断方式为下降沿触发
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;                     //使能中断
	EXTI_Init(&EXTI_InitStructure);
		
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);   			//配置NVIC优先级分组
	NVIC_InitStructure.NVIC_IRQChannel =  EXTI9_5_IRQn;         //EXTI_Line8位于中断源：[9:5]
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 1;    //抢占优先级：1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority= 1;           //子优先级：1
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;               //使能中断通道
	NVIC_Init(&NVIC_InitStructure);	
	
}

void EXTI9_5_IRQHandler(void)							//这里为：EXTI9_5 (外部中断号的5~9都在这里实现）
{		
	uchar a;
	if(EXTI_GetITStatus(EXTI_Line8)!= RESET)            //判断是否进入中断
	{  
		EXTI_ClearITPendingBit(EXTI_Line8);             //清楚中断源
		a++;
		if(a>99)
		a=0;
		Write_Value(0x40+5,a);
	}
}

