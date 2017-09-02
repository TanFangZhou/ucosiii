#ifndef __KEY_H__
#define __KEY_H__

#include "stm32f4xx.h"
#include "delay.h"


#define KEY0  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)
#define KEY1  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)


void KEY_Init(void);
int  KeyDetect(void);





#endif

