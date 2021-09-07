#include "iwdg.h"
//https://blog.csdn.net/weibo1230123/article/details/80705866

void IWDG_Configuration(void)
{							
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);   //�ʿ��Ź�д��
	IWDG_SetPrescaler(IWDG_Prescaler_256);          //��Ƶ��40kHz����ʱ���ź� 40K/256=156HZ(6.4ms)
	IWDG_SetReload(1562);                          	//ι��ʱ�� 10s/6.4MS=1562
	IWDG_ReloadCounter();                           //ι��
	IWDG_Enable();                                  //ʹ�ܿ��Ź�
}

void IWDG_Feed(void)                                // ι�� ��������ʱ��ι��һ��
{
    // ����װ�ؼĴ�����ֵ�ŵ��������У�ι������ֹIWDG��λ
    // ����������ֵ����0��ʱ������ϵͳ��λ
    IWDG_ReloadCounter();
}


