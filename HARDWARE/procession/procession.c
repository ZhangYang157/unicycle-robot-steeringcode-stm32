#include "procession.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/7/11
//版本：V1.3
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved					
//********************************************************************************
//修改说明
//V1.1 20160711
//新增TIM3_PWM_Init函数,用于PWM输出	
//V1.2 20160711
//新增TIM5_CH1_Cap_Init函数,用于输入捕获	
//V1.3 20160713
//新增TIM9_CH2_PWM_Init函数,用于PWM DAC实验					  
////////////////////////////////////////////////////////////////////////////////// 	 

//定时器1中断服务程序	 
void TIM1_CC_IRQHandler(void)
{ 		  
	if(TIM1->SR&0X0001)//溢出中断
	{   				   				     	    	
	}				   
	TIM1->SR&=~(1<<0);//清除中断标志位 	    
}
//通用定时器1中断初始化
//这里时钟选择为APB1的2倍，而APB1为54M
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM1_Int_Init(u16 arr,u16 psc)
{
	RCC->APB2ENR|=1<<0;	//TIM8时钟使能    
 	TIM1->ARR=arr;  	//设定计数器自动重装值 
	TIM1->PSC=psc;  	//预分频器	  
	TIM1->DIER|=1<<0;   //允许更新中断	  
	TIM1->CR1|=0x01;    //使能计数器1
  	MY_NVIC_Init(1,3,TIM1_CC_IRQn,2);	//抢占1，子优先级3，组2									 
}

//TIM3 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM1_PWM_Init(u32 arr,u32 psc)
{	

	RCC->APB2ENR|=1<<0;		//TIM8时钟使能    
	RCC->AHB1ENR|=1<<0;   	//使能PORTB时钟	
	GPIO_Set(GPIOA,PIN8,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//复用功能,上拉输出
	GPIO_AF_Set(GPIOA,8,1);	//PB15,AF3 
	
	TIM1->ARR=arr;			//设定计数器自动重装值 
	TIM1->PSC=psc;			//预分频器不分频 
	
	TIM1->CCMR1|=6<<4;  	//CH4 PWM1模式		 
	TIM1->CCMR1|=1<<3; 	//CH4 预装载使能	 	
	TIM1->CCER|=1<<0;   	//OC4 输出使能	CC3NE
	TIM1->CR1|=1<<7;   		//ARPE使能 
	TIM1->CR1|=1<<0;    	//使能定时器3	
	TIM1->BDTR|=1<<15;
}  
