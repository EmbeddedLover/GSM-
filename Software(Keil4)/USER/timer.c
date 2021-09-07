#include "timer.h"

void TIMER_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);				//ʹ�ܶ�ʱ��  2.3.4.5
	TIM_TimeBaseStructure.TIM_Period =5000;                  		//�Զ���װ�ļ���ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = 7200;                  	//�������Ԥ��Ƶϵ��72MHz/7200=10k   ��ÿ�μ���ʱ��Ϊ1s/10k �ٳ������ϼ������õ���ʱʱ��
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;              		//�����˲�������ʱ��ʱ���漰�˹��ܣ�Ϊ�㼴
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 	//����ģʽѡ�񣬴˴�����Ϊ����ģʽ
	TIM_TimeBaseInit(TIM2, & TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);						//����ʱ��X����ж�
	TIM_Cmd(TIM2,ENABLE);											//������ʹ�ܣ���ʼ����
}

/*void NVIC_Configuration(void)                  
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x0000);
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}
*/

