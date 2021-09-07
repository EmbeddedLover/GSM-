#include "EXTI.h"
#include "GPIOLIKE51.h"
#include "display.h"
//https://blog.csdn.net/u010834669/article/details/79253850


void GPIO_Configuration_EXTI(void)               //B8
{   
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);//��GPIO AFIO��ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;                                //ѡ��GPIO_Pin_8
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;                            //ѡ����������ģʽ
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
		
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource8);	//PB  ΪGPIOC��PIN8 ѡ��EXTI�ź�Դ
	EXTI_InitStructure.EXTI_Line= EXTI_Line8; 					//PB8���ж���ѡ��
	EXTI_InitStructure.EXTI_Mode= EXTI_Mode_Interrupt;          //EXTIΪ�ж�ģʽ
	EXTI_InitStructure.EXTI_Trigger= EXTI_Trigger_Falling;   	//�жϷ�ʽΪ�½��ش���
	EXTI_InitStructure.EXTI_LineCmd=ENABLE;                     //ʹ���ж�
	EXTI_Init(&EXTI_InitStructure);
		
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);   			//����NVIC���ȼ�����
	NVIC_InitStructure.NVIC_IRQChannel =  EXTI9_5_IRQn;         //EXTI_Line8λ���ж�Դ��[9:5]
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 1;    //��ռ���ȼ���1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority= 1;           //�����ȼ���1
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;               //ʹ���ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);	
	
}

void EXTI9_5_IRQHandler(void)							//����Ϊ��EXTI9_5 (�ⲿ�жϺŵ�5~9��������ʵ�֣�
{		
	uchar a;
	if(EXTI_GetITStatus(EXTI_Line8)!= RESET)            //�ж��Ƿ�����ж�
	{  
		EXTI_ClearITPendingBit(EXTI_Line8);             //����ж�Դ
		a++;
		if(a>99)
		a=0;
		Write_Value(0x40+5,a);
	}
}

