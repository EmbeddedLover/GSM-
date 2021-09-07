#include "key.h"
#include "GPIOLIKE51.h"
#include "I2C.h"
#include "display.h"
#include "ADC.h"

#define SELT    GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15)
#define ENTER   GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)
#define UP      GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13)
#define DOWN    GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)
#define SWITCH  GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8)
#define LED     BIT_ADDR(GPIOA_ODR_Addr,12)

uchar Command_AT_NUM1[]="00000000000";			//ָ������1
uchar table20[]=" 1.Phone Number ";			 	//���ñ�������
uchar table21[]=" 2.Set Temper   ";			 	//�����¶ȷ�ֵ
uchar table22[]=" 3.Set Smoke    ";			 	//��������Ũ����ֵ
uchar table23[]="                ";

uchar table30[]="Set Phone Number";			 	//���ú���
uchar table31[]="--           ---";			 	//--����--
uchar table32[]="Set Temperature ";			 	//�����¶�
uchar table33[]="              C ";			 	//
uchar table34[]="Set Smoke Concen";			 	//�����¶�
uchar table35[]="              % ";			 	//

uchar table11[]="NOW:    %     C ";				//Һ������ʾ����
uchar table12[]="SET:    %     C ";				//Һ������ʾ����

uchar Select_num;	  //ѡ�񰴼����´���
uchar Enter_num;	  //ȷ�ϰ������´���
uchar Adjust;
uchar Steal_flag;
uchar Switch_flag;
uchar Count;

void delay1(uint z)			  					 //��ʱԼ3ms����
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=5000;y>0;y--);
}

void GPIO_Configuration_KEY(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;													//����

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); 			//ʹ��GPIOB APB2����ʱ��			 
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;//ѡ�йܽ�0 1 10����GPIOB0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; 						//��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);								//������ṹ����Ӧ�õ�GPIOB	
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;							//ѡ�йܽ�0 1 10����GPIOB0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; 						//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);								//������ṹ����Ӧ�õ�GPIOA
	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	LED=1;
	
	Command_AT_NUM1[0]=I2C_EE_BufferRead(EEPROM_Phone1);				//��ȡ��Ӧ��ַ��ֵ
	Command_AT_NUM1[1]=I2C_EE_BufferRead(EEPROM_Phone2);				//��ȡ��Ӧ��ַ��ֵ
	Command_AT_NUM1[2]=I2C_EE_BufferRead(EEPROM_Phone3);				//��ȡ��Ӧ��ַ��ֵ
	Command_AT_NUM1[3]=I2C_EE_BufferRead(EEPROM_Phone4);				//��ȡ��Ӧ��ַ��ֵ
	Command_AT_NUM1[4]=I2C_EE_BufferRead(EEPROM_Phone5);				//��ȡ��Ӧ��ַ��ֵ
	Command_AT_NUM1[5]=I2C_EE_BufferRead(EEPROM_Phone6);				//��ȡ��Ӧ��ַ��ֵ
	Command_AT_NUM1[6]=I2C_EE_BufferRead(EEPROM_Phone7);				//��ȡ��Ӧ��ַ��ֵ
	Command_AT_NUM1[7]=I2C_EE_BufferRead(EEPROM_Phone8);				//��ȡ��Ӧ��ַ��ֵ
	Command_AT_NUM1[8]=I2C_EE_BufferRead(EEPROM_Phone9);				//��ȡ��Ӧ��ַ��ֵ
	Command_AT_NUM1[9]=I2C_EE_BufferRead(EEPROM_Phone10);				//��ȡ��Ӧ��ַ��ֵ
	Command_AT_NUM1[10]=I2C_EE_BufferRead(EEPROM_Phone11);				//��ȡ��Ӧ��ַ��ֵ
}

