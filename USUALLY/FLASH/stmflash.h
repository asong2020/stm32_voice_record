#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "sys.h"  

#include "stm32f10x.h"
#include <stdbool.h> 

#define SSID_ADDRESS		0x0807F800 //�˺ű����ַ	 							��ַ������2�ı���
#define PSWD_ADDRESS		0x0807F8A0 //���뱣���ַ	����ƫ��160�ֽ�(80������)	��ַ������2�ı���
//�û������Լ�����Ҫ����
#define STM32_FLASH_SIZE 512 	 		//��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN 1              //ʹ��FLASHд��(0��������;1��ʹ��)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ
/* ȫ�ֺ��� */

bool flash_pro_page_text(const uint8_t *buffer,uint32_t address,uint32_t length);
bool flash_pro_page_text_else(const uint8_t *buffer, uint32_t address, uint32_t length);
uint8_t write_to_flash(uint32_t addr, uint32_t *data, uint32_t datasize);
uint8_t erase_flash(uint32_t start_addr, uint16_t pages);
extern uint8_t flashEraseFlag;
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH������ֵ
 

u16 STMFLASH_ReadHalfWord(u32 faddr);		  //��������  
void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//ָ����ַ��ʼд��ָ�����ȵ�����
u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);						//ָ����ַ��ʼ��ȡָ����������
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����

//����д��
void Test_Write(u32 WriteAddr,u16 WriteData);		





_Bool Flash_NeedErase(void);

void Flash_Read(unsigned int addr, char *rBuf, unsigned short len);

void Flash_Write(unsigned int addr, char *wBuf, unsigned short len);
#endif

















