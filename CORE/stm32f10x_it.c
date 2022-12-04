/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "bsp_usart1.h"
#include "stmflash.h"
#include "bsp_adc.h"
#include "bsp_dac.h"
extern uint32_t addressFlash;
extern uint8_t VoicePlay;
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}
uint8_t USART2_RXdata=0;
//usart2 接收
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)
	{
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
		USART_ReceiveData(USART2);
    USART2_RXdata=1;

	}
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}
extern uint8_t VoiceMFlag;
//定时器5中断服务函数
void TIM5_IRQHandler(void)
{
	static uint16_t VoiceCnt1=0;
	static uint16_t VoiceCnt2=0;
	static uint16_t VoiceCnt3=0;
	static uint16_t VoiceCnt4=0;
	if(TIM_GetITStatus(TIM5,TIM_IT_Update)==SET) //溢出中断
	{
		
//		if(VoiceMFlag==1)
//		{
//		   VoiceCnt1++;
//			 if(VoiceCnt1>1000)
//			 {
//				 VoiceCnt1=0;
//			   VoiceCnt2++;
//				 if(VoiceCnt2>10)
//				 {
//					 VoiceCnt2=0;
//				   VoiceMFlag=0;
//					 TIM_Cmd(TIM3, DISABLE);  //开启定时器3
//	         ADC_Cmd(ADC1, DISABLE);   //开启ADC1
//		       DMA_Cmd(DMA1_Channel1, DISABLE);	//DMA1的通道1 
//           printf("录音总时长为10S,已经结束可以播放录音\r\n");				 
//				 }
//			 
//			 }
//		
//		}
//		if(VoicePlay==1)
//		{
//		   VoiceCnt3++;
//			if(VoiceCnt3>1000)
//			{
//			  VoiceCnt3=0;
//				VoiceCnt4++;
//				if(VoiceCnt4>10)
//				{
//				   VoiceCnt4=0;
//					 VoicePlay=0;
//					 TIM_Cmd(TIM2, DISABLE);  //开启定时器3
//					 DAC_Cmd(DAC_Channel_1,DISABLE);
//					printf("播放总长时间为10s,已结束播放\r\n");
//				}
//			}
//		
//		
//		
//		}

	}

	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);  //清除中断标志位
}

//uint32_t dataRead;
////定时器2中断
//void TIM2_IRQHandler(void)
//{
//	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET) //溢出中断
//	{

//     if(VoicePlay==1)
//		 {
//			 Flash_Read(addressreadFlash,(char *)dataRead,4);
//			 DAC1_Out(dataRead);
////		   DAC_SetChannel1Data(DAC_Align_8b_R,dataRead[0]);
////			 DAC_Cmd(DAC_Channel_1,ENABLE);
//			 addressreadFlash +=4;
//				printf("读取Flash中数据\r\n");
//		 }
//			



//	}
//	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除中断标志位
//}

////定时器3中断
//void TIM3_IRQHandler(void)
//{
//	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
//	{


//	}
//	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
//}

//DMA 传输中断
void DMA1_Channel1_IRQHandler(void) 
{


	if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)
	{
		
		printf("音频数据采集完毕\r\n");
		TIM_Cmd(TIM3, DISABLE);  //开启定时器3
//		ADC_Cmd(ADC1, DISABLE);   //开启ADC1
//	  DMA_Cmd(DMA1_Channel1, DISABLE);	//DMA1的通道1 
//        printf("数据采集0x%x\r\n",g_ADC_Value);
//        if(addressFlash<=0x0807FFFF)
//				{
//				   write_to_flash(addressFlash,(uint32_t *)g_ADC_Value,4);
//				   addressFlash +=4;
//				
//				}
//						else	{
//					 TIM_Cmd(TIM3, DISABLE);  //开启定时器3
//	         ADC_Cmd(ADC1, DISABLE);   //开启ADC1
//		       DMA_Cmd(DMA1_Channel1, DISABLE);	//DMA1的通道1 
//			    printf("数据采集结束\r\n");
//			}
		
	}	
  
	DMA_ClearITPendingBit(DMA1_IT_TC1);


		
}

void DMA2_Channel3_IRQHandler(void)
{

  	if(DMA_GetITStatus(DMA2_IT_TC3) != RESET)
	{
		printf("音频数据播放完毕\r\n");
		
		TIM_Cmd(TIM2, DISABLE);	
//    DAC_Cmd(DAC_Channel_1,DISABLE);		
	}
  DMA_ClearITPendingBit(DMA2_IT_TC3);
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
