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

uchar Command_AT_NUM1[]="00000000000";			//指定号码1
uchar table20[]=" 1.Phone Number ";			 	//设置报警号码
uchar table21[]=" 2.Set Temper   ";			 	//设置温度阀值
uchar table22[]=" 3.Set Smoke    ";			 	//设置烟雾浓度阈值
uchar table23[]="                ";

uchar table30[]="Set Phone Number";			 	//设置号码
uchar table31[]="--           ---";			 	//--号码--
uchar table32[]="Set Temperature ";			 	//设置温度
uchar table33[]="              C ";			 	//
uchar table34[]="Set Smoke Concen";			 	//设置温度
uchar table35[]="              % ";			 	//

uchar table11[]="NOW:    %     C ";				//液晶屏显示内容
uchar table12[]="SET:    %     C ";				//液晶屏显示内容

uchar Select_num;	  //选择按键按下次数
uchar Enter_num;	  //确认按键按下次数
uchar Adjust;
uchar Steal_flag;
uchar Switch_flag;
uchar Count;

void delay1(uint z)			  					 //延时约3ms函数
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=5000;y>0;y--);
}

void GPIO_Configuration_KEY(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;													//定义

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); 			//使能GPIOB APB2外设时钟			 
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;//选中管脚0 1 10，即GPIOB0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; 						//上拉输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);								//以上设结构体置应用到GPIOB	
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;							//选中管脚0 1 10，即GPIOB0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; 						//上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);								//以上设结构体置应用到GPIOA
	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	LED=1;
	
	Command_AT_NUM1[0]=I2C_EE_BufferRead(EEPROM_Phone1);				//读取对应地址的值
	Command_AT_NUM1[1]=I2C_EE_BufferRead(EEPROM_Phone2);				//读取对应地址的值
	Command_AT_NUM1[2]=I2C_EE_BufferRead(EEPROM_Phone3);				//读取对应地址的值
	Command_AT_NUM1[3]=I2C_EE_BufferRead(EEPROM_Phone4);				//读取对应地址的值
	Command_AT_NUM1[4]=I2C_EE_BufferRead(EEPROM_Phone5);				//读取对应地址的值
	Command_AT_NUM1[5]=I2C_EE_BufferRead(EEPROM_Phone6);				//读取对应地址的值
	Command_AT_NUM1[6]=I2C_EE_BufferRead(EEPROM_Phone7);				//读取对应地址的值
	Command_AT_NUM1[7]=I2C_EE_BufferRead(EEPROM_Phone8);				//读取对应地址的值
	Command_AT_NUM1[8]=I2C_EE_BufferRead(EEPROM_Phone9);				//读取对应地址的值
	Command_AT_NUM1[9]=I2C_EE_BufferRead(EEPROM_Phone10);				//读取对应地址的值
	Command_AT_NUM1[10]=I2C_EE_BufferRead(EEPROM_Phone11);				//读取对应地址的值
}

