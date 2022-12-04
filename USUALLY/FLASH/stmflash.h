#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "sys.h"  

#include "stm32f10x.h"
#include <stdbool.h> 

#define SSID_ADDRESS		0x0807F800 //账号保存地址	 							地址必须是2的倍数
#define PSWD_ADDRESS		0x0807F8A0 //密码保存地址	长度偏移160字节(80个半字)	地址必须是2的倍数
//用户根据自己的需要设置
#define STM32_FLASH_SIZE 512 	 		//所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN 1              //使能FLASH写入(0，不是能;1，使能)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH起始地址
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址
/* 全局函数 */

bool flash_pro_page_text(const uint8_t *buffer,uint32_t address,uint32_t length);
bool flash_pro_page_text_else(const uint8_t *buffer, uint32_t address, uint32_t length);
uint8_t write_to_flash(uint32_t addr, uint32_t *data, uint32_t datasize);
uint8_t erase_flash(uint32_t start_addr, uint16_t pages);
extern uint8_t flashEraseFlag;
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH解锁键值
 

u16 STMFLASH_ReadHalfWord(u32 faddr);		  //读出半字  
void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//指定地址开始写入指定长度的数据
u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);						//指定地址开始读取指定长度数据
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//从指定地址开始读出指定长度的数据

//测试写入
void Test_Write(u32 WriteAddr,u16 WriteData);		





_Bool Flash_NeedErase(void);

void Flash_Read(unsigned int addr, char *rBuf, unsigned short len);

void Flash_Write(unsigned int addr, char *wBuf, unsigned short len);
#endif

















