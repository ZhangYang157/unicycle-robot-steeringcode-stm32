#include "ground.h"


//��ʱ��8�жϷ������	 
void TIM8_CC_IRQHandler(void)
{ 		  
	if(TIM8->SR&0X0001)//����ж�
	{   				   				     	    	
	}				   
	TIM8->SR&=~(1<<0);//����жϱ�־λ 	    
}
//ͨ�ö�ʱ��8�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ54M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!
void TIM8_Int_Init(u16 arr,u16 psc)
{
	RCC->APB2ENR|=1<<1;	//TIM1ʱ��ʹ��    
 	TIM8->ARR=arr;  	//�趨�������Զ���װֵ 
	TIM8->PSC=psc;  	//Ԥ��Ƶ��	  
	TIM8->DIER|=1<<0;   //��������ж�	  
	TIM8->CR1|=0x01;    //ʹ�ܼ�����1
  	MY_NVIC_Init(1,3,TIM8_CC_IRQn,2);	//��ռ1�������ȼ�3����2									 
}

//TIM8 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM8_PWM_Init(u32 arr,u32 psc)
{	

	RCC->APB2ENR|=1<<1;		//TIM1ʱ��ʹ��    
	RCC->AHB1ENR|=1<<2;   	//ʹ��PORTAʱ��	
	GPIO_Set(GPIOC,PIN6,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//���ù���,�������
	GPIO_AF_Set(GPIOC,6,3);	//PA7,AF2 
	
	TIM8->ARR=arr;			//�趨�������Զ���װֵ 
	TIM8->PSC=psc;			//Ԥ��Ƶ������Ƶ 
	
	TIM8->CCMR1|=6<<4;  	//CH4 PWM1ģʽ		 
	TIM8->CCMR1|=1<<3; 	//CH4 Ԥװ��ʹ��	 	
	TIM8->CCER|=1<<0;   	//OC4 ���ʹ��	CC1NE
	TIM8->CR1|=1<<7;   		//ARPEʹ�� 
	TIM8->CR1|=1<<0;    	//ʹ�ܶ�ʱ��3	
	TIM8->BDTR|=1<<15;
}  
