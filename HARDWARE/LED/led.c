#include "led.h" 
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

//��ʼ��PB0��PB1Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{    	 
	RCC->AHB1ENR|=1<<0;	//ʹ��PORTAʱ�� 
	GPIO_Set(GPIOA,PIN9,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //PA9����
	RCC->AHB1ENR|=1<<3;	//ʹ��PORTDʱ�� 
	GPIO_Set(GPIOD,PIN13,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //PD13����
	GPIO_Set(GPIOD,PIN14,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //PD14����
	RCC->AHB1ENR|=1<<4;	//ʹ��PORTEʱ�� 
	GPIO_Set(GPIOE,PIN2,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //PE2����
	GPIO_Set(GPIOE,PIN3,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //PE3����
	GPIO_Set(GPIOE,PIN11,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //PE11����
	GPIO_Set(GPIOE,PIN14,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //PE14����
	LED0(1);
	LED1(1);
	LED2(1);
	LED3(1);
}







