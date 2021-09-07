#include "iwdg.h"
//https://blog.csdn.net/weibo1230123/article/details/80705866

void IWDG_Configuration(void)
{							
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);   //允看门狗写入
	IWDG_SetPrescaler(IWDG_Prescaler_256);          //分频由40kHz产生时钟信号 40K/256=156HZ(6.4ms)
	IWDG_SetReload(1562);                          	//喂狗时间 10s/6.4MS=1562
	IWDG_ReloadCounter();                           //喂狗
	IWDG_Enable();                                  //使能看门狗
}

void IWDG_Feed(void)                                // 喂狗 以上设置时间喂狗一次
{
    // 把重装载寄存器的值放到计数器中，喂狗，防止IWDG复位
    // 当计数器的值减到0的时候会产生系统复位
    IWDG_ReloadCounter();
}


