//ͷ�ļ�
#include "stm32f10x.h"
#include "GPIOLIKE51.h"
#include "display.h"
#include "ds18b20.h"
#include "key.h"
#include "usart.h"
#include "timer.h"
#include "ADC.h"
#include "iwdg.h"
#include "I2C.h"
#include "EXTI.h"

uchar table1[]="NOW:    %     C ";				//Һ������ʾ����
uchar table2[]="SET:    %     C ";				//Һ������ʾ����

#define BEEP BIT_ADDR(GPIOA_ODR_Addr,11)                    //�������ӿ�
#define STEAL  GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10)    //����ģ��ӿ�

char Alarm_flag;										//�¶ȷ��Ͷ��ű�־λ��=1��ʾ�Ѿ����ͣ�=0��ʾû�з���
char Smoke_Alarm;										//�����Ͷ��ű�־λ��=1��ʾ�Ѿ����ͣ�=0��ʾû�з���
char Steal_Alarm;										//�������Ͷ��ű�־λ��=1��ʾ�Ѿ����ͣ�=0��ʾû�з���

void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;													//��ͬ��int a;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE); 			//GPIOC APB2			 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//ѡ�йܽ�11
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //
	GPIO_Init(GPIOA, &GPIO_InitStructure);//������ṹ����Ӧ�õ�GPIOA				 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;		//ѡ�йܽ�10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; 	//��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);			//������ṹ����Ӧ�õ�GPIOB
}
/************************************���������Ƿ��쳣***********************************************************/

void Check(void)
{
	if(temperature>Set_temp)		//���ʵ���¶�ֵ��������ֵ
	{
		if((temperature>0)&(temperature<100)) //�¶���������Χ�ڲű���
		{
			BEEP=1;						//����������
			if(Alarm_flag==0)			//������־λΪ1ʱ�����¶ȱ�����Ϣ
			{
				Send_Message3();		//�����¶ȹ��߱�������
			}
			Alarm_flag=1;				//��־λ��һ����ʾ�����Ѿ����ͣ������ظ����Ͷ���
		}
	}
	if(temperature<=Set_temp)		//�������
	{
		Alarm_flag=0;				//�����־λ���´��¶ȳ����������Ͷ���һ��
	}

	if(Steal_flag==1)
	{
		if(STEAL==0)				    //��⵽����
		{
			BEEP=1;						//����������
			if(Steal_Alarm==0)			//������־λΪ1ʱ���ͷ���������Ϣ
			{
				Send_Message2();
			}
			Steal_Alarm=1;				//��־λ��һ����ʾ�����Ѿ����ͣ������ظ����Ͷ���
		}
	}
	if(STEAL==1)					//�������
	{
		Steal_Alarm=0;				 //�����־λ���´μ������Ͷ���һ��
	}

	if(Smoke>Set_smoke)				 //��⵽Ũ�ȳ���
	{
		BEEP=1;						//����������
		if(Smoke_Alarm==0)			//������־λΪ1ʱ������������Ϣ
		{
			Send_Message1();
		}
		Smoke_Alarm=1;				//��־λ��һ����ʾ�����Ѿ����ͣ������ظ����Ͷ���
	}
	if(Smoke<=Set_smoke)				//�������
	{
		Smoke_Alarm=0;				 //�����־λ���´��¶ȳ����������Ͷ���һ��
	}

	if((Smoke<=Set_smoke)&&(temperature<=Set_temp)&&(STEAL==1))
	{
		BEEP=0;						 //�رշ�����
	}
}
//=============================================================================
//�ļ����ƣ�Delay
//���ܸ�Ҫ����ʱ
//����˵����nCount����ʱ����
//�������أ���
//=============================================================================

void Delay(uint z)                 			//��ʱԼ3ms
{
  uint x,y;
	for(x=z;x>0;x--)
		for(y=5000;y>0;y--);
}


//=============================================================================
//�ļ����ƣ�main
//���ܸ�Ҫ��������
//����˵������
//�������أ�int
//=============================================================================
int main(void)
{   		
	GPIO_Configuration();
	GPIO_Configuration_LCD();
	GPIO_Configuration_USART();
	NVIC_Configuration();
	Set_ADC_Init();
	I2C_Init_Set();	
	TIMER_Init();	
	DS18B20_Init(); 
	temperature=DS18B20_Get_Temp();					//��ȡʵʱ�¶�   ��һ�ζ�ȡ����85��
	LCD_Initinal();
	GPIO_Configuration_KEY();	
	Set_temp=I2C_EE_BufferRead(EEPROM_ADDRESS1);	//��ȡ��Ӧ��ַ��ֵ
	Set_smoke=I2C_EE_BufferRead(EEPROM_ADDRESS2);	//��ȡ��Ӧ��ַ��ֵ
	
	Delay(40000);Delay(40000);    		
	GSM_Init();                                     //GSM��ʼ��
	LCD_Display_String(1,table1);					//��ʾ����
	LCD_Display_String(2,table2);					//��ʾ����	
	IWDG_Configuration();
	while (1)
	{   
		Read_Message();
		KEY();
		if(Adjust==0)
		{
			if(TIM_GetITStatus(TIM2, TIM_IT_Update)==SET)	  	//ÿ0.5s��ȡһ���¶�
			{			
				temperature=DS18B20_Get_Temp();					//��ȡʵʱ�¶�
				TIM_ClearITPendingBit(TIM2, TIM_IT_Update);		//�����ʱ���ж�				
				
				ADC_RegularChannelConfig(ADC1,ADC_Channel_9,1,ADC_SampleTime_7Cycles5); //ѡȡͨ���Ͳ�������
				ADC_SoftwareStartConvCmd(ADC1,ENABLE);			//ʹ��ָ����ADC�����ת����������
				Smoke=0.000806*ADC_GetConversionValue(ADC1)/(10/14.7)*20-10;       //3.3V��4096�� ��ÿ��Լ0.000806V�� Ӳ���ѱ���ѹ������10k/��10+4.7k���ó���ʵ��ѹֵ��0-5V����
				                                                //�ٸ���y=20x��0����0%��5V����100%��,10��У׼ֵ����û�м�⵽����ʱ��ʾ��ֵ��ȷ���ڽྻ������Ũ��ֵ�����ܵ�
				if(Smoke<0)                                     //��ֵ����0��0����
				Smoke=0;
				Write_Value(6,Smoke);               			//��ʾŨ��ֵ
                
				if(Switch_flag==1)							   //�������ܿ���״̬
				{
					Count++;
					if(Count>=20)                              //������ʱ��10s��������
					{
						Count=0;
						Steal_flag=1;
					}									   
				}
			} 
			Write_Value(12,temperature);	//��ʾ�¶ȵ�LCD1602
			Write_Value(0x40+12,Set_temp);  //��ʾ�����¶�
			
			Write_Value(0x40+6,Set_smoke);  //��ʾ�����¶�
		}
				
        Check();
		IWDG_Feed();                        //10��ι��һ��
	}
}



