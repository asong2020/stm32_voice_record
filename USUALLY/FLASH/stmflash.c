#include "stmflash.h"
#include "delay.h"
#include "bsp_usart1.h"
 
 #define STM_FLASH_PAGE_SIZE (2*1024)
 /* 写Flash的相关变量 Save Parameter Write Flash */

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
__IO FLASH_Status FLASHStatus;
__IO TestStatus MemoryProgramStatus;
uint8_t flashEraseFlag = 0 ;
uint8_t flashPageNumID;
 
//读取指定地址的半字(16位数据)
//faddr:读地址(此地址必须为2的倍数!!)
//返回值:对应数据.
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}
#if STM32_FLASH_WREN	//如果使能了写   
//不检查的写入
//WriteAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数   
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
	for(i=0;i<NumToWrite;i++)
	{
		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//地址增加2.
	}  
} 
//从指定地址开始写入指定长度的数据
//WriteAddr:起始地址(此地址必须为2的倍数!!)
//pBuffer:数据指针
//NumToWrite:半字(16位)数(就是要写入的16位数据的个数.)
#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //字节
#else 
#define STM_SECTOR_SIZE	2048
#endif		 
u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];//最多是2K字节
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
{
	u32 secpos;	   //扇区地址
	u16 secoff;	   //扇区内偏移地址(16位字计算)
	u16 secremain; //扇区内剩余地址(16位字计算)	   
 	u16 i;    
	u32 offaddr;   //去掉0X08000000后的地址
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return;//非法地址
	FLASH_Unlock();						//解锁
	offaddr=WriteAddr-STM32_FLASH_BASE;		//实际偏移地址.
	secpos=offaddr/STM_SECTOR_SIZE;			//扇区地址  0~127 for STM32F103RBT6
	secoff=(offaddr%STM_SECTOR_SIZE)/2;		//在扇区内的偏移(2个字节为基本单位.)
	secremain=STM_SECTOR_SIZE/2-secoff;		//扇区剩余空间大小   
	if(NumToWrite<=secremain)secremain=NumToWrite;//不大于该扇区范围
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//擦除这个扇区
			for(i=0;i<secremain;i++)//复制
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//写入整个扇区  
		}else STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumToWrite==secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;				//扇区地址增1
			secoff=0;				//偏移位置为0 	 
		   	pBuffer+=secremain;  	//指针偏移
			WriteAddr+=secremain;	//写地址偏移	   
		   	NumToWrite-=secremain;	//字节(16位)数递减
			if(NumToWrite>(STM_SECTOR_SIZE/2))secremain=STM_SECTOR_SIZE/2;//下一个扇区还是写不完
			else secremain=NumToWrite;//下一个扇区可以写完了
		}	 
	};	
	FLASH_Lock();//上锁
}
#endif

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//读取2个字节.
		ReadAddr+=2;//偏移2个字节.	
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//WriteAddr:起始地址
//WriteData:要写入的数据
void Test_Write(u32 WriteAddr,u16 WriteData)   	
{
	STMFLASH_Write(WriteAddr,&WriteData,1);//写入一个字 
}


/******************************************************************************
 * 		Function name		： flash_pro_page_text
 *    Function describe     ： 用于更新flash的函数。
 * 			Input		    ： *buffer ：要写入数据的指针
 								address：flash的地址
								length ：要写入的数据的长度
 * 			Output		    ： true or 
 * 		 Illustration	    ：  
 *			Revise			：
 *		 Existing problems 	:  No
 *****************************************************************************/

