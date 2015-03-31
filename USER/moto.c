#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stdio.h"
#include "sys.h"

#include "delay.h"
#include "moto.h"


/*
**LEFT_CTL1 PC5
**LEFT_CTL2 PB7
**
**RIGHT_CTL1 PG14
**RIGHT_CTL2 PG15
*/




/******************
* @brief:驱动电机控制io初始化
* @param:
* @date:2015.1.6
* @aulther: zzl
* @other:
 *******************/

void moto_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
//	NVIC_InitTypeDef NVIC_InitStruct;
//	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
/**********					初始化马达控制io				**************/
	/************左右行走电机io************/
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	PCout(9)=1;
	
	
	/***********初始化马达电源控制io*************/

	
	
	/***********马达控制计时器中断初始化*************/
	/************timer init using timer3***************/
	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  //enable clock  36M
//	

//	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
//		
//	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;  //??whats this?
//	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
//	TIM_TimeBaseInitStruct.TIM_Period = 200;
//	TIM_TimeBaseInitStruct.TIM_Prescaler = 36;//36M/36 = 1M  10us once
//	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
//	
//	TIM_CounterModeConfig(TIM3, TIM_CounterMode_Up);
//	NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
//	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
//	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStruct);
//	TIM_Cmd(TIM3, ENABLE);
}

#define DELAY 1
void xmotostep(int dir, int step)
{
	int i;
	PCout(1)=dir>0?1:0;
	delay_ms(2);
	PCout(9)=0;
	for(i=0;i<step*10;i++)
	{
		PCout(3)=1;
		delay_ms(DELAY);
		PCout(3)=0;
		PCout(3)=1;
	}
	PCout(9)=1;
}
