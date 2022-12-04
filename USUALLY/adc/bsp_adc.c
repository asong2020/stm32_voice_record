

#include "bsp_adc.h"

#define SAMPLE_FREQ (8000)		//����Ƶ��8kHz
#define ADC1_DR_Address    ((uint32_t)0x40012400+0x4c)

uint32_t addressFlash=0x08058800;
uint32_t addressreadFlash=0x08058800;
//__IO uint16_t ADC_ConvertedValue;
//__IO u16 ADC_ConvertedValueLocal;

int16_t g_ADC_Value[VcBuf_Len];

/**
  * @brief  ʹ��ADC1��DMA1��ʱ�ӣ���ʼ��PC.01
  * @param  ��
  * @retval ��
  */
static void ADC1_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;	//GPIO��ʼ���ṹ
	  ADC_InitTypeDef ADC_InitStructure;		//ADC1��ʼ���ṹ
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    //��GPIO��ADC1��ʱ��
  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA , ENABLE);
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	

    //���ö�ӦGPIO�ڣ�����ʹ����PA0��ADC1   ͨ��0
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	//
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);				// PA0,����ʱ������������
	
		/* TIME1 configuration */
		TIM_DeInit(TIM3);
		TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
		/* TIM3 used for timing, the timing period depends on the sample rate */
		TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;    /* TIM3CLK = 72 MHz */
		TIM_TimeBaseStructure.TIM_Period = 124;             //��Ӧ8K����
		TIM_TimeBaseStructure.TIM_ClockDivision = 0x00;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_SelectOutputTrigger(TIM3,TIM_TRGOSource_Update); //ѡ��TRGO����ԴΪ��ʱ������ʱ��
//		TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,���������
		TIM_Cmd(TIM3, DISABLE);

	  //����ADC1�Ĺ���ģʽ
	  ADC_DeInit(ADC1);		//����ʼ��
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;			//����ADCģʽ
    ADC_InitStructure.ADC_ScanConvMode = DISABLE ; 	 				//����ɨ��ģʽ��ɨ��ģʽ���ڶ�ͨ���ɼ�
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;			//��������ת��ģʽ������ͣ�ؽ���ADCת��
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;	//��ʹ���ⲿ����ת��
	  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	//�ɼ������Ҷ���
    ADC_InitStructure.ADC_NbrOfChannel = 1;	 								//Ҫת����ͨ����Ŀ1
    ADC_Init(ADC1, &ADC_InitStructure);
	
//			/*����ADCʱ�ӣ�ΪPCLK2��6��Ƶ����12MHz*/
	  RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
// 	//����ADC1��ת��˳��Ͳ�������
 	  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_1Cycles5);	
	
 	  ADC_Cmd(ADC1, ENABLE);		//ʹ��ADC1
	
	/*��λУ׼�Ĵ��� */
	  ADC_ResetCalibration(ADC1);		
    while(ADC_GetResetCalibrationStatus(ADC1));/*�ȴ�У׼�Ĵ�����λ��� */

	/* ADCУ׼ */
    ADC_StartCalibration(ADC1);		
    while(ADC_GetCalibrationStatus(ADC1));/* �ȴ�У׼���*/
	
	//��ADC�ж�
	//ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

	//����ADC�ⲿ����ģʽ
	 ADC_ExternalTrigConvCmd(ADC1, ENABLE);
	//ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/**
  * @brief  ����ADC1��DMA����
  * @param  ��
  * @retval ��
  * ADC1 in Channel1 of DMA1
  */
static void ADC1_DMA1_Config()
{
	DMA_InitTypeDef DMA_InitStructure;
	
	 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	/* DMA channel1 ���� */
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	 			//ADC��ַ
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&(g_ADC_Value);	//�ڴ��ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = VcBuf_Len;			//Ҫ��������ݸ���
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//�����ַ�̶�
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  				//�ڴ��ַ����
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//����
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;										//ѭ������
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    /* Enable DMA channel1 */
    DMA_Cmd(DMA1_Channel1, ENABLE);	//DMA1��ͨ��1
		DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
		ADC_DMACmd(ADC1,ENABLE);
}

/**
  * @brief  ADC1��ʼ��
  * @param  ��
  * @retval ��
  */
void ADC1_Init(void)
{
    ADC1_Config();	//����ADC
    ADC1_DMA1_Config();	//����DMA
}

/**
  * @brief  ��ȡADCֵ
  * @param  ��
  * @retval ��
  */
u8 Get_Adc(void)   
{
	u16 temp = 0;
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

 	temp = ADC_GetConversionValue(ADC1);// - (u16)2.05/3.3*4096;	//ȥ��ƫ��
//  	temp = temp*2;	//�Ŵ��ı�
//  	temp = temp + 2048;		//����3.3/2Vƫ��
	
	return (u8)(temp>>4);	//�������һ��ADC1�������ת�����
}
/*********************************************END OF FILE**********************/
