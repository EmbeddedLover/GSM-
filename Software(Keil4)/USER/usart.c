#include "usart.h"
#include <ds18b20.h>
#include "ADC.h"
#include "I2C.h"
#include "display.h"
#include "GPIOLIKE51.h"

#define JDQ BIT_ADDR(GPIOA_ODR_Addr,12)             //�̵����ӿ�

char NEW_SMS;										//����Ϣ��ʾ��־λ
char READ_SMS;										//��ȡ����Ϣ��־λ
char ATH;											//�һ���־λ
uchar UART0_Data[128];								//��Ž��յ�������
uchar UART0_Len;									//���峤��


uchar Command_AT[]="AT\r\n";						//����AT����������
uchar Command_AT_CMGF[]="AT+CMGF=1\r\n";			//�Ѹ�ʽ����Ϊtext��ʽ
uchar Command_AT_CMGS[]="AT+CMGS=";					//���͵�ָ������ָ��
uchar Command_AT_YIN[]={0x22};						//����ָ������˫����
uchar Command_AT_ENTER[]="\r\n";					//����ָ������˫����
uchar Command_AT_NUM[]="           ";				//���͵�ָ������
uchar Command_AT_IPR[]="AT+IPR=9600\r\n";			//���ò�����
uchar Command_AT_CMGR[]="AT+CMGR=  \r\n";	  		//���ڼ�������
uchar Command_AT_CMGD[]="AT+CMGD=  \r\n";	  		//ɾ���ڼ�������
uchar Command_AT_CMGD1[]="AT+CMGD=01\r\n";	  		//ɾ����1������
uchar Command_AT_CNMI[]="AT+CNMI=1,1,2\r\n";		//���ö���Ϣ��ʾ
uchar Command_AT_CLIP[]="AT+CLIP=1\r\n";			//������ʾ
uchar Command_AT_CRC[]="AT+CRC=0\r\n";				//�������翪ͷΪRING���е绰�����յ�RING����
uchar Command_ATH[]="ATH\r\n";						//�ҵ绰
uchar Command_ATE[]="ATE1\r\n";						//���û���
uchar Command_AT_End[]={0x1A};						//���Ͷ������ݽ�����
uchar Command_NUM[]="0123456789+-";					//�������¶�ֵ��ȡ������
uchar Command_AT_Tem[44]="   C, Temperature exceed limit, please note!";	  	//�¶ȳ������ޣ���ע�⣡
uchar Command_Someone[34]="Someone in the house, please note!";		  					//�����������ע�⣡
uchar Command_Smoke[56]="  %, Combustible gas or smoke exceed limit, please note!";   //������ȼ�����峬�����ޣ���ע�⣡
uchar Command_Smoke1[6]=" &   %";
uchar Command_Opened[13]="It is opened!";			//��
uchar Command_Closed[13]="It is closed!";			//�ر�
uchar OPEN[]="#OPEN#";	  					 		//����
uchar CLOSE[]="#CLOSE#";	  					 	//�ر�

void delay2(uint z)			  					 //
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=300;y>0;y--);
}

void GPIO_Configuration_USART(void)
{   
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);               //RCC�д���Ӧ����
	                                                                    //RCC��STM32��ʱ�ӿ��������ɿ�����رո����ߵ�ʱ�ӣ���ʹ�ø����蹦�ܱ����ȿ������Ӧ��ʱ�ӣ�û�����ʱ���ڲ��ĸ������Ͳ������С�
	USART_InitStructure.USART_BaudRate = 9600; 							//������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 		//8λ����
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 				//1λֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No ; 				//��żʧ�� �޼���λ
	USART_InitStructure.USART_HardwareFlowControl = 
	USART_HardwareFlowControl_None; 									//��Ӳ������
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; 	//ʹ�ܷ��� ����  �շ�ģʽ
	USART_Init(USART1, &USART_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                      //ʹ�ܽ����ж�
	USART_Cmd(USART1, ENABLE);                                          //ʹ��USART1
	
    
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;					//TX
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;           //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//������50MHz
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;                //RX
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;     //���ÿ�©����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//������50MHz
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); 						 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void Send_Command(uchar Value[],uchar Len)				//����ָ�Value[]��ʾ�����ĸ����飬Len��ʾ���͸�������ļ�λ
{
	uchar i;
	for(i=0;i<Len;i++)
	{
		USART_SendData(USART1, Value[i]);  				//����1bit����
		delay2(500);
	}
}