void KEY(void)
{
	if(SELT==0)
	{
		delay1(2);
		if(SELT==0)
		{	
			while(!SELT);
			Adjust=!Adjust;							//进入调节模式
			if(Adjust==1)
			{	
				Select_num=1;						//设置手机号码
				LCD_Display_String(1,table20);
				LCD_Display_String(2,table21);
				write_com(0x80+0);					//写 >
				write_date(0x3e);
				Enter_num=0;
			}
			else									//回到初始界面
			{
				LCD_Display_String(1,table11);
				LCD_Display_String(2,table12);
				Select_num=0;
				Enter_num=0;					
				Adjust=0;
			}
			write_com(0x0c);						//光标不再闪烁
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
		
			if(Select_num==1)						//设置手机号码
			{
				if(Enter_num==1)
				{
					LCD_Display_String(1,table30);
					LCD_Display_String(2,table31);
					LCD_Display_PhoneNumber(2,Command_AT_NUM1);//显示个号码
					write_com(0x80+0x40+2);				//光标闪烁					
					write_com(0x0f);
				}
				if(Enter_num==2)
				{
					I2C_EE_ByteWrite(Command_AT_NUM1[0], EEPROM_Phone1);
					write_com(0x80+0x40+3);				//光标闪烁					
					write_com(0x0f);
				}
				if(Enter_num==3)
				{
					I2C_EE_ByteWrite(Command_AT_NUM1[1], EEPROM_Phone2);
					write_com(0x80+0x40+4);				//光标闪烁					
					write_com(0x0f);
				}
				if(Enter_num==4)
				{
					I2C_EE_ByteWrite(Command_AT_NUM1[2], EEPROM_Phone3);
					write_com(0x80+0x40+5);				//光标闪烁					
					write_com(0x0f);
				}
				if(Enter_num==5)
				{
					I2C_EE_ByteWrite(Command_AT_NUM1[3], EEPROM_Phone4);
					write_com(0x80+0x40+6);				//光标闪烁					
					write_com(0x0f);
				}
				if(Enter_num==6)
				{
					I2C_EE_ByteWrite(Command_AT_NUM1[4], EEPROM_Phone5);
					write_com(0x80+0x40+7);				//光标闪烁					
					write_com(0x0f);
				}
				if(Enter_num==7)
				{
					I2C_EE_ByteWrite(Command_AT_NUM1[5], EEPROM_Phone6);
					write_com(0x80+0x40+8);				//光标闪烁					
					write_com(0x0f);
				}
				if(Enter_num==8)
				{
					I2C_EE_ByteWrite(Command_AT_NUM1[6], EEPROM_Phone7);
					write_com(0x80+0x40+9);				//光标闪烁					
					write_com(0x0f);
				}
				if(Enter_num==9)
				{
					I2C_EE_ByteWrite(Command_AT_NUM1[7], EEPROM_Phone8);
					write_com(0x80+0x40+10);				//光标闪烁					
					write_com(0x0f);
				}
				if(Enter_num==10)
				{
					I2C_EE_ByteWrite(Command_AT_NUM1[8], EEPROM_Phone9);
					write_com(0x80+0x40+11);				//光标闪烁					
					write_com(0x0f);
				}
				if(Enter_num==11)
				{
					I2C_EE_ByteWrite(Command_AT_NUM1[9], EEPROM_Phone10);
					write_com(0x80+0x40+12);				//光标闪烁					
					write_com(0x0f);
				}
				if(Enter_num==12)
				{	
					I2C_EE_ByteWrite(Command_AT_NUM1[10], EEPROM_Phone11); //把号码存入EEPROM对应的地址
					Enter_num=0;
					write_com(0x0c);						//光标不再闪烁					
					LCD_Display_String(1,table11);
					LCD_Display_String(2,table12);
					Select_num=0;
					Adjust=0;										
				}
			}
			if(Select_num==2)								//设置温度阀值
			{
				if(Enter_num==1)
				{
					LCD_Display_String(1,table32);
					LCD_Display_String(2,table33);
					Write_Value(0x40+12,Set_temp);  		//显示设置温度
					write_com(0x80+0x40+12);				//光标闪烁					
					write_com(0x0f);
				}
				if(Enter_num==2)
				{
					Enter_num=0;
					write_com(0x0c);						//光标不再闪烁
					I2C_EE_ByteWrite(Set_temp, EEPROM_ADDRESS1);
					LCD_Display_String(1,table11);
					LCD_Display_String(2,table12);
					Select_num=0;
					Adjust=0;
				}
			}
			if(Select_num==3)								//设置温度阀值
			{
				if(Enter_num==1)
				{				
					LCD_Display_String(1,table34);
					LCD_Display_String(2,table35);
					Write_Value(0x40+12,Set_smoke);  		//显示设置温度
					write_com(0x80+0x40+12);				//光标闪烁					
					write_com(0x0f);
				}
				if(Enter_num==2)
				{
					Enter_num=0;
					write_com(0x0c);						//光标不再闪烁
					I2C_EE_ByteWrite(Set_smoke, EEPROM_ADDRESS2);
					LCD_Display_String(1,table11);
					LCD_Display_String(2,table12);
					Select_num=0;
					Adjust=0;
				}
			}
		}
	}
	
	if(UP==0)						//按键按下
	{
		delay1(2);					//延时消抖
		if(UP==0)					//按键确实按下
		{
			while(!UP);			//等待按键释放
			if(Enter_num==0)					//确认按键未按下
			{
				if(Adjust==1)					//设置模式
				{					
					Select_num--;
					if(Select_num<1)
					Select_num=3;
					if(Select_num==1)						//设置手机号码
					{
						LCD_Display_String(1,table20);
						LCD_Display_String(2,table21);
						write_com(0x80+0);					//写 >
						write_date(0x3e);
						Enter_num=0;
					}
					if(Select_num==2)						//设置温度阀值
					{
						LCD_Display_String(1,table20);
						LCD_Display_String(2,table21);
						write_com(0x80+0x40+0);				//写 >
						write_date(0x3e);
						Enter_num=0;
					}
					if(Select_num==3)						//温度校准
					{
						LCD_Display_String(1,table22);
						LCD_Display_String(2,table23);
						write_com(0x80+0);					//写 >
						write_date(0x3e);
						Enter_num=0;
					}
                }
			}
			if(Enter_num!=0)					//进入调节
			{
				if(Select_num==1)
				{
					if(Enter_num==1)
					{
						Command_AT_NUM1[0]++;						//数值加1
						if(Command_AT_NUM1[0]>0x39)					//数值超过9
						Command_AT_NUM1[0]=0x30;					//变为0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//显示数值
						write_com(0x80+0x40+2);					 	//光标继续闪烁			
						write_com(0x0f);			
					}
					if(Enter_num==2)
					{
						Command_AT_NUM1[1]++;						//数值加1
						if(Command_AT_NUM1[1]>0x39)					//数值超过9
						Command_AT_NUM1[1]=0x30;					//变为0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//显示数值
						write_com(0x80+0x40+3);					 	//光标继续闪烁			
						write_com(0x0f);			
					}
					if(Enter_num==3)
					{
						Command_AT_NUM1[2]++;						//数值加1
						if(Command_AT_NUM1[2]>0x39)					//数值超过9
						Command_AT_NUM1[2]=0x30;					//变为0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//显示数值
						write_com(0x80+0x40+4);					 	//光标继续闪烁			
						write_com(0x0f);			
					}
					if(Enter_num==4)
					{
						Command_AT_NUM1[3]++;						//数值加1
						if(Command_AT_NUM1[3]>0x39)					//数值超过9
						Command_AT_NUM1[3]=0x30;					//变为0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//显示数值
						write_com(0x80+0x40+5);					 	//光标继续闪烁			
						write_com(0x0f);			
					}
					if(Enter_num==5)
					{
						Command_AT_NUM1[4]++;						//数值加1
						if(Command_AT_NUM1[4]>0x39)					//数值超过9
						Command_AT_NUM1[4]=0x30;					//变为0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//显示数值
						write_com(0x80+0x40+6);					 	//光标继续闪烁			
						write_com(0x0f);			
					}
					if(Enter_num==6)
					{
						Command_AT_NUM1[5]++;						//数值加1
						if(Command_AT_NUM1[5]>0x39)					//数值超过9
						Command_AT_NUM1[5]=0x30;					//变为0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//显示数值
						write_com(0x80+0x40+7);					 	//光标继续闪烁			
						write_com(0x0f);			
					}
					if(Enter_num==7)
					{
						Command_AT_NUM1[6]++;						//数值加1
						if(Command_AT_NUM1[6]>0x39)					//数值超过9
						Command_AT_NUM1[6]=0x30;					//变为0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//显示数值
						write_com(0x80+0x40+8);					 	//光标继续闪烁			
						write_com(0x0f);			
					}
					if(Enter_num==8)
					{
						Command_AT_NUM1[7]++;						//数值加1
						if(Command_AT_NUM1[7]>0x39)					//数值超过9
						Command_AT_NUM1[7]=0x30;					//变为0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//显示数值
						write_com(0x80+0x40+9);					 	//光标继续闪烁			
						write_com(0x0f);			
					}
					if(Enter_num==9)
					{
						Command_AT_NUM1[8]++;						//数值加1
						if(Command_AT_NUM1[8]>0x39)					//数值超过9
						Command_AT_NUM1[8]=0x30;					//变为0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//显示数值
						write_com(0x80+0x40+10);					 	//光标继续闪烁			
						write_com(0x0f);			
					}
					if(Enter_num==10)
					{
						Command_AT_NUM1[9]++;						//数值加1
						if(Command_AT_NUM1[9]>0x39)					//数值超过9
						Command_AT_NUM1[9]=0x30;					//变为0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//显示数值
						write_com(0x80+0x40+11);					 	//光标继续闪烁			
						write_com(0x0f);			
					}
					if(Enter_num==11)
					{
						Command_AT_NUM1[10]++;						//数值加1
						if(Command_AT_NUM1[10]>0x39)				//数值超过9
						Command_AT_NUM1[10]=0x30;					//变为0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//显示数值
						write_com(0x80+0x40+12);					 	//光标继续闪烁			
						write_com(0x0f);			
					}				
				}
				if(Select_num==2)
				{			
					Set_temp++;				//设置的温度值加一
					if(Set_temp==100)		//温度值到达100
					Set_temp=0;		    	//清零
					Write_Value(0x40+12,Set_temp);  		//显示设置温度
					write_com(0x80+0x40+12);				//光标闪烁					
					write_com(0x0f);				
				}
				if(Select_num==3)
				{			
					Set_smoke++;				//设置值加一
					if(Set_smoke==100)			//值到达100
					Set_smoke=0;		    	//清零
					Write_Value(0x40+12,Set_smoke);  		//显示设置温度
					write_com(0x80+0x40+12);				//光标闪烁					
					write_com(0x0f);				
				}
			}			
		}
	}
	if(DOWN==0)						//按键按下
	{
		delay1(2);					//延时消抖
		if(DOWN==0)					//按键确实按下
		{
			while(!DOWN);			//等待按键释放
			
			if(Enter_num==0)					//确认按键未按下
			{
				if(Adjust==1)					//设置模式
				{
					Select_num++;
					if(Select_num>3)
					Select_num=1;
					if(Select_num==1)						//设置手机号码
					{
						LCD_Display_String(1,table20);
						LCD_Display_String(2,table21);
						write_com(0x80+0);					//写 >
						write_date(0x3e);
						Enter_num=0;
					}
					if(Select_num==2)						//设置温度阀值
					{
						LCD_Display_String(1,table20);
						LCD_Display_String(2,table21);
						write_com(0x80+0x40+0);				//写 >
						write_date(0x3e);
						Enter_num=0;
					}
					if(Select_num==3)						//温度校准
					{
						LCD_Display_String(1,table22);
						LCD_Display_String(2,table23);
						write_com(0x80+0);					//写 >
						write_date(0x3e);
						Enter_num=0;
					}
				}
			}
			if(Enter_num!=0)					//进入调节
			{
				if(Select_num==1)
				{
					if(Enter_num==1)
					{
						Command_AT_NUM1[0]--;						//数值加1
						if(Command_AT_NUM1[0]<0x30)					//数值超过9
						Command_AT_NUM1[0]=0x39;					//变为0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//显示数值
						write_com(0x80+0x40+2);					 	//光标继续闪烁			
						write_com(0x0f);			
					}
					if(Enter_num==2)
					{
						Command_AT_NUM1[1]--;						//数值加1
						if(Command_AT_NUM1[1]<0x30)					//数值超过9
						Command_AT_NUM1[1]=0x39;					//变为0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//显示数值
						write_com(0x80+0x40+3);					 	//光标继续闪烁			
						write_com(0x0f);			
					}
					if(Enter_num==3)
					{
						Command_AT_NUM1[2]--;						//数值加1
						if(Command_AT_NUM1[2]<0x30)					//数值超过9
						Command_AT_NUM1[2]=0x39;					//变为0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//显示数值
						write_com(0x80+0x40+4);					 	//光标继续闪烁			
						write_com(0x0f);			
					}
					if(Enter_num==4)
					{
						Command_AT_NUM1[3]--;						//数值加1
						if(Command_AT_NUM1[3]<0x30)					//数值超过9
						Command_AT_NUM1[3]=0x39;					//变为0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//显示数值
						write_com(0x80+0x40+5);					 	//光标继续闪烁			
						write_com(0x0f);			
					}
					if(Enter_num==5)
					{
						Command_AT_NUM1[4]--;						//数值加1
						if(Command_AT_NUM1[4]<0x30)					//数值超过9
						Command_AT_NUM1[4]=0x39;					//变为0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//显示数值
						write_com(0x80+0x40+6);					 	//光标继续闪烁			
						write_com(0x0f);			
					}
					if(Enter_num==6)
					{
						Command_AT_NUM1[5]--;						//数值加1
						if(Command_AT_NUM1[5]<0x30)					//数值超过9
						Command_AT_NUM1[5]=0x39;					//变为0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//显示数值
						write_com(0x80+0x40+7);					 	//光标继续闪烁			
						write_com(0x0f);			
					}
					if(Enter_num==7)
					{
						Command_AT_NUM1[6]--;						//数值加1
						if(Command_AT_NUM1[6]<0x30)					//数值超过9
						Command_AT_NUM1[6]=0x39;					//变为0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//显示数值
						write_com(0x80+0x40+8);					 	//光标继续闪烁			
						write_com(0x0f);			
					}
					if(Enter_num==8)
					{
						Command_AT_NUM1[7]--;						//数值加1
						if(Command_AT_NUM1[7]<0x30)					//数值超过9
						Command_AT_NUM1[7]=0x39;					//变为0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//显示数值
						write_com(0x80+0x40+9);					 	//光标继续闪烁			
						write_com(0x0f);			
					}
					if(Enter_num==9)
					{
						Command_AT_NUM1[8]--;						//数值加1
						if(Command_AT_NUM1[8]<0x30)					//数值超过9
						Command_AT_NUM1[8]=0x39;					//变为0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//显示数值
						write_com(0x80+0x40+10);					 	//光标继续闪烁			
						write_com(0x0f);			
					}
					if(Enter_num==10)
					{
						Command_AT_NUM1[9]--;						//数值加1
						if(Command_AT_NUM1[9]<0x30)					//数值超过9
						Command_AT_NUM1[9]=0x39;					//变为0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//显示数值
						write_com(0x80+0x40+11);					 	//光标继续闪烁			
						write_com(0x0f);			
					}
					if(Enter_num==11)
					{
						Command_AT_NUM1[10]--;						//数值加1
						if(Command_AT_NUM1[10]<0x30)				//数值超过9
						Command_AT_NUM1[10]=0x39;					//变为0
						LCD_Display_PhoneNumber(2,Command_AT_NUM1);	//显示数值
						write_com(0x80+0x40+12);					 	//光标继续闪烁			
						write_com(0x0f);			
					}
				}
				
				if(Select_num==2)
				{
					Set_temp--;				//设置的温度值减一
					if(Set_temp<0)			//温度值小于0
					Set_temp=99;			//变为最大值
					Write_Value(0x40+12,Set_temp);  		//显示设置温度
					write_com(0x80+0x40+12);				//光标闪烁					
					write_com(0x0f);
				}
				if(Select_num==3)
				{
					Set_smoke--;			//设置值减一
					if(Set_smoke<0)			//值小于0
					Set_smoke=99;			//变为最大值
					Write_Value(0x40+12,Set_smoke);  		//显示设置温度
					write_com(0x80+0x40+12);				//光标闪烁					
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
			Switch_flag=!Switch_flag;				   		   //状态取反
			if(Switch_flag==0)							   //关闭状态
			{
				LED=1;                                     //关闭功能
				Steal_flag=0;
				Count=0;
			}
			if(Switch_flag==1)							   //开启状态
			{
				LED=0;
			}
		}
	}
}


