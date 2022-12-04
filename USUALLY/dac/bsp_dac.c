/**
  ******************************************************************************
  * @file    bsp_xxx.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   adc1 应用bsp / DMA 模式
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
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
 * 函数名：DAC1_GPIO_Config
 * 描述  ：使能DAC1和DMA1的时钟，初始化PA.04
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
static void DAC1_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	DAC_InitTypeDef DAC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	/* 使能 GPIOA 和 DAC 时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC|RCC_APB1Periph_TIM2, ENABLE);//注意DAC clock is int APB1
	
	/* 配置PA.04为模拟输出 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;	//GPIO_Pin_4|,//PA4，PA5是DAC1,DAC2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);			
		/* TIME1 configuration */
		TIM_DeInit(TIM2);
		TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
		/* TIM3 used for timing, the timing period depends on the sample rate */
		TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;    /* TIM3CLK = 72 MHz */
		TIM_TimeBaseStructure.TIM_Period = 124;             //对应8K采样
		TIM_TimeBaseStructure.TIM_ClockDivision = 0x00;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
		TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_SelectOutputTrigger(TIM2,TIM_TRGOSource_Update); //选择TRGO触发源为计时器更新时间
//		TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中
		TIM_Cmd(TIM2, DISABLE);

	/* 配置DAC1 */
	DAC_DeInit();
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;	// DAC_Trigger_T2_TRGO
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None ; 	 //无噪声
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;		//使能输出，不打开就没有输出
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);

	DAC_Cmd(DAC_Channel_1,ENABLE);
	DAC_SetChannel1Data(DAC_Align_12b_R, 0);  //12位右对齐数据格式设置DAC值
}

 /* 函数名：ADC1_Mode_Config
 * 描述  ：配置ADC1的工作模式为MDA模式
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 * DAC1 in Channel3 of DMA2
 */
static void DAC1_DMA2_Config(void)
{
	DMA_InitTypeDef  DMA_InitStructure;

	/* 使能DMA2时钟 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	
	/* 配置DMA2 */
	DMA_DeInit(DMA2_Channel3);		//反初始化
	DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12RD_Address;					//外设数据地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)g_ADC_Value;			//内存数据地址 DualSine12bit
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;								//数据传输方向内存至外设
	DMA_InitStructure.DMA_BufferSize = VcBuf_Len;		//数据个数											//缓存大小为32字节	
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				//外设数据地址固定	
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;							//内存数据地址自增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;			//外设数据以字为单位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;					//内存数据以字为单位	
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;				//DMA_Mode_Circular;//循环模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;								//高DMA通道优先级
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;									//非内存至内存模式
	DMA_Init(DMA2_Channel3, &DMA_InitStructure);

	/* 使能DMA2 3通道 */
	DMA_Cmd(DMA2_Channel3, ENABLE);  
	DMA_ITConfig(DMA2_Channel3, DMA_IT_TC, ENABLE);
	DAC_DMACmd(DAC_Channel_1, ENABLE);	//使能DMA传输
}



/*
 * 函数名：DAC1_Init
 * 描述  ：无
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
void DAC1_Init(void)
{
	DAC1_Config();
	DAC1_DMA2_Config();
}

/**
  * @brief  DAC1自动输出开/关
  * @param  无
  * @retval 无
  */
void DAC1_Cmd(FunctionalState status)
{
	if (status)
	{
		TIM_Cmd(TIM2, ENABLE);	//打开定时器
	}
	else
	{
		TIM_Cmd(TIM2, DISABLE);	//关闭定时器
	}
}

/**
  * @brief  输出到DAC1，即PA.04
  * @param  无
  * @retval 无
  */
void DAC1_Out(uint16_t data)
{
	//DAC_SetChannel1Data(DAC_Align_8b_R, data);		//8位右对齐
	//DAC_Cmd(DAC_Channel_1,ENABLE);
	DAC_SetChannel1Data(DAC_Align_8b_R, data);		//8位右对齐，这句话必须要有！！！也许是ST的BUG ~..~，偏移地址之间的字节数好像会出问题哦~~有空再深究
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


