void GSM_Init(void)					  	  //GSMģ���ʼ��
{
	Send_Command(Command_ATE,5);		  //���û���
	delay2(10000);
	Send_Command(Command_AT,3);			  //��������
	delay2(100);
	Send_Command(Command_AT_CMGF,10);	  //����Ϊtext��ʽ
	delay2(20000);
	Send_Command(Command_AT_CNMI,14);	  //����Ϊ����Ϊ�жϷ�ʽ���ն��ţ���GSM���ն��ź�����λ�����ڷ��� +CMTI: "SM",1 ��
	delay2(30000);
	Send_Command(Command_AT_CLIP,10);	  //����������ʾ  RING   +CLIP:
	delay2(10000);
	Send_Command(Command_AT_CRC,9);	  	  //����������ʾ  RING   +CLIP:
	delay2(10000);
	Send_Command(Command_AT_IPR,12);	  //���ò�����=9600
	delay2(10000);
	Send_Command(Command_AT_CMGD1,11);	  		//ɾ�����ŵ�һ������Ϣ,�ڳ��ռ���ն���
	delay2(10000);
}
void Send_Message(void)					  	  //���͵�ָ������
{
	Command_AT_NUM[0]=I2C_EE_BufferRead(EEPROM_Phone1);				//��ȡ��Ӧ��ַ��ֵ
	Command_AT_NUM[1]=I2C_EE_BufferRead(EEPROM_Phone2);				//��ȡ��Ӧ��ַ��ֵ
	Command_AT_NUM[2]=I2C_EE_BufferRead(EEPROM_Phone3);				//��ȡ��Ӧ��ַ��ֵ
	Command_AT_NUM[3]=I2C_EE_BufferRead(EEPROM_Phone4);				//��ȡ��Ӧ��ַ��ֵ
	Command_AT_NUM[4]=I2C_EE_BufferRead(EEPROM_Phone5);				//��ȡ��Ӧ��ַ��ֵ
	Command_AT_NUM[5]=I2C_EE_BufferRead(EEPROM_Phone6);				//��ȡ��Ӧ��ַ��ֵ
	Command_AT_NUM[6]=I2C_EE_BufferRead(EEPROM_Phone7);				//��ȡ��Ӧ��ַ��ֵ
	Command_AT_NUM[7]=I2C_EE_BufferRead(EEPROM_Phone8);				//��ȡ��Ӧ��ַ��ֵ
	Command_AT_NUM[8]=I2C_EE_BufferRead(EEPROM_Phone9);				//��ȡ��Ӧ��ַ��ֵ
	Command_AT_NUM[9]=I2C_EE_BufferRead(EEPROM_Phone10);			//��ȡ��Ӧ��ַ��ֵ
	Command_AT_NUM[10]=I2C_EE_BufferRead(EEPROM_Phone11);			//��ȡ��Ӧ��ַ��ֵ
	
	Send_Command(Command_AT_CMGS,8);		  //����ָ��
	delay2(5000);							  
	Send_Command(Command_AT_YIN,1);		      //��������
	delay2(1000);
	Send_Command(Command_AT_NUM,11);		  //���ͺ���
	delay2(1000);
	Send_Command(Command_AT_YIN,1);		      //��������
	delay2(1000);
	Send_Command(Command_AT_ENTER,1);		  //���ͻس�
	delay2(1000);
}	
void Send_Message1(void)					  //���;���Ũ��
{	
	Command_Smoke[0]=Command_NUM[Smoke/10];	  //��ʮλ�����ִ浽Command_Smoke[0]
	Command_Smoke[1]=Command_NUM[Smoke%10];	  //�Ѹ�λ�����ִ浽Command_Smoke[1]
	Send_Message();							  //���͵�ָ������
	delay2(10000);							  //�ȴ��ظ��������
	Send_Command(Command_Smoke,56);		      //������������Ϣ
	delay2(10000);
	Send_Command(Command_AT_End,1);			  //���Ͷ��Ž�����
	delay2(1000);
}

