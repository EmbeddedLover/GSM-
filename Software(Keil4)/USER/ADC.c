#include "ADC.h"

int Smoke;									//烟雾浓度
int Set_smoke;								//设置烟雾浓度

void GPIO_Configuration_ADC(void)               //ADC12_IN9
{   
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);  	
}
void Set_ADC_Init(void)
{   
	ADC_InitTypeDef ADC_InitStructure;
	
	ADC_DeInit(ADC1);													//ADC时钟复位
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);									//设置ADC分频因子，不能超过14MHz，当前72/6=12MHz
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 ,ENABLE);               	//RCC中打开
	
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;					//ADC工作模式：独立模式
	ADC_InitStructure.ADC_ScanConvMode=DISABLE;							//AD单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;					//AD单次转换模块
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;				//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel=1;								//顺序进行规则转换的ADC通道的数目1
	ADC_Init(ADC1,&ADC_InitStructure);                  //ADC初始化

	ADC_Cmd(ADC1,ENABLE);												//使能ADC1
	ADC_ResetCalibration(ADC1);                 //复位选中的ADC校准寄存器
	while(ADC_GetResetCalibrationStatus(ADC1));//获取选定的ADC复位校准寄存器状态
	ADC_StartCalibration(ADC1);                 //启动选定的ADC校准过程
	while(ADC_GetCalibrationStatus(ADC1));      //获取选定的ADC校准状态
	
}
 




