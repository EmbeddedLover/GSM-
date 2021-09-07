#include "usart.h"
#include <ds18b20.h>
#include "ADC.h"
#include "I2C.h"
#include "display.h"
#include "GPIOLIKE51.h"

#define JDQ BIT_ADDR(GPIOA_ODR_Addr,12)             //继电器接口

char NEW_SMS;										//短消息提示标志位
char READ_SMS;										//读取短消息标志位
char ATH;											//挂机标志位
uchar UART0_Data[128];								//存放接收到的数据
uchar UART0_Len;									//定义长度


uchar Command_AT[]="AT\r\n";						//发送AT，建立连接
uchar Command_AT_CMGF[]="AT+CMGF=1\r\n";			//把格式设置为text格式
uchar Command_AT_CMGS[]="AT+CMGS=";					//发送到指定号码指令
uchar Command_AT_YIN[]={0x22};						//发送指定号码双引号
uchar Command_AT_ENTER[]="\r\n";					//发送指定号码双引号
uchar Command_AT_NUM[]="           ";				//发送到指定号码
uchar Command_AT_IPR[]="AT+IPR=9600\r\n";			//设置波特率
uchar Command_AT_CMGR[]="AT+CMGR=  \r\n";	  		//读第几条短信
uchar Command_AT_CMGD[]="AT+CMGD=  \r\n";	  		//删除第几条短信
uchar Command_AT_CMGD1[]="AT+CMGD=01\r\n";	  		//删除第1条短信
uchar Command_AT_CNMI[]="AT+CNMI=1,1,2\r\n";		//设置短消息提示
uchar Command_AT_CLIP[]="AT+CLIP=1\r\n";			//来电显示
uchar Command_AT_CRC[]="AT+CRC=0\r\n";				//设置来电开头为RING，有电话打入收到RING提醒
uchar Command_ATH[]="ATH\r\n";						//挂电话
uchar Command_ATE[]="ATE1\r\n";						//设置回显
uchar Command_AT_End[]={0x1A};						//发送短信内容结束符
uchar Command_NUM[]="0123456789+-";					//短信里温度值提取的数字
uchar Command_AT_Tem[44]="   C, Temperature exceed limit, please note!";	  	//温度超过界限，请注意！
uchar Command_Someone[34]="Someone in the house, please note!";		  					//有人在屋里，请注意！
uchar Command_Smoke[56]="  %, Combustible gas or smoke exceed limit, please note!";   //烟雾或可燃性气体超过界限，请注意！
uchar Command_Smoke1[6]=" &   %";
uchar Command_Opened[13]="It is opened!";			//打开
uchar Command_Closed[13]="It is closed!";			//关闭
uchar OPEN[]="#OPEN#";	  					 		//开启
uchar CLOSE[]="#CLOSE#";	  					 	//关闭

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
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);               //RCC中打开相应串口
	                                                                    //RCC是STM32的时钟控制器，可开启或关闭各总线的时钟，在使用各外设功能必须先开启其对应的时钟，没有这个时钟内部的各器件就不能运行。
	USART_InitStructure.USART_BaudRate = 9600; 							//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 		//8位长度
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 				//1位停止位
	USART_InitStructure.USART_Parity = USART_Parity_No ; 				//奇偶失能 无检验位
	USART_InitStructure.USART_HardwareFlowControl = 
	USART_HardwareFlowControl_None; 									//无硬件流控
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; 	//使能发送 接收  收发模式
	USART_Init(USART1, &USART_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                      //使能接收中断
	USART_Cmd(USART1, ENABLE);                                          //使能USART1
	
    
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;					//TX
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;           //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//最高输出50MHz
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;                //RX
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;     //复用开漏输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//最高输出50MHz
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); 						 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void Send_Command(uchar Value[],uchar Len)				//发送指令，Value[]表示发送哪个数组，Len表示发送该数组里的几位
{
	uchar i;
	for(i=0;i<Len;i++)
	{
		USART_SendData(USART1, Value[i]);  				//发送1bit数据
		delay2(500);
	}
}

