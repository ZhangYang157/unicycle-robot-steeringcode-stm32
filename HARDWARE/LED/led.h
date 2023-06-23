#ifndef __LED_H
#define __LED_H	 
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/7/11
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

//LED�˿ڶ���
#define D12(x)			GPIO_Pin_Set(GPIOA,PIN9,x)		// DigIN12
#define LED0(x)			GPIO_Pin_Set(GPIOD,PIN13,x)		// LED0
#define LED1(x)			GPIO_Pin_Set(GPIOD,PIN14,x)		// LED1
#define LED2(x)			GPIO_Pin_Set(GPIOE,PIN2,x)		// LED2
#define LED3(x)			GPIO_Pin_Set(GPIOE,PIN3,x)		// LED3
#define D22(x)			GPIO_Pin_Set(GPIOE,PIN11,x)		// DigIN22
#define D24(x)			GPIO_Pin_Set(GPIOE,PIN14,x)		// DigIN22

void LED_Init(void);	//��ʼ��		 				    
#endif
