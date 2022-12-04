#ifndef __LED_H
#define	__LED_H

#include "stm32f10x.h"

/** the macro definition to trigger the led on or off 
  * 1 - off
  *0 - on
  */
#define ON  0
#define OFF 1

/* ���κ꣬��������������һ��ʹ�� */
#define LED1(a)	if (a)	\
					GPIO_SetBits(GPIOC,GPIO_Pin_1);\
					else		\
					GPIO_ResetBits(GPIOC,GPIO_Pin_1)

#define LED2(a)	if (a)	\
					GPIO_SetBits(GPIOC,GPIO_Pin_2);\
					else		\
					GPIO_ResetBits(GPIOC,GPIO_Pin_2)

#define LED3(a)	if (a)	\
					GPIO_SetBits(GPIOC,GPIO_Pin_3);\
					else		\
					GPIO_ResetBits(GPIOC,GPIO_Pin_3)


/* ֱ�Ӳ����Ĵ����ķ�������IO */
#define	digitalHi(p,i)				{p->BSRR=i;}			//����Ϊ�ߵ�ƽ		
#define digitalLo(p,i)				{p->BRR	=i;}				//����͵�ƽ
#define digitalToggle(p,i)		{p->ODR ^=i;}			//�����ת״̬


/* �������IO�ĺ� */
#define LED1_TOGGLE		digitalToggle(GPIOC,GPIO_Pin_1)
#define LED1_OFF			digitalHi(GPIOC,GPIO_Pin_1)
#define LED1_ON				digitalLo(GPIOC,GPIO_Pin_1)

#define LED2_TOGGLE		digitalToggle(GPIOC,GPIO_Pin_2)
#define LED2_OFF			digitalHi(GPIOC,GPIO_Pin_2)
#define LED2_ON				digitalLo(GPIOC,GPIO_Pin_2)

#define LED3_TOGGLE		digitalToggle(GPIOC,GPIO_Pin_3)
#define LED3_OFF			digitalHi(GPIOC,GPIO_Pin_3)
#define LED3_ON				digitalLo(GPIOC,GPIO_Pin_3)

void LED_GPIO_Config(void);

#endif /* __LED_H */