void GSM_Init(void)					  	  //GSM模块初始化
{
	Send_Command(Command_ATE,5);		  //设置回显
	delay2(10000);
	Send_Command(Command_AT,3);			  //建立连接
	delay2(100);
	Send_Command(Command_AT_CMGF,10);	  //设置为text格式
	delay2(20000);
	Send_Command(Command_AT_CNMI,14);	  //设置为设置为中断方式接收短信，即GSM接收短信后向下位机串口发送 +CMTI: "SM",1 。
	delay2(30000);
	Send_Command(Command_AT_CLIP,10);	  //设置来电提示  RING   +CLIP:
	delay2(10000);
	Send_Command(Command_AT_CRC,9);	  	  //设置来电提示  RING   +CLIP:
	delay2(10000);
	Send_Command(Command_AT_IPR,12);	  //设置波特率=9600
	delay2(10000);
	Send_Command(Command_AT_CMGD1,11);	  		//删除短信第一条短信息,腾出空间接收短信
	delay2(10000);
}
void Send_Message(void)					  	  //发送到指定号码
{
	Command_AT_NUM[0]=I2C_EE_BufferRead(EEPROM_Phone1);				//读取对应地址的值
	Command_AT_NUM[1]=I2C_EE_BufferRead(EEPROM_Phone2);				//读取对应地址的值
	Command_AT_NUM[2]=I2C_EE_BufferRead(EEPROM_Phone3);				//读取对应地址的值
	Command_AT_NUM[3]=I2C_EE_BufferRead(EEPROM_Phone4);				//读取对应地址的值
	Command_AT_NUM[4]=I2C_EE_BufferRead(EEPROM_Phone5);				//读取对应地址的值
	Command_AT_NUM[5]=I2C_EE_BufferRead(EEPROM_Phone6);				//读取对应地址的值
	Command_AT_NUM[6]=I2C_EE_BufferRead(EEPROM_Phone7);				//读取对应地址的值
	Command_AT_NUM[7]=I2C_EE_BufferRead(EEPROM_Phone8);				//读取对应地址的值
	Command_AT_NUM[8]=I2C_EE_BufferRead(EEPROM_Phone9);				//读取对应地址的值
	Command_AT_NUM[9]=I2C_EE_BufferRead(EEPROM_Phone10);			//读取对应地址的值
	Command_AT_NUM[10]=I2C_EE_BufferRead(EEPROM_Phone11);			//读取对应地址的值
	
	Send_Command(Command_AT_CMGS,8);		  //发送指令
	delay2(5000);							  
	Send_Command(Command_AT_YIN,1);		      //发送引号
	delay2(1000);
	Send_Command(Command_AT_NUM,11);		  //发送号码
	delay2(1000);
	Send_Command(Command_AT_YIN,1);		      //发送引号
	delay2(1000);
	Send_Command(Command_AT_ENTER,1);		  //发送回车
	delay2(1000);
}	
void Send_Message1(void)					  //发送警报浓度
{	
	Command_Smoke[0]=Command_NUM[Smoke/10];	  //把十位的数字存到Command_Smoke[0]
	Command_Smoke[1]=Command_NUM[Smoke%10];	  //把个位的数字存到Command_Smoke[1]
	Send_Message();							  //发送到指定号码
	delay2(10000);							  //等待回复输入符号
	Send_Command(Command_Smoke,56);		      //发送烟雾报警信息
	delay2(10000);
	Send_Command(Command_AT_End,1);			  //发送短信结束符
	delay2(1000);
}

