#ifndef __ADC_H
#define	__ADC_H


#include "stm32f10x.h"
void ADC1_Init(void);
u8 Get_Adc(void);
#define Frame_size  160
#define	VcBuf_Len	16000	//�������������� ��λ���� ÿ��������16λ
extern int16_t g_ADC_Value[];
extern uint32_t addressFlash;
extern uint32_t addressreadFlash;
#endif /* __ADC_H */

