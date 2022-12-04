#include "stmflash.h"
#include "delay.h"
#include "bsp_usart1.h"
 
 #define STM_FLASH_PAGE_SIZE (2*1024)
 /* дFlash����ر��� Save Parameter Write Flash */

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
__IO FLASH_Status FLASHStatus;
__IO TestStatus MemoryProgramStatus;
uint8_t flashEraseFlag = 0 ;
uint8_t flashPageNumID;
 
//��ȡָ����ַ�İ���(16λ����)
//faddr:����ַ(�˵�ַ����Ϊ2�ı���!!)
//����ֵ:��Ӧ����.
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}
#if STM32_FLASH_WREN	//���ʹ����д   
//������д��
//WriteAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��   
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
	for(i=0;i<NumToWrite;i++)
	{
		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//��ַ����2.
	}  
} 
//��ָ����ַ��ʼд��ָ�����ȵ�����
//WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ2�ı���!!)
//pBuffer:����ָ��
//NumToWrite:����(16λ)��(����Ҫд���16λ���ݵĸ���.)
#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //�ֽ�
#else 
#define STM_SECTOR_SIZE	2048
#endif		 
u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];//�����2K�ֽ�
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
{
	u32 secpos;	   //������ַ
	u16 secoff;	   //������ƫ�Ƶ�ַ(16λ�ּ���)
	u16 secremain; //������ʣ���ַ(16λ�ּ���)	   
 	u16 i;    
	u32 offaddr;   //ȥ��0X08000000��ĵ�ַ
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return;//�Ƿ���ַ
	FLASH_Unlock();						//����
	offaddr=WriteAddr-STM32_FLASH_BASE;		//ʵ��ƫ�Ƶ�ַ.
	secpos=offaddr/STM_SECTOR_SIZE;			//������ַ  0~127 for STM32F103RBT6
	secoff=(offaddr%STM_SECTOR_SIZE)/2;		//�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)
	secremain=STM_SECTOR_SIZE/2-secoff;		//����ʣ��ռ��С   
	if(NumToWrite<=secremain)secremain=NumToWrite;//�����ڸ�������Χ
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//�����������
			for(i=0;i<secremain;i++)//����
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//д����������  
		}else STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;				//������ַ��1
			secoff=0;				//ƫ��λ��Ϊ0 	 
		   	pBuffer+=secremain;  	//ָ��ƫ��
			WriteAddr+=secremain;	//д��ַƫ��	   
		   	NumToWrite-=secremain;	//�ֽ�(16λ)���ݼ�
			if(NumToWrite>(STM_SECTOR_SIZE/2))secremain=STM_SECTOR_SIZE/2;//��һ����������д����
			else secremain=NumToWrite;//��һ����������д����
		}	 
	};	
	FLASH_Lock();//����
}
#endif

//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//��ȡ2���ֽ�.
		ReadAddr+=2;//ƫ��2���ֽ�.	
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//WriteAddr:��ʼ��ַ
//WriteData:Ҫд�������
void Test_Write(u32 WriteAddr,u16 WriteData)   	
{
	STMFLASH_Write(WriteAddr,&WriteData,1);//д��һ���� 
}


/******************************************************************************
 * 		Function name		�� flash_pro_page_text
 *    Function describe     �� ���ڸ���flash�ĺ�����
 * 			Input		    �� *buffer ��Ҫд�����ݵ�ָ��
 								address��flash�ĵ�ַ
								length ��Ҫд������ݵĳ���
 * 			Output		    �� true or 
 * 		 Illustration	    ��  
 *			Revise			��
 *		 Existing problems 	:  No
 *****************************************************************************/

bool flash_pro_page_text(const uint8_t *buffer, uint32_t address, uint32_t length)
{
	uint32_t i , j;
	uint32_t temp;
	uint32_t addr;
  	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED;
    /* ����Flash */
  	FLASH_Unlock();

  	/* ��Flash��־ */
  	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	

  	/* ����ҳ */
	for(j = 0;j < flashPageNumID;j++)
	{
		FLASHStatus = FLASH_ErasePage(address+1024*j);
	}

  	if(FLASHStatus != FLASH_COMPLETE)return false;

  	addr = address;
	temp = 0;
	for(i = 0;i < length;)
	{
        temp = *(uint32_t*)(buffer+i);
        if(FLASH_ProgramWord(addr+i,temp)!=FLASH_COMPLETE) return false;
				i = i+4;
	}
	return true;
}
/******************************************************************************
 * 		Function name		�� flash_pro_page_text_else
 *    Function describe     �� ���ڸ���flash�ĺ�����
 * 			Input		    �� *buffer ��Ҫд�����ݵ�ָ��
 								address��flash�ĵ�ַ
								length ��Ҫд������ݵĳ���
 * 			Output		    �� true or false
 * 		 Illustration	    ��  
 *			Revise			��
 *		 Existing problems 	:  No
 *****************************************************************************/