void Send_Message2(void)					  //发送入室盗贼短信息
{
	Send_Message();							  //发送到指定号码
	delay2(10000);							  //等待回复输入符号
	Send_Command(Command_Someone,34);		  //发送警报消息
	delay2(10000);
	Send_Command(Command_AT_End,1);			  //发送短信结束符
	delay2(1000);
}
void Send_Message3(void)					  //发送警报温度
{
	if((temperature>0)&(temperature<100)) 			//温度在正常范围内转换
	{	
		if(Fuhao==1)
		Command_AT_Tem[0]=Command_NUM[10];				//Command_AT_Tem[0]为+，表示温度大于或等于0
		if(Fuhao==0)
		Command_AT_Tem[0]=Command_NUM[11];				//Command_AT_Tem[0]为-，表示温度小于0
		
		Command_AT_Tem[1]=Command_NUM[temperature/10];	//把十位的数字存到Command_AT_Tem[1]
		Command_AT_Tem[2]=Command_NUM[temperature%10];	//把个位的数字存到Command_AT_Tem[2]
	}
	Send_Message();							  //发送到指定号码
	delay2(10000);							  //等待回复输入符号
	Send_Command(Command_AT_Tem,44);		  //发送温度
	delay2(10000);
	Send_Command(Command_AT_End,1);			  //发送短信结束符
	delay2(1000);
}
void Send_Message4(void)					  //发送当前温度
{
	if((temperature>0)&(temperature<100)) 			//温度在正常范围内转换
	{	
		if(Fuhao==1)
		Command_AT_Tem[0]=Command_NUM[10];				//Command_AT_Tem[0]为+，表示温度大于或等于0
		if(Fuhao==0)
		Command_AT_Tem[0]=Command_NUM[11];				//Command_AT_Tem[0]为-，表示温度小于0
		
		Command_AT_Tem[1]=Command_NUM[temperature/10];	//把十位的数字存到Command_AT_Tem[1]
		Command_AT_Tem[2]=Command_NUM[temperature%10];	//把个位的数字存到Command_AT_Tem[2]
	}
	
	Command_Smoke1[3]=Command_NUM[Smoke/10];	//把十位的数字存到Command_Smoke1[1]
	Command_Smoke1[4]=Command_NUM[Smoke%10];	//把个位的数字存到Command_Smoke1[2]
	
	Send_Message();							  //发送到指定号码
	delay2(50000);							  //等待回复输入符号
	Send_Command(Command_AT_Tem,4);		      //发送温度
	delay2(5000);
	Send_Command(Command_Smoke1,6);		      //发送浓度
	delay2(5000);
	Send_Command(Command_AT_End,1);			  //发送短信结束符
	delay2(1000);
}
void Read_Message(void)
{
	if(NEW_SMS==1)														//有新消息
	{
		if((UART0_Data[13]>=0x30)&&(UART0_Data[13]<=0x39))				//短信数第二位有数据，表示有二位数或三位数					
		{
			if((UART0_Data[14]>=0x30)&&(UART0_Data[14]<=0x39))			//三位数 暂不处理
			{

			}
			if((UART0_Data[14]<0x30)||(UART0_Data[14]>0x39))														//二位数 
			{		
				Command_AT_CMGR[8]=UART0_Data[12];					//第一位为短信数十位
				delay2(1000);
				Command_AT_CMGR[9]=UART0_Data[13];					//第二位为短信数个位
				delay2(1000);
				Command_AT_CMGD[8]=UART0_Data[12];					//第一位为短信数十位
				delay2(1000);
				Command_AT_CMGD[9]=UART0_Data[13];					//第一位为短信数个位
				delay2(1000);
			}
		}
		if((UART0_Data[13]<0x30)||(UART0_Data[13]>0x39))				//短信数第二位无数据，表示只有一位数
		{
			Command_AT_CMGR[8]=0x30;									//第一位为0
			delay2(1000);
			Command_AT_CMGR[9]=UART0_Data[12];						//第二位为短信数
			delay2(1000);
			Command_AT_CMGD[8]=0x30;									//第一位为0
			delay2(1000);
			Command_AT_CMGD[9]=UART0_Data[12];						//第二位为短信数
			delay2(1000);
		}		  
		Send_Command(Command_AT_CMGR,11);					//发送"AT+CMGR=?",读取第几条短信
		delay2(50000);delay2(50000);delay2(50000);			   							//延时
		
		
		if(((UART0_Data[6]==OPEN[0])&&(UART0_Data[7]==OPEN[1])&&(UART0_Data[8]==OPEN[2])&&(UART0_Data[9]==OPEN[3])&&(UART0_Data[10]==OPEN[4])&&(UART0_Data[11]==OPEN[5]))//移动
		  ||(UART0_Data[3]==OPEN[0])&&(UART0_Data[4]==OPEN[1])&&(UART0_Data[5]==OPEN[2])&&(UART0_Data[6]==OPEN[3])&&(UART0_Data[7]==OPEN[4])&&(UART0_Data[8]==OPEN[5]))//联通
		{	
			JDQ=1;								  	  //开启继电器
			Send_Command(Command_AT_CMGD,11);	  	  //删除短信
			delay2(10000);
			Send_Message();							  //发送到指定号码
			delay2(10000);							  //等待回复输入符号							  
			Send_Command(Command_Opened,13);		  //发送消息
			delay2(10000);
			Send_Command(Command_AT_End,1);			  //发送短信结束符
			delay2(20000);
		}
		if(((UART0_Data[6]==CLOSE[0])&&(UART0_Data[7]==CLOSE[1])&&(UART0_Data[8]==CLOSE[2])&&(UART0_Data[9]==CLOSE[3])&&(UART0_Data[10]==CLOSE[4])&&(UART0_Data[11]==CLOSE[5])&&(UART0_Data[12]==CLOSE[6]))//移动
		  ||(UART0_Data[3]==CLOSE[0])&&(UART0_Data[4]==CLOSE[1])&&(UART0_Data[5]==CLOSE[2])&&(UART0_Data[6]==CLOSE[3])&&(UART0_Data[7]==CLOSE[4])&&(UART0_Data[8]==CLOSE[5])&&(UART0_Data[9]==CLOSE[6]))//联通
		{
			JDQ=0;								  	  //关闭继电器
			Send_Command(Command_AT_CMGD,11);	  	  //删除短信
			delay2(10000);
			Send_Message();							  //发送到指定号码
			delay2(10000);							  //等待回复输入符号
			Send_Command(Command_Closed,13);		  //发送消息
			delay2(10000);
			Send_Command(Command_AT_End,1);			  //发送短信结束符
			delay2(20000);
		}
		UART0_Data[13]=0x40;UART0_Data[14]=0x40;
		UART0_Len=0;
		NEW_SMS=0;											//清除新消息提示标志

	}														   

	if(ATH==1)												//电话打入直接挂机
	{
		Send_Command(Command_ATH,4);						//发送挂机命令
		delay2(50000);
		Send_Message4();								    //发送当前温度
		ATH=0;
		UART0_Len=0;										//标志
	}
}


void NVIC_Configuration(void)//配置中断优先级  
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
		
		if(UART0_Data[UART0_Len]==0x2B)                      	//+号
		UART0_Len=0;			

		if((UART0_Data[1]==0x43)&&(UART0_Data[2]==0x4C)&&(UART0_Data[3]==0x49)&&(UART0_Data[4]==0x50)&&(UART0_Data[5]==0x3A))
		ATH=1;			//挂机标志位置一						//接收到的内容是电话提示UART0_Data【】="+CLIP:"，比较前6位
		
		if((UART0_Data[1]==0x43)&&(UART0_Data[2]==0x4D)&&(UART0_Data[3]==0x54)&&(UART0_Data[4]==0x49))
		NEW_SMS=1;		//短信标志位置一						//接收到的内容是短信提示UART0_Data【】="+CMTI",比较前五位		
			
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

	

