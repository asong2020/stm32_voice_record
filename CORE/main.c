
#include "stm32f10x.h"
#include <stdio.h>
#include "nvic.h"
#include "bsp_adc.h"
#include "bsp_dac.h"
#include "bsp_usart1.h"
#include "bsp_key.h" 
#include "delay.h"
#include "bsp_timer.h"
#include "stmflash.h"
extern uint8_t USART2_RXdata;
uint8_t VoiceMFlag=0;
uint8_t VoicePlay=0;
int main(void)
{
	delay_init();
	USART1_Config();
  nvic_cfg();
	ADC1_Init();
	DAC1_Init();
	Key_GPIO_Config();
	printf("初始化完毕\r\n");
  /* Infinite loop */
  while (1)
  {

	if(KEY1==0x01)
	{
	   delay_ms(20);
		if(KEY1==0x01)
		{
		
		while (KEY1 == 0x01 )
				; 

					printf("按键1已被按下\r\n");					
          VoiceMFlag=1;

		}
	}
		if(KEY2==0x01)
	{
	   delay_ms(20);
		if(KEY2==0x01)
		{
		
		while (KEY2 == 0x01 )
				; 
	
					printf("按键2已被按下\r\n");	
    		VoicePlay=1;

		}
	}
	 
	if(VoicePlay==1)
	{
	        VoicePlay=0;
	         TIM_Cmd(TIM2, ENABLE);	
		      printf("开始播放Flash中数据\r\n");
	}
	
	if(VoiceMFlag==1)
	{
	    VoiceMFlag=0;
//		  erase_flash(addressFlash,79);
//		  printf("Flash擦出成功\r\n");
		  TIM_Cmd(TIM3, ENABLE);  //开启定时器3
		  printf("开始录音\r\n");
	}
		
  }
}

