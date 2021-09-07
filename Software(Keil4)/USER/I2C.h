#ifndef __I2C_H__
#define __I2C_H__

#include "stm32f10x.h"
#include "stm32f10x_i2c.h"

#define EEPROM_24CXX_WRITE  	 0xA0			  	//EEPROM器件写地址   24c02
#define EEPROM_24CXX_READ   	 0xA1			  	//EEPROM器件读地址   24c02
#define EEPROM_ADDRESS1  0x0001			  			//设置温度存储地址
#define EEPROM_ADDRESS2  0x0002			  			//设置烟雾浓度存储地址

#define EEPROM_Phone1    0x0003			  			//手机号码存储地址
#define EEPROM_Phone2    0x0004			  			//手机号码存储地址
#define EEPROM_Phone3    0x0005			  			//手机号码存储地址
#define EEPROM_Phone4    0x0006			  			//手机号码存储地址
#define EEPROM_Phone5    0x0007			  			//手机号码存储地址
#define EEPROM_Phone6    0x0008			  			//手机号码存储地址
#define EEPROM_Phone7    0x0009			  			//手机号码存储地址
#define EEPROM_Phone8    0x0010			  			//手机号码存储地址
#define EEPROM_Phone9    0x0011			  			//手机号码存储地址
#define EEPROM_Phone10   0x0012			  			//手机号码存储地址
#define EEPROM_Phone11   0x0013			  			//手机号码存储地址

extern void I2C_Init_Set(void);
extern void EEPROM_write_byte(unsigned int id,unsigned char write_address,unsigned char byte);
extern unsigned char EEPROM_read_byte(unsigned int id,unsigned char read_address);
extern unsigned char I2C_EE_BufferRead(u8 ReadAddr);
extern void I2C_EE_ByteWrite(u8 pBuffer, u8 WriteAddr);

#endif

