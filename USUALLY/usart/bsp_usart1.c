/**
  ******************************************************************************
  * @file    bsp_usart1.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����c��printf������usart�˿�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  

#include "bsp_usart1.h"

/**
 * @brief  USART1 GPIO ����,����ģʽ���á�115200 8-N-1
 * @param  ��
 * @retval ��
 */
void USART1_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /* config USART1 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    /* USART1 GPIO config */
    /* Configure USART1 Tx (PA.09) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* Configure USART1 Rx (PA.10) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* USART1 mode config */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}

///�ض���c�⺯��printf��USART1
int fputc(int ch, FILE *f)
{
    /* ����һ���ֽ����ݵ�USART1 */
    USART_SendData(USART1, (uint8_t) ch);

    /* �ȴ�������� */
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

    return (ch);
}

void usart2_init(void)
{
		USART_InitTypeDef USART_InitStructure;
		
	  GPIO_InitTypeDef GPIO_InitStructure;

	  NVIC_InitTypeDef NVIC_InitStructure;
	
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
		
	 
		/* Usart1 Tx ����--PA9  Usart2 Tx ����--PA2 */	    
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		/* Usart1 Rx ����--PA10 Usart2 Rx ����--PA3 */
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure); 
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
			//Usart1 NVIC ����
		 NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=5 ;//��ռ���ȼ�7
		 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�0
		 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		 NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ��� 
	
	  USART_Cmd(USART2, DISABLE);
	 
	  USART_InitStructure.USART_BaudRate=115200;
	  USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	  USART_InitStructure.USART_StopBits=USART_StopBits_1;
		USART_InitStructure.USART_Parity=USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode=USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);
	
	  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//ʹ�ܴ��ڽ����ж�	
	 	USART_Cmd(USART2, ENABLE);//��������
}

///�ض���c�⺯��scanf��USART1
int fgetc(FILE *f)
{
    /* �ȴ�����1�������� */
    while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

    return (int)USART_ReceiveData(USART1);
}
/*********************************************END OF FILE**********************/
