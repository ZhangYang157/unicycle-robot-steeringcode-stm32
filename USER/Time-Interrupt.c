


void TIME_Interrupt_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<2;	//TIM3ʱ��ʹ��    
 	TIM4->ARR=arr;  	//�趨�������Զ���װֵ 
	TIM4->PSC=psc;  	//Ԥ��Ƶ��	  
	TIM4->DIER|=1<<0;   //���������ж�	  
	TIM4->CR1|=0x01;    //ʹ�ܶ�ʱ��3
  	MY_NVIC_Init(1,3,TIM4_IRQn,2);	//��ռ1�������ȼ�3����2									 
}