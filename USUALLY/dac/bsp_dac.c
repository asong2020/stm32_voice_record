/**
  ******************************************************************************
  * @file    bsp_xxx.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   adc1 Ӧ��bsp / DMA ģʽ
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

#include "bsp_dac.h"
#include "bsp_adc.h"
//#define DataLength (80000)
#define DAC_DHR12RD_Address     0x40007420

//__IO uint16_t DAC_PreOutputValue;

extern uint16_t SampleFreq;
extern uint32_t DataLength;
extern uint32_t DataAddress;
DMA_InitTypeDef  DMA_InitStructure;
 /*
 * ��������DAC1_GPIO_Config
 * ����  ��ʹ��DAC1��DMA1��ʱ�ӣ���ʼ��PA.04
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
 */
static void DAC1_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	DAC_InitTypeDef DAC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	/* ʹ�� GPIOA �� DAC ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC|RCC_APB1Periph_TIM2, ENABLE);//ע��DAC clock is int APB1
	
	/* ����PA.04Ϊģ����� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;	//GPIO_Pin_4|,//PA4��PA5��DAC1,DAC2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);			
		/* TIME1 configuration */
		TIM_DeInit(TIM2);
		TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
		/* TIM3 used for timing, the timing period depends on the sample rate */
		TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;    /* TIM3CLK = 72 MHz */
		TIM_TimeBaseStructure.TIM_Period = 124;             //��Ӧ8K����
		TIM_TimeBaseStructure.TIM_ClockDivision = 0x00;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_SelectOutputTrigger(TIM2,TIM_TRGOSource_Update); //ѡ��TRGO����ԴΪ��ʱ������ʱ��
//		TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,���������
		TIM_Cmd(TIM2, DISABLE);

	/* ����DAC1 */
	DAC_DeInit();
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;	// DAC_Trigger_T2_TRGO
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None ; 	 //������
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;		//ʹ����������򿪾�û�����
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);

	DAC_Cmd(DAC_Channel_1,ENABLE);
	DAC_SetChannel1Data(DAC_Align_12b_R, 0);  //12λ�Ҷ������ݸ�ʽ����DACֵ
}

 /* ��������ADC1_Mode_Config
 * ����  ������ADC1�Ĺ���ģʽΪMDAģʽ
 * ����  : ��
 * ���  ����
 * ����  ���ڲ�����
 * DAC1 in Channel3 of DMA2
 */
static void DAC1_DMA2_Config(void)
{
	DMA_InitTypeDef  DMA_InitStructure;

	/* ʹ��DMA2ʱ�� */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	
	/* ����DMA2 */
	DMA_DeInit(DMA2_Channel3);		//����ʼ��
	DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12RD_Address;					//�������ݵ�ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)g_ADC_Value;			//�ڴ����ݵ�ַ DualSine12bit
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;								//���ݴ��䷽���ڴ�������
	DMA_InitStructure.DMA_BufferSize = VcBuf_Len;		//���ݸ���											//�����СΪ32�ֽ�	
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				//�������ݵ�ַ�̶�	
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;							//�ڴ����ݵ�ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;			//������������Ϊ��λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;					//�ڴ���������Ϊ��λ	
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;				//DMA_Mode_Circular;//ѭ��ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;								//��DMAͨ�����ȼ�
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;									//���ڴ����ڴ�ģʽ
	DMA_Init(DMA2_Channel3, &DMA_InitStructure);

	/* ʹ��DMA2 3ͨ�� */
	DMA_Cmd(DMA2_Channel3, ENABLE);  
	DMA_ITConfig(DMA2_Channel3, DMA_IT_TC, ENABLE);
	DAC_DMACmd(DAC_Channel_1, ENABLE);	//ʹ��DMA����
}



/*
 * ��������DAC1_Init
 * ����  ����
 * ����  ����
 * ���  ����
 * ����  ���ⲿ����
 */
void DAC1_Init(void)
{
	DAC1_Config();
	DAC1_DMA2_Config();
}

/**
  * @brief  DAC1�Զ������/��
  * @param  ��
  * @retval ��
  */
void DAC1_Cmd(FunctionalState status)
{
	if (status)
	{
		TIM_Cmd(TIM2, ENABLE);	//�򿪶�ʱ��
	}
	else
	{
		TIM_Cmd(TIM2, DISABLE);	//�رն�ʱ��
	}
}

/**
  * @brief  �����DAC1����PA.04
  * @param  ��
  * @retval ��
  */
void DAC1_Out(uint16_t data)
{
	//DAC_SetChannel1Data(DAC_Align_8b_R, data);		//8λ�Ҷ���
	//DAC_Cmd(DAC_Channel_1,ENABLE);
	DAC_SetChannel1Data(DAC_Align_8b_R, data);		//8λ�Ҷ��룬��仰����Ҫ�У�����Ҳ����ST��BUG ~..~��ƫ�Ƶ�ַ֮����ֽ�������������Ŷ~~�п����
	DAC_Cmd(DAC_Channel_1,ENABLE);
}

void DAC1_Tran(uint32_t *Add)
{

	    DMA_Cmd(DMA2_Channel3, DISABLE);   
			DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Add;
			DMA_Init(DMA2_Channel3, &DMA_InitStructure);
			DMA_Cmd(DMA2_Channel3, ENABLE);		
}


/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/


