void Send_Message2(void)					  //�������ҵ�������Ϣ
{
	Send_Message();							  //���͵�ָ������
	delay2(10000);							  //�ȴ��ظ��������
	Send_Command(Command_Someone,34);		  //���;�����Ϣ
	delay2(10000);
	Send_Command(Command_AT_End,1);			  //���Ͷ��Ž�����
	delay2(1000);
}
void Send_Message3(void)					  //���;����¶�
{
	if((temperature>0)&(temperature<100)) 			//�¶���������Χ��ת��
	{	
		if(Fuhao==1)
		Command_AT_Tem[0]=Command_NUM[10];				//Command_AT_Tem[0]Ϊ+����ʾ�¶ȴ��ڻ����0
		if(Fuhao==0)
		Command_AT_Tem[0]=Command_NUM[11];				//Command_AT_Tem[0]Ϊ-����ʾ�¶�С��0
		
		Command_AT_Tem[1]=Command_NUM[temperature/10];	//��ʮλ�����ִ浽Command_AT_Tem[1]
		Command_AT_Tem[2]=Command_NUM[temperature%10];	//�Ѹ�λ�����ִ浽Command_AT_Tem[2]
	}
	Send_Message();							  //���͵�ָ������
	delay2(10000);							  //�ȴ��ظ��������
	Send_Command(Command_AT_Tem,44);		  //�����¶�
	delay2(10000);
	Send_Command(Command_AT_End,1);			  //���Ͷ��Ž�����
	delay2(1000);
}
void Send_Message4(void)					  //���͵�ǰ�¶�
{
	if((temperature>0)&(temperature<100)) 			//�¶���������Χ��ת��
	{	
		if(Fuhao==1)
		Command_AT_Tem[0]=Command_NUM[10];				//Command_AT_Tem[0]Ϊ+����ʾ�¶ȴ��ڻ����0
		if(Fuhao==0)
		Command_AT_Tem[0]=Command_NUM[11];				//Command_AT_Tem[0]Ϊ-����ʾ�¶�С��0
		
		Command_AT_Tem[1]=Command_NUM[temperature/10];	//��ʮλ�����ִ浽Command_AT_Tem[1]
		Command_AT_Tem[2]=Command_NUM[temperature%10];	//�Ѹ�λ�����ִ浽Command_AT_Tem[2]
	}
	
	Command_Smoke1[3]=Command_NUM[Smoke/10];	//��ʮλ�����ִ浽Command_Smoke1[1]
	Command_Smoke1[4]=Command_NUM[Smoke%10];	//�Ѹ�λ�����ִ浽Command_Smoke1[2]
	
	Send_Message();							  //���͵�ָ������
	delay2(50000);							  //�ȴ��ظ��������
	Send_Command(Command_AT_Tem,4);		      //�����¶�
	delay2(5000);
	Send_Command(Command_Smoke1,6);		      //����Ũ��
	delay2(5000);
	Send_Command(Command_AT_End,1);			  //���Ͷ��Ž�����
	delay2(1000);
}
void Read_Message(void)
{
	if(NEW_SMS==1)														//������Ϣ
	{
		if((UART0_Data[13]>=0x30)&&(UART0_Data[13]<=0x39))				//�������ڶ�λ�����ݣ���ʾ�ж�λ������λ��					
		{
			if((UART0_Data[14]>=0x30)&&(UART0_Data[14]<=0x39))			//��λ�� �ݲ�����
			{

			}
			if((UART0_Data[14]<0x30)||(UART0_Data[14]>0x39))														//��λ�� 
			{		
				Command_AT_CMGR[8]=UART0_Data[12];					//��һλΪ������ʮλ
				delay2(1000);
				Command_AT_CMGR[9]=UART0_Data[13];					//�ڶ�λΪ��������λ
				delay2(1000);
				Command_AT_CMGD[8]=UART0_Data[12];					//��һλΪ������ʮλ
				delay2(1000);
				Command_AT_CMGD[9]=UART0_Data[13];					//��һλΪ��������λ
				delay2(1000);
			}
		}
		if((UART0_Data[13]<0x30)||(UART0_Data[13]>0x39))				//�������ڶ�λ�����ݣ���ʾֻ��һλ��
		{
			Command_AT_CMGR[8]=0x30;									//��һλΪ0
			delay2(1000);
			Command_AT_CMGR[9]=UART0_Data[12];						//�ڶ�λΪ������
			delay2(1000);
			Command_AT_CMGD[8]=0x30;									//��һλΪ0
			delay2(1000);
			Command_AT_CMGD[9]=UART0_Data[12];						//�ڶ�λΪ������
			delay2(1000);
		}		  
		Send_Command(Command_AT_CMGR,11);					//����"AT+CMGR=?",��ȡ�ڼ�������
		delay2(50000);delay2(50000);delay2(50000);			   							//��ʱ
		
		
		if(((UART0_Data[6]==OPEN[0])&&(UART0_Data[7]==OPEN[1])&&(UART0_Data[8]==OPEN[2])&&(UART0_Data[9]==OPEN[3])&&(UART0_Data[10]==OPEN[4])&&(UART0_Data[11]==OPEN[5]))//�ƶ�
		  ||(UART0_Data[3]==OPEN[0])&&(UART0_Data[4]==OPEN[1])&&(UART0_Data[5]==OPEN[2])&&(UART0_Data[6]==OPEN[3])&&(UART0_Data[7]==OPEN[4])&&(UART0_Data[8]==OPEN[5]))//��ͨ
		{	
			JDQ=1;								  	  //�����̵���
			Send_Command(Command_AT_CMGD,11);	  	  //ɾ������
			delay2(10000);
			Send_Message();							  //���͵�ָ������
			delay2(10000);							  //�ȴ��ظ��������							  
			Send_Command(Command_Opened,13);		  //������Ϣ
			delay2(10000);
			Send_Command(Command_AT_End,1);			  //���Ͷ��Ž�����
			delay2(20000);
		}
		if(((UART0_Data[6]==CLOSE[0])&&(UART0_Data[7]==CLOSE[1])&&(UART0_Data[8]==CLOSE[2])&&(UART0_Data[9]==CLOSE[3])&&(UART0_Data[10]==CLOSE[4])&&(UART0_Data[11]==CLOSE[5])&&(UART0_Data[12]==CLOSE[6]))//�ƶ�
		  ||(UART0_Data[3]==CLOSE[0])&&(UART0_Data[4]==CLOSE[1])&&(UART0_Data[5]==CLOSE[2])&&(UART0_Data[6]==CLOSE[3])&&(UART0_Data[7]==CLOSE[4])&&(UART0_Data[8]==CLOSE[5])&&(UART0_Data[9]==CLOSE[6]))//��ͨ
		{
			JDQ=0;								  	  //�رռ̵���
			Send_Command(Command_AT_CMGD,11);	  	  //ɾ������
			delay2(10000);
			Send_Message();							  //���͵�ָ������
			delay2(10000);							  //�ȴ��ظ��������
			Send_Command(Command_Closed,13);		  //������Ϣ
			delay2(10000);
			Send_Command(Command_AT_End,1);			  //���Ͷ��Ž�����
			delay2(20000);
		}
		UART0_Data[13]=0x40;UART0_Data[14]=0x40;
		UART0_Len=0;
		NEW_SMS=0;											//�������Ϣ��ʾ��־

	}														   

	if(ATH==1)												//�绰����ֱ�ӹһ�
	{
		Send_Command(Command_ATH,4);						//���͹һ�����
		delay2(50000);
		Send_Message4();								    //���͵�ǰ�¶�
		ATH=0;
		UART0_Len=0;										//��־
	}
}