void KEY(void)
{
	if(SELT==0)
	{
		delay1(2);
		if(SELT==0)
		{	
			while(!SELT);
			Adjust=!Adjust;							//�������ģʽ
			if(Adjust==1)
			{	
				Select_num=1;						//�����ֻ�����
				LCD_Display_String(1,table20);
				LCD_Display_String(2,table21);
				write_com(0x80+0);					//д >
				write_date(0x3e);
				Enter_num=0;
			}
			else									//�ص���ʼ����
			{
				LCD_Display_String(1,table11);
				LCD_Display_String(2,table12);
				Select_num=0;
				Enter_num=0;					
				Adjust=0;
			}
			write_com(0x0c);						//��겻����˸
			Enter_num=0;			
		}
	}
	
	if(ENTER==0)
	{
		delay1(2);
		if(ENTER==0)
		{
			while(!ENTER);
			Enter_num++;
		
			if(Select_num==1)						//�����ֻ�����
			{
				if(Enter_num==1)
				{
					LCD_Display_String(1,table30);
					LCD_Display_String(2,table31);
					LCD_Display_PhoneNumber(2,Command_AT_NUM1);//��ʾ������
					write_com(0x80+0x40+2);				//�����˸					
					write_com(0x0f);
				}
				if(Enter_num==2)
				{
					I2C_EE_ByteWrite(Command_AT_NUM1[0], EEPROM_Phone1);
					write_com(0x80+0x40+3);				//�����˸					
					write_com(0x0f);
				}
				if(Enter_num==3)
				{
					I2C_EE_ByteWrite(Command_AT_NUM1[1], EEPROM_Phone2);
					write_com(0x80+0x40+4);				//�����˸					
					write_com(0x0f);
				}
				if(Enter_num==4)
				{
					I2C_EE_ByteWrite(Command_AT_NUM1[2], EEPROM_Phone3);
					write_com(0x80+0x40+5);				//�����˸					
					write_com(0x0f);
				}
				if(Enter_num==5)
				{
					I2C_EE_ByteWrite(Command_AT_NUM1[3], EEPROM_Phone4);
					write_com(0x80+0x40+6);				//�����˸					
					write_com(0x0f);
				}
				if(Enter_num==6)
				{
					I2C_EE_ByteWrite(Command_AT_NUM1[4], EEPROM_Phone5);
					write_com(0x80+0x40+7);				//�����˸					
					write_com(0x0f);
				}
				if(Enter_num==7)
				{
					I2C_EE_ByteWrite(Command_AT_NUM1[5], EEPROM_Phone6);
					write_com(0x80+0x40+8);				//�����˸					
					write_com(0x0f);
				}
				if(Enter_num==8)
				{
					I2C_EE_ByteWrite(Command_AT_NUM1[6], EEPROM_Phone7);
					write_com(0x80+0x40+9);				//�����˸					
					write_com(0x0f);
				}
				if(Enter_num==9)
				{
					I2C_EE_ByteWrite(Command_AT_NUM1[7], EEPROM_Phone8);
					write_com(0x80+0x40+10);				//�����˸					
					write_com(0x0f);
				}
				if(Enter_num==10)
				{
					I2C_EE_ByteWrite(Command_AT_NUM1[8], EEPROM_Phone9);
					write_com(0x80+0x40+11);				//�����˸					
					write_com(0x0f);
				}
				if(Enter_num==11)
				{
					I2C_EE_ByteWrite(Command_AT_NUM1[9], EEPROM_Phone10);
					write_com(0x80+0x40+12);				//�����˸					
					write_com(0x0f);
				}
				if(Enter_num==12)
				{	
					I2C_EE_ByteWrite(Command_AT_NUM1[10], EEPROM_Phone11); //�Ѻ������EEPROM��Ӧ�ĵ�ַ
					Enter_num=0;
					write_com(0x0c);						//��겻����˸					
					LCD_Display_String(1,table11);
					LCD_Display_String(2,table12);
					Select_num=0;
					Adjust=0;										
				}
			}
			if(Select_num==2)								//�����¶ȷ�ֵ
			{
				if(Enter_num==1)
				{
					LCD_Display_String(1,table32);
					LCD_Display_String(2,table33);
					Write_Value(0x40+12,Set_temp);  		//��ʾ�����¶�
					write_com(0x80+0x40+12);				//�����˸					
					write_com(0x0f);
				}
				if(Enter_num==2)
				{
					Enter_num=0;
					write_com(0x0c);						//��겻����˸
					I2C_EE_ByteWrite(Set_temp, EEPROM_ADDRESS1);
					LCD_Display_String(1,table11);
					LCD_Display_String(2,table12);
					Select_num=0;
					Adjust=0;
				}
			}
			if(Select_num==3)								//�����¶ȷ�ֵ
			{
				if(Enter_num==1)
				{				
					LCD_Display_String(1,table34);
					LCD_Display_String(2,table35);
					Write_Value(0x40+12,Set_smoke);  		//��ʾ�����¶�
					write_com(0x80+0x40+12);				//�����˸					
					write_com(0x0f);
				}
				if(Enter_num==2)
				{
					Enter_num=0;
					write_com(0x0c);						//��겻����˸
					I2C_EE_ByteWrite(Set_smoke, EEPROM_ADDRESS2);
					LCD_Display_String(1,table11);
					LCD_Display_String(2,table12);
					Select_num=0;
					Adjust=0;
				}
			}
		}
	}
	
	if(UP==0)						//��������
	{
		delay1(2);					//��ʱ����
		if(UP==0)					//����ȷʵ����
		{
			while(!UP);			//�ȴ������ͷ�
			if(Enter_num==0)					//ȷ�ϰ���δ����
			{
				if(Adjust==1)					//����ģʽ
				{					
					Select_num--;
					if(Select_num<1)
					Select_num=3;
					if(Select_num==1)						//�����ֻ�����
					{
						LCD_Display_String(1,table20);
						LCD_Display_String(2,table21);
						write_com(0x80+0);					//д >
						write_date(0x3e);
						Enter_num=0;
					}
					if(Select_num==2)						//�����¶ȷ�ֵ
					{
						LCD_Display_String(1,table20);
						LCD_Display_String(2,table21);
						write_com(0x80+0x40+0);				//д >
						write_date(0x3e);
						Enter_num=0;
					}
					if(Select_num==3)						//�¶�У׼
					{
						LCD_Display_String(1,table22);
						LCD_Display_String(2,table23);
						write_com(0x80+0);					//д >
						write_date(0x3e);
						Enter_num=0;
					}
                }
			}
			if(Enter_num!=0)					//�������
			{
				if(Select_num==1)
				{
					if(Enter_num==1)
					{
						Command_AT_NUM1[0]++;						//��ֵ��1
						if(Command_AT_NUM1[0]>0x39)					//��ֵ����9
						Command_AT_NUM1[0]=0x30;					//��Ϊ0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//��ʾ��ֵ
						write_com(0x80+0x40+2);					 	//��������˸			
						write_com(0x0f);			
					}
					if(Enter_num==2)
					{
						Command_AT_NUM1[1]++;						//��ֵ��1
						if(Command_AT_NUM1[1]>0x39)					//��ֵ����9
						Command_AT_NUM1[1]=0x30;					//��Ϊ0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//��ʾ��ֵ
						write_com(0x80+0x40+3);					 	//��������˸			
						write_com(0x0f);			
					}
					if(Enter_num==3)
					{
						Command_AT_NUM1[2]++;						//��ֵ��1
						if(Command_AT_NUM1[2]>0x39)					//��ֵ����9
						Command_AT_NUM1[2]=0x30;					//��Ϊ0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//��ʾ��ֵ
						write_com(0x80+0x40+4);					 	//��������˸			
						write_com(0x0f);			
					}
					if(Enter_num==4)
					{
						Command_AT_NUM1[3]++;						//��ֵ��1
						if(Command_AT_NUM1[3]>0x39)					//��ֵ����9
						Command_AT_NUM1[3]=0x30;					//��Ϊ0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//��ʾ��ֵ
						write_com(0x80+0x40+5);					 	//��������˸			
						write_com(0x0f);			
					}
					if(Enter_num==5)
					{
						Command_AT_NUM1[4]++;						//��ֵ��1
						if(Command_AT_NUM1[4]>0x39)					//��ֵ����9
						Command_AT_NUM1[4]=0x30;					//��Ϊ0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//��ʾ��ֵ
						write_com(0x80+0x40+6);					 	//��������˸			
						write_com(0x0f);			
					}
					if(Enter_num==6)
					{
						Command_AT_NUM1[5]++;						//��ֵ��1
						if(Command_AT_NUM1[5]>0x39)					//��ֵ����9
						Command_AT_NUM1[5]=0x30;					//��Ϊ0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//��ʾ��ֵ
						write_com(0x80+0x40+7);					 	//��������˸			
						write_com(0x0f);			
					}
					if(Enter_num==7)
					{
						Command_AT_NUM1[6]++;						//��ֵ��1
						if(Command_AT_NUM1[6]>0x39)					//��ֵ����9
						Command_AT_NUM1[6]=0x30;					//��Ϊ0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//��ʾ��ֵ
						write_com(0x80+0x40+8);					 	//��������˸			
						write_com(0x0f);			
					}
					if(Enter_num==8)
					{
						Command_AT_NUM1[7]++;						//��ֵ��1
						if(Command_AT_NUM1[7]>0x39)					//��ֵ����9
						Command_AT_NUM1[7]=0x30;					//��Ϊ0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//��ʾ��ֵ
						write_com(0x80+0x40+9);					 	//��������˸			
						write_com(0x0f);			
					}
					if(Enter_num==9)
					{
						Command_AT_NUM1[8]++;						//��ֵ��1
						if(Command_AT_NUM1[8]>0x39)					//��ֵ����9
						Command_AT_NUM1[8]=0x30;					//��Ϊ0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//��ʾ��ֵ
						write_com(0x80+0x40+10);					 	//��������˸			
						write_com(0x0f);			
					}
					if(Enter_num==10)
					{
						Command_AT_NUM1[9]++;						//��ֵ��1
						if(Command_AT_NUM1[9]>0x39)					//��ֵ����9
						Command_AT_NUM1[9]=0x30;					//��Ϊ0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//��ʾ��ֵ
						write_com(0x80+0x40+11);					 	//��������˸			
						write_com(0x0f);			
					}
					if(Enter_num==11)
					{
						Command_AT_NUM1[10]++;						//��ֵ��1
						if(Command_AT_NUM1[10]>0x39)				//��ֵ����9
						Command_AT_NUM1[10]=0x30;					//��Ϊ0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//��ʾ��ֵ
						write_com(0x80+0x40+12);					 	//��������˸			
						write_com(0x0f);			
					}				
				}
				if(Select_num==2)
				{			
					Set_temp++;				//���õ��¶�ֵ��һ
					if(Set_temp==100)		//�¶�ֵ����100
					Set_temp=0;		    	//����
					Write_Value(0x40+12,Set_temp);  		//��ʾ�����¶�
					write_com(0x80+0x40+12);				//�����˸					
					write_com(0x0f);				
				}
				if(Select_num==3)
				{			
					Set_smoke++;				//����ֵ��һ
					if(Set_smoke==100)			//ֵ����100
					Set_smoke=0;		    	//����
					Write_Value(0x40+12,Set_smoke);  		//��ʾ�����¶�
					write_com(0x80+0x40+12);				//�����˸					
					write_com(0x0f);				
				}
			}			
		}
	}
	if(DOWN==0)						//��������
	{
		delay1(2);					//��ʱ����
		if(DOWN==0)					//����ȷʵ����
		{
			while(!DOWN);			//�ȴ������ͷ�
			
			if(Enter_num==0)					//ȷ�ϰ���δ����
			{
				if(Adjust==1)					//����ģʽ
				{
					Select_num++;
					if(Select_num>3)
					Select_num=1;
					if(Select_num==1)						//�����ֻ�����
					{
						LCD_Display_String(1,table20);
						LCD_Display_String(2,table21);
						write_com(0x80+0);					//д >
						write_date(0x3e);
						Enter_num=0;
					}
					if(Select_num==2)						//�����¶ȷ�ֵ
					{
						LCD_Display_String(1,table20);
						LCD_Display_String(2,table21);
						write_com(0x80+0x40+0);				//д >
						write_date(0x3e);
						Enter_num=0;
					}
					if(Select_num==3)						//�¶�У׼
					{
						LCD_Display_String(1,table22);
						LCD_Display_String(2,table23);
						write_com(0x80+0);					//д >
						write_date(0x3e);
						Enter_num=0;
					}
				}
			}
			if(Enter_num!=0)					//�������
			{
				if(Select_num==1)
				{
					if(Enter_num==1)
					{
						Command_AT_NUM1[0]--;						//��ֵ��1
						if(Command_AT_NUM1[0]<0x30)					//��ֵ����9
						Command_AT_NUM1[0]=0x39;					//��Ϊ0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//��ʾ��ֵ
						write_com(0x80+0x40+2);					 	//��������˸			
						write_com(0x0f);			
					}
					if(Enter_num==2)
					{
						Command_AT_NUM1[1]--;						//��ֵ��1
						if(Command_AT_NUM1[1]<0x30)					//��ֵ����9
						Command_AT_NUM1[1]=0x39;					//��Ϊ0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//��ʾ��ֵ
						write_com(0x80+0x40+3);					 	//��������˸			
						write_com(0x0f);			
					}
					if(Enter_num==3)
					{
						Command_AT_NUM1[2]--;						//��ֵ��1
						if(Command_AT_NUM1[2]<0x30)					//��ֵ����9
						Command_AT_NUM1[2]=0x39;					//��Ϊ0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//��ʾ��ֵ
						write_com(0x80+0x40+4);					 	//��������˸			
						write_com(0x0f);			
					}
					if(Enter_num==4)
					{
						Command_AT_NUM1[3]--;						//��ֵ��1
						if(Command_AT_NUM1[3]<0x30)					//��ֵ����9
						Command_AT_NUM1[3]=0x39;					//��Ϊ0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//��ʾ��ֵ
						write_com(0x80+0x40+5);					 	//��������˸			
						write_com(0x0f);			
					}
					if(Enter_num==5)
					{
						Command_AT_NUM1[4]--;						//��ֵ��1
						if(Command_AT_NUM1[4]<0x30)					//��ֵ����9
						Command_AT_NUM1[4]=0x39;					//��Ϊ0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//��ʾ��ֵ
						write_com(0x80+0x40+6);					 	//��������˸			
						write_com(0x0f);			
					}
					if(Enter_num==6)
					{
						Command_AT_NUM1[5]--;						//��ֵ��1
						if(Command_AT_NUM1[5]<0x30)					//��ֵ����9
						Command_AT_NUM1[5]=0x39;					//��Ϊ0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//��ʾ��ֵ
						write_com(0x80+0x40+7);					 	//��������˸			
						write_com(0x0f);			
					}
					if(Enter_num==7)
					{
						Command_AT_NUM1[6]--;						//��ֵ��1
						if(Command_AT_NUM1[6]<0x30)					//��ֵ����9
						Command_AT_NUM1[6]=0x39;					//��Ϊ0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//��ʾ��ֵ
						write_com(0x80+0x40+8);					 	//��������˸			
						write_com(0x0f);			
					}
					if(Enter_num==8)
					{
						Command_AT_NUM1[7]--;						//��ֵ��1
						if(Command_AT_NUM1[7]<0x30)					//��ֵ����9
						Command_AT_NUM1[7]=0x39;					//��Ϊ0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//��ʾ��ֵ
						write_com(0x80+0x40+9);					 	//��������˸			
						write_com(0x0f);			
					}
					if(Enter_num==9)
					{
						Command_AT_NUM1[8]--;						//��ֵ��1
						if(Command_AT_NUM1[8]<0x30)					//��ֵ����9
						Command_AT_NUM1[8]=0x39;					//��Ϊ0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//��ʾ��ֵ
						write_com(0x80+0x40+10);					 	//��������˸			
						write_com(0x0f);			
					}
					if(Enter_num==10)
					{
						Command_AT_NUM1[9]--;						//��ֵ��1
						if(Command_AT_NUM1[9]<0x30)					//��ֵ����9
						Command_AT_NUM1[9]=0x39;					//��Ϊ0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//��ʾ��ֵ
						write_com(0x80+0x40+11);					 	//��������˸			
						write_com(0x0f);			
					}
					if(Enter_num==11)
					{
						Command_AT_NUM1[10]--;						//��ֵ��1
						if(Command_AT_NUM1[10]<0x30)				//��ֵ����9
						Command_AT_NUM1[10]=0x39;					//��Ϊ0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//��ʾ��ֵ
						write_com(0x80+0x40+12);					 	//��������˸			
						write_com(0x0f);			
					}
				}
				
				if(Select_num==2)
				{
					Set_temp--;				//���õ��¶�ֵ��һ
					if(Set_temp<0)			//�¶�ֵС��0
					Set_temp=99;			//��Ϊ���ֵ
					Write_Value(0x40+12,Set_temp);  		//��ʾ�����¶�
					write_com(0x80+0x40+12);				//�����˸					
					write_com(0x0f);
				}
				if(Select_num==3)
				{
					Set_smoke--;			//����ֵ��һ
					if(Set_smoke<0)			//ֵС��0
					Set_smoke=99;			//��Ϊ���ֵ
					Write_Value(0x40+12,Set_smoke);  		//��ʾ�����¶�
					write_com(0x80+0x40+12);				//�����˸					
					write_com(0x0f);
				}
			}
		}
	}	

	if(SWITCH==0)
	{
		delay1(2);
		if(SWITCH==0)
		{
			while(!SWITCH);
			Switch_flag=!Switch_flag;				   		   //״̬ȡ��
			if(Switch_flag==0)							   //�ر�״̬
			{
				LED=1;                                     //�رչ���
				Steal_flag=0;
				Count=0;
			}
			if(Switch_flag==1)							   //����״̬
			{
				LED=0;
			}
		}
	}
}


