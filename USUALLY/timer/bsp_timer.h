#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"

void TIM2_Config(void);
void TIM5_Int_Init(u16 arr,u16 psc);
#endif	/* __TIMER_H */
