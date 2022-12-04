#ifndef __KEY_H
#define	__KEY_H

#include "stm32f10x.h"
 /*******
 *按键按下标置
 KEY_ON 0
 KEY_OFF 1
 ********/
#define KEY_ON	0
#define KEY_OFF	1

#define KEY1  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_5)//读取按键1
#define KEY2  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)//读取按键2
#define KEY3  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_1)//

void Key_GPIO_Config(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin,uint8_t Down_state);

#endif /* __LED_H */