bool flash_pro_page_text(const uint8_t *buffer, uint32_t address, uint32_t length)
{
	uint32_t i , j;
	uint32_t temp;
	uint32_t addr;
  	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED;
    /* 解锁Flash */
  	FLASH_Unlock();

  	/* 清Flash标志 */
  	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	

  	/* 擦除页 */
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
 * 		Function name		： flash_pro_page_text_else
 *    Function describe     ： 用于更新flash的函数。
 * 			Input		    ： *buffer ：要写入数据的指针
 								address：flash的地址
								length ：要写入的数据的长度
 * 			Output		    ： true or false
 * 		 Illustration	    ：  
 *			Revise			：
 *		 Existing problems 	:  No
 *****************************************************************************/
bool flash_pro_page_text_else(const uint8_t *buffer, uint32_t address, uint32_t length)
{
	uint32_t i;
	uint32_t temp;
	uint32_t addr;
  	FLASHStatus = FLASH_COMPLETE;
	MemoryProgramStatus = PASSED;
    /* 解锁Flash */
  	FLASH_Unlock();

  	/* 清Flash标志 */
  	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	

  	/* 擦除页 */
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
 * 		Function name		： write_to_flash
 *    Function describe     ： 用于写入flash的函数。
 * 			Input		    ：  addr   ：写入flash的地址
 							   *data   ：要写入数据的指针
							  datasize ：要写入的数据的长度
 * 			Output		    ： true or false
 * 		 Illustration	    ：  
 *			Revise			：
 *		 Existing problems 	:  No
 *****************************************************************************/
uint8_t write_to_flash(uint32_t addr, uint32_t *data, uint32_t datasize)
{
    int i = 0;
    FLASH_Status FLASHStatus = FLASH_COMPLETE;        //flash解锁
    FLASH_Unlock();    
    for(i = 0; i < datasize; i++)    
    {        
        //将数据写入Flash        
        FLASHStatus = FLASH_ProgramWord(addr, data[i]);        
        if(FLASHStatus != FLASH_COMPLETE)            
            return 1;                
        addr += 4; 
    }            
    
    //FLASH加锁    
    FLASH_Lock();     
    return 0;
}
/******************************************************************************
 * 		Function name		： erase_flash
 *    Function describe     ： 用于擦除flash的函数。
 * 			Input		    ：  addr   ：要擦除flash的地址
 							   pages   ：要擦除flash的页数
 * 			Output		    ： true or false
 * 		 Illustration	    ：  
 *			Revise			：
 *		 Existing problems 	:  No
 *****************************************************************************/
uint8_t erase_flash(uint32_t start_addr, uint16_t pages)
{    
    uint16_t i;    
    FLASH_Status FLASHStatus = FLASH_COMPLETE;   
         
    //flash解锁    
    FLASH_Unlock();        
    for(i = 0; (i < pages && FLASHStatus == FLASH_COMPLETE); i++)
    { 
        //擦除页，传入一页中任意地址将擦除该页     
        FLASHStatus = FLASH_ErasePage(start_addr + STM_FLASH_PAGE_SIZE*i); 
        while(FLASH_WaitForLastOperation(1000) != FLASH_COMPLETE);    
    } 
    
    //FLASH加锁    
    FLASH_Lock();        
    if(FLASH_COMPLETE == FLASHStatus)        
        return 0;    
    else         
        return 1;
}




//读很简单，吧地址作为指针去读取里边的内容即可，实际上与不涉及到flash的保护的操作，仅仅是取地址里边的值而已
void Flash_Read(unsigned int addr, char *rBuf, unsigned short len)
{

	unsigned short lenCount = 0;
	unsigned char charBuf = 0;
	
	for(; lenCount < len; lenCount++)
	{
		charBuf = *(volatile unsigned char *)addr; //转为uchar类型
		if(charBuf == 0xff) //如果读到了0xFF，则代表数据读完了，因为各个人的ssid和password长度不同
			break;
		
		rBuf[lenCount] = (char)charBuf; //转换一下，赋值
		
		addr += 2; //传入的地址必须是2的倍数
	}

}

//判断是否需要擦除
_Bool Flash_NeedErase(void) //1-需要擦除(有数据)		0-不需要擦除(无数据)
{

	unsigned short rCount = 0;
	
	unsigned int addr = SSID_ADDRESS; //页起始地址
	
	for(; rCount < 1024; rCount++) //读出2KB数据		读一次至少是半字，所以这里是1024
	{
		if(*(volatile unsigned short *)addr != 0xffff)
			return 1;
		
		addr += 2;
	}
	
	return 0;

}

//写比较复杂点，注意三点，1.解锁。2.只有地址里边的值为0xFFFF时能够被写入其他数据，所以重写ssid和pswd前需要擦除一次。3.写完上锁。
void Flash_Write(unsigned int addr, char *wBuf, unsigned short len)
{
	
	unsigned short lenCount = 0;
	
	FLASH_Unlock();	//解锁

	//之前在这里sb了，我是分两次写入ssid和pswd的，结果没注释这里，ssid被擦除了，调试了大半天。。。我去
//	if(Flash_NeedErase()) //需要擦除
//	{
//		FLASH_ErasePage(SSID_ADDRESS);
//	}
	
	for(; lenCount < len; lenCount++)
	{
		//FLASH_ProgramOptionByteData(addr, wBuf[lenCount]); //写入 //这种方式写入我没调出来，反正本质上都是写半字，索性用下面的方式
		FLASH_ProgramHalfWord(addr, (unsigned short)wBuf[lenCount]); //写入

		addr += 2; //地址必须是2的倍数
	}
	
	FLASH_Lock(); //上锁

}










