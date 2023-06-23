#include "encoder.h"


/******************�����Ƕȱ�������ȡ***************************************
*����ԭ��:		TIM2_IRQHandler(void)
				void TIM2_Int_Init(u16 arr,u16 psc)
				TIM2_PWM_Init(u32 arr,u32 psc)
				void TIM2_CH1_Cap_Init(u16 arr,u16 psc)
������ҪӦ��TIM2�Ա���ֵ���ж�ȡ����TIM2��16λ�ģ����arrΪu16��������

*******************************************************************************/
//��ʱ��2�жϷ������	 
void TIM2_IRQHandler(void)
{ 		  
	if(TIM2->SR&0X0001)//����ж�
	{			    				   				     	    	
	}				   
	TIM2->SR&=~(1<<0);//����жϱ�־λ 	    
}
//ͨ�ö�ʱ��2�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ54M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!
void TIM2_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<0;	//TIM4ʱ��ʹ��    
 	TIM2->ARR=arr;  	//�趨�������Զ���װֵ 
	TIM2->PSC=psc;  	//Ԥ��Ƶ��	  
	TIM2->DIER|=1<<0;   //��������ж�	  
	TIM2->CR1|=0x01;    //ʹ�ܶ�ʱ��3
  	MY_NVIC_Init(1,3,TIM2_IRQn,2);	//��ռ1�������ȼ�3����2									 
}

//TIM4 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM2_PWM_Init(u32 arr,u32 psc)
{		 					 
	//�˲������ֶ��޸�IO������
	RCC->APB1ENR|=1<<0;		//TIM4ʱ��ʹ��    
	RCC->AHB1ENR|=1<<0;   	//ʹ��PORTAʱ��	
	RCC->AHB1ENR|=1<<1;   	//ʹ��PORTBʱ��	
	GPIO_Set(GPIOA,PIN15,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//���ù���,�������
	GPIO_AF_Set(GPIOA,15,2);	//PD12,AF2 
	
	TIM2->ARR=arr;			//�趨�������Զ���װֵ 
	TIM2->PSC=psc;			//Ԥ��Ƶ������Ƶ 
	TIM2->CCMR2|=6<<12;  	//CH4 PWM1ģʽ		 
	TIM2->CCMR2|=1<<11; 	//CH4 Ԥװ��ʹ��	   
	TIM2->CCER|=1<<12;   	//OC4 ���ʹ��	
	TIM2->CCER|=1<<13;   	//OC4 �͵�ƽ��Ч	   
	TIM2->CR1|=1<<7;   		//ARPEʹ�� 
	TIM2->CR1|=1<<0;    	//ʹ�ܶ�ʱ��3										  
}  

//��ʱ��4ͨ��1���벶������
//arr���Զ���װֵ(TIM4��16λ��!!)
//psc��ʱ��Ԥ��Ƶ��
void TIM2_CH1_Cap_Init(u16 arr,u16 psc)
{		 
	RCC->APB1ENR|=1<<0;   	//TIM2 ʱ��ʹ�� 
	RCC->AHB1ENR|=1<<0;   	//ʹ��PORTAʱ��	
	RCC->AHB1ENR|=1<<1;   	//ʹ��PORTBʱ��
	GPIO_Set(GPIOA,PIN15,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PD);//���ù���,����
	GPIO_AF_Set(GPIOA,15,1);	//PA15,AF1
	GPIO_Set(GPIOB,PIN3,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PD);//���ù���,����
	GPIO_AF_Set(GPIOB,3,1);	//PB3,AF1
 	TIM2->ARR=arr;  		//�趨�������Զ���װֵ   
	TIM2->PSC=psc;  		//Ԥ��Ƶ�� 

	TIM2->CCMR1|=1<<0;		//CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM2->CCMR1|=1<<8;		//CC2S=01 	ѡ������� IC2ӳ�䵽TI2��
 	TIM2->CCMR1|=0<<4; 		//IC1F=0000 ���������˲��� ���˲�
	TIM2->CCMR1|=0<<12; 	//IC2F=0000 ���������˲��� ���˲�
 	TIM2->CCMR1|=0<<2;   	//IC1PS=00 	���������Ƶ,����Ƶ 
	TIM2->CCMR1|=0<<10; 	//IC2PS=00 	���������Ƶ,����Ƶ 

	TIM2->CCER|=0<<1; 		//CC1P=0	�����ش���
	TIM2->CCER|=0<<5; 		//CC2P=0	�����ش���

	TIM2->EGR=1<<0;			//������Ʋ��������¼�,ʹд��PSC��ֵ������Ч,���򽫻�Ҫ�ȵ���ʱ������Ż���Ч!
	TIM2->SMCR|=3<<0;       //SMS=011�������غ��½��ؾ�����
	TIM2->CNT|=0;
	TIM2->CR1|=0x01;    	//ʹ�ܼ�����2
	MY_NVIC_Init(2,0,TIM2_IRQn,2);//��ռ2�������ȼ�0����2	   
}


/******************���ֽǶȱ�������ȡ***************************************
*����ԭ��:		TIM5_IRQHandler(void)
				void TIM5_Int_Init(u16 arr,u16 psc)
				TIM5_PWM_Init(u32 arr,u32 psc)
				void TIM5_CH1_Cap_Init(u16 arr,u16 psc)
������ҪӦ��TIM5�Ա���ֵ���ж�ȡ����TIM5��32λ�ģ����arrΪu32��������

*******************************************************************************/
//��ʱ��5�жϷ������	 
void TIM5_IRQHandler(void)
{ 		  
	if(TIM5->SR&0X0001)//����ж�
	{			    				   				     	    	
	}				   
	TIM5->SR&=~(1<<0);//����жϱ�־λ 	    
}
//ͨ�ö�ʱ��5�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ54M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!
void TIM5_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<3;	//TIM4ʱ��ʹ��    
 	TIM5->ARR=arr;  	//�趨�������Զ���װֵ 
	TIM5->PSC=psc;  	//Ԥ��Ƶ��	  
	TIM5->DIER|=1<<0;   //��������ж�	  
	TIM5->CR1|=0x01;    //ʹ�ܶ�ʱ��3
  	MY_NVIC_Init(1,3,TIM5_IRQn,2);	//��ռ1�������ȼ�3����2									 
}

