//头文件
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

uchar table1[]="NOW:    %     C ";				//液晶屏显示内容
uchar table2[]="SET:    %     C ";				//液晶屏显示内容

#define BEEP BIT_ADDR(GPIOA_ODR_Addr,11)                    //蜂鸣器接口
#define STEAL  GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10)    //防盗模块接口

char Alarm_flag;										//温度发送短信标志位，=1表示已经发送，=0表示没有发送
char Smoke_Alarm;										//烟雾发送短信标志位，=1表示已经发送，=0表示没有发送
char Steal_Alarm;										//防盗发送短信标志位，=1表示已经发送，=0表示没有发送

void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;													//等同于int a;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE); 			//GPIOC APB2			 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//选中管脚11
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //
	GPIO_Init(GPIOA, &GPIO_InitStructure);//以上设结构体置应用到GPIOA				 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;		//选中管脚10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; 	//上拉输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);			//以上设结构体置应用到GPIOB
}
/************************************检测各部分是否异常***********************************************************/

void Check(void)
{
	if(temperature>Set_temp)		//如果实际温度值大于设置值
	{
		if((temperature>0)&(temperature<100)) //温度在正常范围内才报警
		{
			BEEP=1;						//启动蜂鸣器
			if(Alarm_flag==0)			//警报标志位为1时发送温度报警信息
			{
				Send_Message3();		//发送温度过高报警短信
			}
			Alarm_flag=1;				//标志位置一，表示短信已经发送，避免重复发送短信
		}
	}
	if(temperature<=Set_temp)		//正常情况
	{
		Alarm_flag=0;				//清除标志位，下次温度超过继续发送短信一次
	}

	if(Steal_flag==1)
	{
		if(STEAL==0)				    //检测到人体
		{
			BEEP=1;						//启动蜂鸣器
			if(Steal_Alarm==0)			//警报标志位为1时发送防盗报警信息
			{
				Send_Message2();
			}
			Steal_Alarm=1;				//标志位置一，表示短信已经发送，避免重复发送短信
		}
	}
	if(STEAL==1)					//正常情况
	{
		Steal_Alarm=0;				 //清除标志位，下次继续发送短信一次
	}

	if(Smoke>Set_smoke)				 //检测到浓度超标
	{
		BEEP=1;						//启动蜂鸣器
		if(Smoke_Alarm==0)			//警报标志位为1时发送烟雾报警信息
		{
			Send_Message1();
		}
		Smoke_Alarm=1;				//标志位置一，表示短信已经发送，避免重复发送短信
	}
	if(Smoke<=Set_smoke)				//正常情况
	{
		Smoke_Alarm=0;				 //清除标志位，下次温度超过继续发送短信一次
	}

	if((Smoke<=Set_smoke)&&(temperature<=Set_temp)&&(STEAL==1))
	{
		BEEP=0;						 //关闭蜂鸣器
	}
}
//=============================================================================
//文件名称：Delay
//功能概要：延时
//参数说明：nCount：延时长短
//函数返回：无
//=============================================================================

void Delay(uint z)                 			//延时约3ms
{
  uint x,y;
	for(x=z;x>0;x--)
		for(y=5000;y>0;y--);
}


//=============================================================================
//文件名称：main
//功能概要：主函数
//参数说明：无
//函数返回：int
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
	temperature=DS18B20_Get_Temp();					//读取实时温度   第一次读取跳过85度
	LCD_Initinal();
	GPIO_Configuration_KEY();	
	Set_temp=I2C_EE_BufferRead(EEPROM_ADDRESS1);	//读取对应地址的值
	Set_smoke=I2C_EE_BufferRead(EEPROM_ADDRESS2);	//读取对应地址的值
	
	Delay(40000);Delay(40000);    		
	GSM_Init();                                     //GSM初始化
	LCD_Display_String(1,table1);					//显示内容
	LCD_Display_String(2,table2);					//显示内容	
	IWDG_Configuration();
	while (1)
	{   
		Read_Message();
		KEY();
		if(Adjust==0)
		{
			if(TIM_GetITStatus(TIM2, TIM_IT_Update)==SET)	  	//每0.5s读取一次温度
			{			
				temperature=DS18B20_Get_Temp();					//读取实时温度
				TIM_ClearITPendingBit(TIM2, TIM_IT_Update);		//清除定时器中断				
				
				ADC_RegularChannelConfig(ADC1,ADC_Channel_9,1,ADC_SampleTime_7Cycles5); //选取通道和采样周期
				ADC_SoftwareStartConvCmd(ADC1,ENABLE);			//使能指定的ADC的软件转换启动功能
				Smoke=0.000806*ADC_GetConversionValue(ADC1)/(10/14.7)*20-10;       //3.3V分4096份 ，每份约0.000806V， 硬件已被分压，除以10k/（10+4.7k）得出真实电压值（0-5V），
				                                                //再根据y=20x（0代表0%，5V代表100%）,10是校准值，是没有检测到烟雾时显示的值，确保在洁净空气中浓度值尽可能低
				if(Smoke<0)                                     //数值低于0做0处理
				Smoke=0;
				Write_Value(6,Smoke);               			//显示浓度值
                
				if(Switch_flag==1)							   //防盗功能开启状态
				{
					Count++;
					if(Count>=20)                              //启动定时器10s后开启功能
					{
						Count=0;
						Steal_flag=1;
					}									   
				}
			} 
			Write_Value(12,temperature);	//显示温度到LCD1602
			Write_Value(0x40+12,Set_temp);  //显示设置温度
			
			Write_Value(0x40+6,Set_smoke);  //显示设置温度
		}
				
        Check();
		IWDG_Feed();                        //10秒喂狗一次
	}
}



