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
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); //ʹ��GPIOB APB2����ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE); 	//ʹ��IICʱ��
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;					//SCL�������ߣ�SDA��������--��I2C1����
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;           //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//������2MHz
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	for(i=0;i<10;i++)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_6);
		Delay1(10);
		GPIO_ResetBits(GPIOB,GPIO_Pin_6);
		Delay1(10);	
	}   
	
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;		//SCL SDA I2C1
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_OD;           //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//������2MHz
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
	
	I2C_DeInit(I2C1);                                       //I2C1��λ
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C ;        	   	//���� I2C Ϊ I2C ģʽ
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;      //I2C ����ģʽ Tlow / Thigh = 2 
	I2C_InitStructure.I2C_OwnAddress1 = 0xD0;    			//��һ���豸�����ַ
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;             //ʹ��Ӧ��
	I2C_InitStructure.I2C_AcknowledgedAddress = 
	I2C_AcknowledgedAddress_7bit;                           //Ӧ�� 7 λ��ַ
	I2C_InitStructure.I2C_ClockSpeed = 50000;              //����ʱ��Ƶ�ʣ����ֵ���ܸ��� 400KHz��
	I2C_Cmd(I2C1, ENABLE);                                  //ʹ��I2C1
	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_AcknowledgeConfig(I2C1,ENABLE);						//ʹ��Ӧ����		
}

//**********************************************************************************
//EEPROMд�����ֽ�
//**********************************************************************************
void I2C_EE_ByteWrite(u8 pBuffer, u8 WriteAddr)                                 //���豸EEPROM_24CXX�ĵ�ַ  WriteAddr  д�� pBuffer
{
  I2C_GenerateSTART(I2C1, ENABLE);
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));			        //������ʼ����
 
  I2C_Send7bitAddress(I2C1, EEPROM_24CXX_WRITE, I2C_Direction_Transmitter);     //���óɷ���״̬
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
      
  I2C_SendData(I2C1, WriteAddr);                                                //д���Ӧ��ַ
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
 
  I2C_SendData(I2C1, pBuffer);                                                  //д������
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
  I2C_GenerateSTOP(I2C1, ENABLE);                                               //���������ź�
}

//**********************************************************************************
//EEPROM�������ֽ�
//**********************************************************************************
unsigned char I2C_EE_BufferRead(u8 ReadAddr)                                    //���豸EEPROM_24CXX�ĵ�ַ  ReadAddr  ��ȡ���ݷ��ص� pBuffer
{   
	u8 pBuffer;
	u16 NumByteToRead=1;

	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));    							//�ȴ�I2C

	I2C_GenerateSTART(I2C1, ENABLE);                                            //������ʼ����
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C1, EEPROM_24CXX_WRITE, I2C_Direction_Transmitter);   //���óɷ���״̬
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_Cmd(I2C1, ENABLE);                                                      //�����趨
	I2C_SendData(I2C1, ReadAddr);                                               //��ȡ��ַ
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_GenerateSTART(I2C1, ENABLE);                                            //������ʼ����
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2C1, EEPROM_24CXX_READ, I2C_Direction_Receiver);       //���óɶ�ȡ״̬
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	while(NumByteToRead)  
	{
		I2C_AcknowledgeConfig(I2C1, DISABLE);                                   //�ر�Ӧ��
		I2C_GenerateSTOP(I2C1, ENABLE);                                         //���������ź�
		if(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))  
		{      
			pBuffer = I2C_ReceiveData(I2C1);                                    //��������
			NumByteToRead--;        
		}   
	}

	I2C_AcknowledgeConfig(I2C1, ENABLE);                                        //���¿���Ӧ��

	return pBuffer;
}