//TIM5 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM5_PWM_Init(u32 arr,u32 psc)
{		 					 
	//�˲������ֶ��޸�IO������
	RCC->APB1ENR|=1<<3;		//TIM4ʱ��ʹ��    
	RCC->AHB1ENR|=1<<0;   	//ʹ��PORTDʱ��	
	GPIO_Set(GPIOA,PIN0,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//���ù���,�������
	GPIO_AF_Set(GPIOA,0,2);	//PD12,AF2 
	
	TIM5->ARR=arr;			//�趨�������Զ���װֵ 
	TIM5->PSC=psc;			//Ԥ��Ƶ������Ƶ 
	TIM5->CCMR2|=6<<12;  	//CH4 PWM1ģʽ		 
	TIM5->CCMR2|=1<<11; 	//CH4 Ԥװ��ʹ��	   
	TIM5->CCER|=1<<12;   	//OC4 ���ʹ��	
	TIM5->CCER|=1<<13;   	//OC4 �͵�ƽ��Ч	   
	TIM5->CR1|=1<<7;   		//ARPEʹ�� 
	TIM5->CR1|=1<<0;    	//ʹ�ܶ�ʱ��3											  
}  

//��ʱ��5ͨ��1���벶������
//arr���Զ���װֵ(TIM5��32λ��!!)
//psc��ʱ��Ԥ��Ƶ��
void TIM5_CH1_Cap_Init(u32 arr,u16 psc)
{		 
	RCC->APB1ENR|=1<<3;   	//TIM5 ʱ��ʹ�� 
	RCC->AHB1ENR|=1<<0;   	//ʹ��PORTAʱ��	
	GPIO_Set(GPIOA,PIN0,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PD);//���ù���,����
	GPIO_AF_Set(GPIOA,0,2);	//PA0,AF2
	GPIO_Set(GPIOA,PIN1,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PD);//���ù���,����
	GPIO_AF_Set(GPIOA,1,2);	//PA1,AF2
 	TIM5->ARR=arr;  		//�趨�������Զ���װֵ   
	TIM5->PSC=psc;  		//Ԥ��Ƶ�� 
	TIM5->CCMR1|=1<<0;		//CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM5->CCMR1|=1<<8;		//CC2S=01 	ѡ������� IC2ӳ�䵽TI2��
 	TIM5->CCMR1&=~(1<<4); 		//IC1F=0000 ���������˲��� ���˲�
	TIM5->CCMR1&=~(1<<12); 	//IC2F=0000 ���������˲��� ���˲�
 	TIM5->CCMR1&=~(1<<2);   	//IC1PS=00 	���������Ƶ,����Ƶ 
	TIM5->CCMR1&=~(1<<10); 	//IC2PS=00 	���������Ƶ,����Ƶ 

	TIM5->CCER&=~(1<<1); 		//CC1P=0	�����ش���
	TIM5->CCER&=~(1<<5); 		//CC2P=0	�����ش���
	TIM5->CCER|=1<<0;
	TIM5->CCER|=1<<4;
	TIM5->EGR=1<<0;			//������Ʋ��������¼�,ʹд��PSC��ֵ������Ч,���򽫻�Ҫ�ȵ���ʱ������Ż���Ч!
	TIM5->SMCR|=3<<0;       //SMS=011�������غ��½��ؾ�����
	TIM5->CNT=0;
	TIM5->CR1|=0x01;    	//ʹ�ܼ�����2
	MY_NVIC_Init(2,0,TIM5_IRQn,2);//��ռ2�������ȼ�0����2	   
}