void NVIC_Configuration(void)//�����ж����ȼ�  
{  
  NVIC_InitTypeDef NVIC_InitStructure;  
    
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);  
    
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
  NVIC_Init(&NVIC_InitStructure);  
}

void USART1_IRQHandler(void)
{  	  	
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
	{
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		UART0_Data[UART0_Len]=USART_ReceiveData(USART1);
		
		if(UART0_Data[UART0_Len]==0x2B)                      	//+��
		UART0_Len=0;			

		if((UART0_Data[1]==0x43)&&(UART0_Data[2]==0x4C)&&(UART0_Data[3]==0x49)&&(UART0_Data[4]==0x50)&&(UART0_Data[5]==0x3A))
		ATH=1;			//�һ���־λ��һ						//���յ��������ǵ绰��ʾUART0_Data����="+CLIP:"���Ƚ�ǰ6λ
		
		if((UART0_Data[1]==0x43)&&(UART0_Data[2]==0x4D)&&(UART0_Data[3]==0x54)&&(UART0_Data[4]==0x49))
		NEW_SMS=1;		//���ű�־λ��һ						//���յ��������Ƕ�����ʾUART0_Data����="+CMTI",�Ƚ�ǰ��λ		
			
		UART0_Len++;
	}
}
void display_Usart1(void)
{
	write_com(0x80+0);
	write_date(UART0_Data[0]);
	write_date(UART0_Data[1]);
	write_date(UART0_Data[2]);
	write_date(UART0_Data[3]);
	write_date(UART0_Data[4]);
	write_date(UART0_Data[5]);
	write_date(UART0_Data[6]);
	write_date(UART0_Data[7]);
	write_date(UART0_Data[8]);
	write_date(UART0_Data[9]);
	write_date(UART0_Data[10]);
	write_date(UART0_Data[11]);
	write_date(UART0_Data[12]);
	write_date(UART0_Data[13]);
	write_date(UART0_Data[14]);
	write_date(UART0_Data[15]);
	write_com(0x80+0x40+0);
	write_date(UART0_Data[16]);
	write_date(UART0_Data[17]);
	write_date(UART0_Data[18]);
	write_date(UART0_Data[19]);
	write_date(UART0_Data[20]);
	write_date(UART0_Data[21]);
	write_date(UART0_Data[22]);
	write_date(UART0_Data[23]);
	write_date(UART0_Data[24]);
	write_date(UART0_Data[25]);
	write_date(UART0_Data[26]);
	write_date(UART0_Data[27]);
	write_date(UART0_Data[28]);
	write_date(UART0_Data[29]);
	write_date(UART0_Data[30]);
	write_date(UART0_Data[31]);
}

	

