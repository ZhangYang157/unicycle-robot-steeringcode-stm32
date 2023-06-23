#include "time_interrpt.h"

void TIM_interrupt_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<2;	//TIM4时钟使能    
 	TIM4->ARR=arr;  	//设定计数器自动重装值 
	TIM4->PSC=psc;  	//预分频器	  
	TIM4->DIER|=1<<0;   //允许更新中断	  
	TIM4->CR1|=0x01;    //使能定时器4
  	MY_NVIC_Init(1,3,TIM4_IRQn,2);	//抢占1，子优先级3，组2			 
}