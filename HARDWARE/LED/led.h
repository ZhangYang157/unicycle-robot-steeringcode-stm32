#ifndef __LED_H
#define __LED_H	 
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/7/11
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

//LED端口定义
#define D12(x)			GPIO_Pin_Set(GPIOA,PIN9,x)		// DigIN12
#define LED0(x)			GPIO_Pin_Set(GPIOD,PIN13,x)		// LED0
#define LED1(x)			GPIO_Pin_Set(GPIOD,PIN14,x)		// LED1
#define LED2(x)			GPIO_Pin_Set(GPIOE,PIN2,x)		// LED2
#define LED3(x)			GPIO_Pin_Set(GPIOE,PIN3,x)		// LED3
#define D22(x)			GPIO_Pin_Set(GPIOE,PIN11,x)		// DigIN22
#define D24(x)			GPIO_Pin_Set(GPIOE,PIN14,x)		// DigIN22

void LED_Init(void);	//初始化		 				    
#endif
