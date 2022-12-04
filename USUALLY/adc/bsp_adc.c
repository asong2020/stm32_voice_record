

#include "bsp_adc.h"

#define SAMPLE_FREQ (8000)		//采样频率8kHz
#define ADC1_DR_Address    ((uint32_t)0x40012400+0x4c)

uint32_t addressFlash=0x08058800;
uint32_t addressreadFlash=0x08058800;
//__IO uint16_t ADC_ConvertedValue;
//__IO u16 ADC_ConvertedValueLocal;

int16_t g_ADC_Value[VcBuf_Len];

/**
  * @brief  使能ADC1和DMA1的时钟，初始化PC.01
  * @param  无
  * @retval 无
  */
static void ADC1_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;	//GPIO初始化结构
	  ADC_InitTypeDef ADC_InitStructure;		//ADC1初始化结构
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    //打开GPIO和ADC1的时钟
  
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA , ENABLE);
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	

    //配置对应GPIO口，这里使用了PA0即ADC1   通道0
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	//
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);				// PA0,输入时不用设置速率
	
		/* TIME1 configuration */
		TIM_DeInit(TIM3);
		TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
		/* TIM3 used for timing, the timing period depends on the sample rate */
		TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;    /* TIM3CLK = 72 MHz */
		TIM_TimeBaseStructure.TIM_Period = 124;             //对应8K采样
		TIM_TimeBaseStructure.TIM_ClockDivision = 0x00;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_SelectOutputTrigger(TIM3,TIM_TRGOSource_Update); //选择TRGO触发源为计时器更新时间
//		TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中
		TIM_Cmd(TIM3, DISABLE);

	  //配置ADC1的工作模式
	  ADC_DeInit(ADC1);		//反初始化
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;			//独立ADC模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE ; 	 				//开启扫描模式，扫描模式用于多通道采集
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;			//开启连续转换模式，即不停地进行ADC转换
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;	//不使用外部触发转换
	  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 	//采集数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1;	 								//要转换的通道数目1
    ADC_Init(ADC1, &ADC_InitStructure);
	
//			/*配置ADC时钟，为PCLK2的6分频，即12MHz*/
	  RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
// 	//配置ADC1的转换顺序和采样周期
 	  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_1Cycles5);	
	
 	  ADC_Cmd(ADC1, ENABLE);		//使能ADC1
	
	/*复位校准寄存器 */
	  ADC_ResetCalibration(ADC1);		
    while(ADC_GetResetCalibrationStatus(ADC1));/*等待校准寄存器复位完成 */

	/* ADC校准 */
    ADC_StartCalibration(ADC1);		
    while(ADC_GetCalibrationStatus(ADC1));/* 等待校准完成*/
	
	//打开ADC中断
	//ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

	//设置ADC外部触发模式
	 ADC_ExternalTrigConvCmd(ADC1, ENABLE);
	//ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/**
  * @brief  配置ADC1的DMA传输
  * @param  无
  * @retval 无
  * ADC1 in Channel1 of DMA1
  */
static void ADC1_DMA1_Config()
{
	DMA_InitTypeDef DMA_InitStructure;
	
	 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	/* DMA channel1 配置 */
    DMA_DeInit(DMA1_Channel1);
    DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	 			//ADC地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&(g_ADC_Value);	//内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = VcBuf_Len;			//要缓存的数据个数
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//外设地址固定
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  				//内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//半字
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;										//循环传输
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    /* Enable DMA channel1 */
    DMA_Cmd(DMA1_Channel1, ENABLE);	//DMA1的通道1
		DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
		ADC_DMACmd(ADC1,ENABLE);
}

/**
  * @brief  ADC1初始化
  * @param  无
  * @retval 无
  */
void ADC1_Init(void)
{
    ADC1_Config();	//配置ADC
    ADC1_DMA1_Config();	//配置DMA
}

/**
  * @brief  获取ADC值
  * @param  无
  * @retval 无
  */
u8 Get_Adc(void)   
{
	u16 temp = 0;
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

 	temp = ADC_GetConversionValue(ADC1);// - (u16)2.05/3.3*4096;	//去除偏置
//  	temp = temp*2;	//放大四倍
//  	temp = temp + 2048;		//加上3.3/2V偏置
	
	return (u8)(temp>>4);	//返回最近一次ADC1规则组的转换结果
}
/*********************************************END OF FILE**********************/
