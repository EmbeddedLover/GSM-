#include "timer.h"

void TIMER_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);				//使能定时器  2.3.4.5
	TIM_TimeBaseStructure.TIM_Period =5000;                  		//自动重装的计数值
	TIM_TimeBaseStructure.TIM_Prescaler = 7200;                  	//这个就是预分频系数72MHz/7200=10k   即每次计数时间为1s/10k 再乘以以上计数，得到定时时间
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;              		//数字滤波器，定时的时候不涉及此功能，为零即
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 	//计数模式选择，此处设置为向上模式
	TIM_TimeBaseInit(TIM2, & TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);						//开定时器X溢出中断
	TIM_Cmd(TIM2,ENABLE);											//计数器使能，开始工作
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

