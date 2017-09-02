#include "key.h"



void KEY_Init(void)
{
	
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOA,GPIOE时钟
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4; //KEY0 KEY1 KEY2对应引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOE3,4
	
}

int KeyDetect(void)
{
    if(KEY0==0||KEY1==0)
    {
        delay_ms(10);
        if(KEY0==0) return 0;
        if(KEY1==0) return 1;
    }
    return 100;
}

