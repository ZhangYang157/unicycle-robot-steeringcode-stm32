#include "gyro.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/7/11
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved					
//********************************************************************************
//�޸�˵��
//V1.1 20160711
//����TIM3_PWM_Init����,����PWM���							  
////////////////////////////////////////////////////////////////////////////////// 	 


//��ʱ��3�жϷ������	 
void TIM3_IRQHandler(void)
{ 		  
	if(TIM3->SR&0X0001)//����ж�
	{		    				   				     	    	
	}				   
	TIM3->SR&=~(1<<0);//����жϱ�־λ 	    
}
//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ54M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3ʱ��ʹ��    
 	TIM3->ARR=arr;  	//�趨�������Զ���װֵ 
	TIM3->PSC=psc;  	//Ԥ��Ƶ��	  
	TIM3->DIER|=1<<0;   //��������ж�	  
	TIM3->CR1|=0x01;    //ʹ�ܶ�ʱ��3
  	MY_NVIC_Init(1,3,TIM3_IRQn,2);	//��ռ1�������ȼ�3����2									 
}

//TIM3 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM3_PWM_Init(u32 arr,u32 psc)
{
	RCC->APB1ENR|=1<<1;		//TIM3ʱ��ʹ��    
	RCC->AHB1ENR|=1<<1;   	//ʹ��PORTBʱ��	
	GPIO_Set(GPIOB,PIN4,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//���ù���,�������
	GPIO_Set(GPIOB,PIN5,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//���ù���,�������
	GPIO_AF_Set(GPIOB,4,2);	//PB0,AF2
	GPIO_AF_Set(GPIOB,5,2);	//PB1,AF2 
	
	TIM3->ARR=arr;			//�趨�������Զ���װֵ 
	TIM3->PSC=psc;			//Ԥ��Ƶ������Ƶ 
	TIM3->CCMR1|=6<<4;  	//CH3 PWM1ģʽ
	TIM3->CCMR1|=6<<12;  	//CH4 PWM1ģʽ		
	TIM3->CCMR1|=1<<3; 	    //CH3 Ԥװ��ʹ��	   	
	TIM3->CCMR1|=1<<11; 	//CH4 Ԥװ��ʹ��	   
	TIM3->CCER|=1<<0;   	//OC3 ���ʹ��
	TIM3->CCER|=1<<4;   	//OC4 ���ʹ��
	TIM3->CCER|=0<<1;   	//OC3 �͵�ƽ��Ч	
	TIM3->CCER|=0<<5;   	//OC4 �͵�ƽ��Ч	   
  
	TIM3->CR1|=1<<7;   		//ARPEʹ�� 
	TIM3->CR1|=1<<0;    	//ʹ�ܶ�ʱ��3		
}


