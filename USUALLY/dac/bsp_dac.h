#ifndef __DAC_H
#define	__DAC_H


#include "stm32f10x.h"

void DAC1_Init(void);
void DAC1_Tran(uint32_t *Add);
// void DAC1_Cmd(FunctionalState status);
void DAC1_Out(uint16_t data);

#endif /* __DAC_H */