bool flash_pro_page_text_else(const uint8_t *buffer, uint32_t address, uint32_t length)
{
	uint32_t i;
	uint32_t temp;
	uint32_t addr;
  	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED;
    /* ����Flash */
  	FLASH_Unlock();

  	/* ��Flash��־ */
  	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	

  	/* ����ҳ */
	FLASHStatus = FLASH_ErasePage(address);
  	if(FLASHStatus != FLASH_COMPLETE)return false;

  	addr = address;
	temp = 0;
	for(i = 0;i < length;)
	{
        temp = *(uint32_t*)(buffer+i);
        if(FLASH_ProgramWord(addr+i,temp)!=FLASH_COMPLETE) return false;
		i = i+4;
	}
	return true;
}
/******************************************************************************
 * 		Function name		�� write_to_flash
 *    Function describe     �� ����д��flash�ĺ�����
 * 			Input		    ��  addr   ��д��flash�ĵ�ַ
 							   *data   ��Ҫд�����ݵ�ָ��
							  datasize ��Ҫд������ݵĳ���
 * 			Output		    �� true or false
 * 		 Illustration	    ��  
 *			Revise			��
 *		 Existing problems 	:  No
 *****************************************************************************/
uint8_t write_to_flash(uint32_t addr, uint32_t *data, uint32_t datasize)
{
    int i = 0;
    FLASH_Status FLASHStatus = FLASH_COMPLETE;        //flash����
    FLASH_Unlock();    
    for(i = 0; i < datasize; i++)    
    {        
        //������д��Flash        
        FLASHStatus = FLASH_ProgramWord(addr, data[i]);        
        if(FLASHStatus != FLASH_COMPLETE)            
            return 1;                
        addr += 4; 
    }            
    
    //FLASH����    
    FLASH_Lock();     
    return 0;
}
/******************************************************************************
 * 		Function name		�� erase_flash
 *    Function describe     �� ���ڲ���flash�ĺ�����
 * 			Input		    ��  addr   ��Ҫ����flash�ĵ�ַ
 							   pages   ��Ҫ����flash��ҳ��
 * 			Output		    �� true or false
 * 		 Illustration	    ��  
 *			Revise			��
 *		 Existing problems 	:  No
 *****************************************************************************/
uint8_t erase_flash(uint32_t start_addr, uint16_t pages)
{    
    uint16_t i;    
    FLASH_Status FLASHStatus = FLASH_COMPLETE;   
         
    //flash����    
    FLASH_Unlock();        
    for(i = 0; (i < pages && FLASHStatus == FLASH_COMPLETE); i++)
    { 
        //����ҳ������һҳ�������ַ��������ҳ     
        FLASHStatus = FLASH_ErasePage(start_addr + STM_FLASH_PAGE_SIZE*i); 
        while(FLASH_WaitForLastOperation(1000) != FLASH_COMPLETE);    
    } 
    
    //FLASH����    
    FLASH_Lock();        
    if(FLASH_COMPLETE == FLASHStatus)        
        return 0;    
    else         
        return 1;
}




//���ܼ򵥣��ɵ�ַ��Ϊָ��ȥ��ȡ��ߵ����ݼ��ɣ�ʵ�����벻�漰��flash�ı����Ĳ�����������ȡ��ַ��ߵ�ֵ����
void Flash_Read(unsigned int addr, char *rBuf, unsigned short len)
{

	unsigned short lenCount = 0;
	unsigned char charBuf = 0;
	
	for(; lenCount < len; lenCount++)
	{
		charBuf = *(volatile unsigned char *)addr; //תΪuchar����
		if(charBuf == 0xff) //���������0xFF����������ݶ����ˣ���Ϊ�����˵�ssid��password���Ȳ�ͬ
			break;
		
		rBuf[lenCount] = (char)charBuf; //ת��һ�£���ֵ
		
		addr += 2; //����ĵ�ַ������2�ı���
	}

}

//�ж��Ƿ���Ҫ����
_Bool Flash_NeedErase(void) //1-��Ҫ����(������)		0-����Ҫ����(������)
{

	unsigned short rCount = 0;
	
	unsigned int addr = SSID_ADDRESS; //ҳ��ʼ��ַ
	
	for(; rCount < 1024; rCount++) //����2KB����		��һ�������ǰ��֣�����������1024
	{
		if(*(volatile unsigned short *)addr != 0xffff)
			return 1;
		
		addr += 2;
	}
	
	return 0;

}

//д�Ƚϸ��ӵ㣬ע�����㣬1.������2.ֻ�е�ַ��ߵ�ֵΪ0xFFFFʱ�ܹ���д���������ݣ�������дssid��pswdǰ��Ҫ����һ�Ρ�3.д��������
void Flash_Write(unsigned int addr, char *wBuf, unsigned short len)
{
	
	unsigned short lenCount = 0;
	
	FLASH_Unlock();	//����

	//֮ǰ������sb�ˣ����Ƿ�����д��ssid��pswd�ģ����ûע�����ssid�������ˣ������˴���졣������ȥ
//	if(Flash_NeedErase()) //��Ҫ����
//	{
//		FLASH_ErasePage(SSID_ADDRESS);
//	}
	
	for(; lenCount < len; lenCount++)
	{
		//FLASH_ProgramOptionByteData(addr, wBuf[lenCount]); //д�� //���ַ�ʽд����û�����������������϶���д���֣�����������ķ�ʽ
		FLASH_ProgramHalfWord(addr, (unsigned short)wBuf[lenCount]); //д��

		addr += 2; //��ַ������2�ı���
	}
	
	FLASH_Lock(); //����

}










