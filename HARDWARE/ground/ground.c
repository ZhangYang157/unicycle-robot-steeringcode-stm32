#include "ground.h"


//定时器8中断服务程序	 
void TIM8_CC_IRQHandler(void)
{ 		  
	if(TIM8->SR&0X0001)//溢出中断
	{   				   				     	    	
	}				   
	TIM8->SR&=~(1<<0);//清除中断标志位 	    
}
//通用定时器8中断初始化
//这里时钟选择为APB1的2倍，而APB1为54M
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM8_Int_Init(u16 arr,u16 psc)
{
	RCC->APB2ENR|=1<<1;	//TIM1时钟使能    
 	TIM8->ARR=arr;  	//设定计数器自动重装值 
	TIM8->PSC=psc;  	//预分频器	  
	TIM8->DIER|=1<<0;   //允许更新中断	  
	TIM8->CR1|=0x01;    //使能计数器1
  	MY_NVIC_Init(1,3,TIM8_CC_IRQn,2);	//抢占1，子优先级3，组2									 
}

//TIM8 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM8_PWM_Init(u32 arr,u32 psc)
{	

	RCC->APB2ENR|=1<<1;		//TIM1时钟使能    
	RCC->AHB1ENR|=1<<2;   	//使能PORTA时钟	
	GPIO_Set(GPIOC,PIN6,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//复用功能,上拉输出
	GPIO_AF_Set(GPIOC,6,3);	//PA7,AF2 
	
	TIM8->ARR=arr;			//设定计数器自动重装值 
	TIM8->PSC=psc;			//预分频器不分频 
	
	TIM8->CCMR1|=6<<4;  	//CH4 PWM1模式		 
	TIM8->CCMR1|=1<<3; 	//CH4 预装载使能	 	
	TIM8->CCER|=1<<0;   	//OC4 输出使能	CC1NE
	TIM8->CR1|=1<<7;   		//ARPE使能 
	TIM8->CR1|=1<<0;    	//使能定时器3	
	TIM8->BDTR|=1<<15;
}  
