#include "ADC.h"

int Smoke;									//����Ũ��
int Set_smoke;								//��������Ũ��

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
	
	ADC_DeInit(ADC1);													//ADCʱ�Ӹ�λ
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);									//����ADC��Ƶ���ӣ����ܳ���14MHz����ǰ72/6=12MHz
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 ,ENABLE);               	//RCC�д�
	
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;					//ADC����ģʽ������ģʽ
	ADC_InitStructure.ADC_ScanConvMode=DISABLE;							//AD��ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;					//AD����ת��ģ��
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;				//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel=1;								//˳����й���ת����ADCͨ������Ŀ1
	ADC_Init(ADC1,&ADC_InitStructure);                  //ADC��ʼ��

	ADC_Cmd(ADC1,ENABLE);												//ʹ��ADC1
	ADC_ResetCalibration(ADC1);                 //��λѡ�е�ADCУ׼�Ĵ���
	while(ADC_GetResetCalibrationStatus(ADC1));//��ȡѡ����ADC��λУ׼�Ĵ���״̬
	ADC_StartCalibration(ADC1);                 //����ѡ����ADCУ׼����
	while(ADC_GetCalibrationStatus(ADC1));      //��ȡѡ����ADCУ׼״̬
	
}
 




