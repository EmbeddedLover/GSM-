#include "I2C.h"

void Delay1(uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

void I2C_Init_Set(void)
{   
    unsigned char i=0;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); //使能GPIOB APB2外设时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE); 	//使能IIC时钟
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;					//SCL控制总线，SDA数据总线--》I2C1总线
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;           //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//最高输出2MHz
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	for(i=0;i<10;i++)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_6);
		Delay1(10);
		GPIO_ResetBits(GPIOB,GPIO_Pin_6);
		Delay1(10);	
	}   
	
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;		//SCL SDA I2C1
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_OD;           //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//最高输出2MHz
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
	
	I2C_DeInit(I2C1);                                       //I2C1复位
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C ;        	   	//设置 I2C 为 I2C 模式
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;      //I2C 快速模式 Tlow / Thigh = 2 
	I2C_InitStructure.I2C_OwnAddress1 = 0xD0;    			//第一个设备自身地址
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;             //使能应答
	I2C_InitStructure.I2C_AcknowledgedAddress = 
	I2C_AcknowledgedAddress_7bit;                           //应答 7 位地址
	I2C_InitStructure.I2C_ClockSpeed = 50000;              //设置时钟频率，这个值不能高于 400KHz。
	I2C_Cmd(I2C1, ENABLE);                                  //使能I2C1
	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_AcknowledgeConfig(I2C1,ENABLE);						//使能应答功能		
}

//**********************************************************************************
//EEPROM写单个字节
//**********************************************************************************
void I2C_EE_ByteWrite(u8 pBuffer, u8 WriteAddr)                                 //从设备EEPROM_24CXX的地址  WriteAddr  写入 pBuffer
{
  I2C_GenerateSTART(I2C1, ENABLE);
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));			        //产生起始条件
 
  I2C_Send7bitAddress(I2C1, EEPROM_24CXX_WRITE, I2C_Direction_Transmitter);     //设置成发送状态
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
      
  I2C_SendData(I2C1, WriteAddr);                                                //写入对应地址
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
 
  I2C_SendData(I2C1, pBuffer);                                                  //写入数据
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  I2C_GenerateSTOP(I2C1, ENABLE);                                               //产生结束信号
}

//**********************************************************************************
//EEPROM读单个字节
//**********************************************************************************
unsigned char I2C_EE_BufferRead(u8 ReadAddr)                                    //从设备EEPROM_24CXX的地址  ReadAddr  读取数据返回到 pBuffer
{   
	u8 pBuffer;
	u16 NumByteToRead=1;

	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));    							//等待I2C

	I2C_GenerateSTART(I2C1, ENABLE);                                            //产生起始条件
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C1, EEPROM_24CXX_WRITE, I2C_Direction_Transmitter);   //设置成发送状态
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_Cmd(I2C1, ENABLE);                                                      //重新设定
	I2C_SendData(I2C1, ReadAddr);                                               //读取地址
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_GenerateSTART(I2C1, ENABLE);                                            //产生起始条件
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C1, EEPROM_24CXX_READ, I2C_Direction_Receiver);       //设置成读取状态
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	while(NumByteToRead)  
	{
		I2C_AcknowledgeConfig(I2C1, DISABLE);                                   //关闭应答
		I2C_GenerateSTOP(I2C1, ENABLE);                                         //产生结束信号
		if(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))  
		{      
			pBuffer = I2C_ReceiveData(I2C1);                                    //接收数据
			NumByteToRead--;        
		}   
	}

	I2C_AcknowledgeConfig(I2C1, ENABLE);                                        //重新开启应答

	return pBuffer;
}

