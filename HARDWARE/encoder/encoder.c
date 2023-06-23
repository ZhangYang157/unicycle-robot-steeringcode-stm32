#include "encoder.h"


/******************进动角度编码器读取***************************************
*函数原型:		TIM2_IRQHandler(void)
				void TIM2_Int_Init(u16 arr,u16 psc)
				TIM2_PWM_Init(u32 arr,u32 psc)
				void TIM2_CH1_Cap_Init(u16 arr,u16 psc)
这里主要应用TIM2对编码值进行读取，因TIM2是16位的，因此arr为u16长度数据

*******************************************************************************/
//定时器2中断服务程序	 
void TIM2_IRQHandler(void)
{ 		  
	if(TIM2->SR&0X0001)//溢出中断
	{			    				   				     	    	
	}				   
	TIM2->SR&=~(1<<0);//清除中断标志位 	    
}
//通用定时器2中断初始化
//这里时钟选择为APB1的2倍，而APB1为54M
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM2_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<0;	//TIM4时钟使能    
 	TIM2->ARR=arr;  	//设定计数器自动重装值 
	TIM2->PSC=psc;  	//预分频器	  
	TIM2->DIER|=1<<0;   //允许更新中断	  
	TIM2->CR1|=0x01;    //使能定时器3
  	MY_NVIC_Init(1,3,TIM2_IRQn,2);	//抢占1，子优先级3，组2									 
}

//TIM4 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM2_PWM_Init(u32 arr,u32 psc)
{		 					 
	//此部分需手动修改IO口设置
	RCC->APB1ENR|=1<<0;		//TIM4时钟使能    
	RCC->AHB1ENR|=1<<0;   	//使能PORTA时钟	
	RCC->AHB1ENR|=1<<1;   	//使能PORTB时钟	
	GPIO_Set(GPIOA,PIN15,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//复用功能,上拉输出
	GPIO_AF_Set(GPIOA,15,2);	//PD12,AF2 
	
	TIM2->ARR=arr;			//设定计数器自动重装值 
	TIM2->PSC=psc;			//预分频器不分频 
	TIM2->CCMR2|=6<<12;  	//CH4 PWM1模式		 
	TIM2->CCMR2|=1<<11; 	//CH4 预装载使能	   
	TIM2->CCER|=1<<12;   	//OC4 输出使能	
	TIM2->CCER|=1<<13;   	//OC4 低电平有效	   
	TIM2->CR1|=1<<7;   		//ARPE使能 
	TIM2->CR1|=1<<0;    	//使能定时器3										  
}  

//定时器4通道1输入捕获配置
//arr：自动重装值(TIM4是16位的!!)
//psc：时钟预分频数
void TIM2_CH1_Cap_Init(u16 arr,u16 psc)
{		 
	RCC->APB1ENR|=1<<0;   	//TIM2 时钟使能 
	RCC->AHB1ENR|=1<<0;   	//使能PORTA时钟	
	RCC->AHB1ENR|=1<<1;   	//使能PORTB时钟
	GPIO_Set(GPIOA,PIN15,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PD);//复用功能,下拉
	GPIO_AF_Set(GPIOA,15,1);	//PA15,AF1
	GPIO_Set(GPIOB,PIN3,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PD);//复用功能,下拉
	GPIO_AF_Set(GPIOB,3,1);	//PB3,AF1
 	TIM2->ARR=arr;  		//设定计数器自动重装值   
	TIM2->PSC=psc;  		//预分频器 

	TIM2->CCMR1|=1<<0;		//CC1S=01 	选择输入端 IC1映射到TI1上
	TIM2->CCMR1|=1<<8;		//CC2S=01 	选择输入端 IC2映射到TI2上
 	TIM2->CCMR1|=0<<4; 		//IC1F=0000 配置输入滤波器 不滤波
	TIM2->CCMR1|=0<<12; 	//IC2F=0000 配置输入滤波器 不滤波
 	TIM2->CCMR1|=0<<2;   	//IC1PS=00 	配置输入分频,不分频 
	TIM2->CCMR1|=0<<10; 	//IC2PS=00 	配置输入分频,不分频 

	TIM2->CCER|=0<<1; 		//CC1P=0	上升沿触发
	TIM2->CCER|=0<<5; 		//CC2P=0	上升沿触发

	TIM2->EGR=1<<0;			//软件控制产生更新事件,使写入PSC的值立即生效,否则将会要等到定时器溢出才会生效!
	TIM2->SMCR|=3<<0;       //SMS=011，上升沿和下降沿均触发
	TIM2->CNT|=0;
	TIM2->CR1|=0x01;    	//使能计数器2
	MY_NVIC_Init(2,0,TIM2_IRQn,2);//抢占2，子优先级0，组2	   
}


/******************底轮角度编码器读取***************************************
*函数原型:		TIM5_IRQHandler(void)
				void TIM5_Int_Init(u16 arr,u16 psc)
				TIM5_PWM_Init(u32 arr,u32 psc)
				void TIM5_CH1_Cap_Init(u16 arr,u16 psc)
这里主要应用TIM5对编码值进行读取，因TIM5是32位的，因此arr为u32长度数据

*******************************************************************************/
//定时器5中断服务程序	 
void TIM5_IRQHandler(void)
{ 		  
	if(TIM5->SR&0X0001)//溢出中断
	{			    				   				     	    	
	}				   
	TIM5->SR&=~(1<<0);//清除中断标志位 	    
}
//通用定时器5中断初始化
//这里时钟选择为APB1的2倍，而APB1为54M
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM5_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<3;	//TIM4时钟使能    
 	TIM5->ARR=arr;  	//设定计数器自动重装值 
	TIM5->PSC=psc;  	//预分频器	  
	TIM5->DIER|=1<<0;   //允许更新中断	  
	TIM5->CR1|=0x01;    //使能定时器3
  	MY_NVIC_Init(1,3,TIM5_IRQn,2);	//抢占1，子优先级3，组2									 
}

//TIM5 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM5_PWM_Init(u32 arr,u32 psc)
{		 					 
	//此部分需手动修改IO口设置
	RCC->APB1ENR|=1<<3;		//TIM4时钟使能    
	RCC->AHB1ENR|=1<<0;   	//使能PORTD时钟	
	GPIO_Set(GPIOA,PIN0,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);//复用功能,上拉输出
	GPIO_AF_Set(GPIOA,0,2);	//PD12,AF2 
	
	TIM5->ARR=arr;			//设定计数器自动重装值 
	TIM5->PSC=psc;			//预分频器不分频 
	TIM5->CCMR2|=6<<12;  	//CH4 PWM1模式		 
	TIM5->CCMR2|=1<<11; 	//CH4 预装载使能	   
	TIM5->CCER|=1<<12;   	//OC4 输出使能	
	TIM5->CCER|=1<<13;   	//OC4 低电平有效	   
	TIM5->CR1|=1<<7;   		//ARPE使能 
	TIM5->CR1|=1<<0;    	//使能定时器3											  
}  

//定时器5通道1输入捕获配置
//arr：自动重装值(TIM5是32位的!!)
//psc：时钟预分频数
void TIM5_CH1_Cap_Init(u32 arr,u16 psc)
{		 
	RCC->APB1ENR|=1<<3;   	//TIM5 时钟使能 
	RCC->AHB1ENR|=1<<0;   	//使能PORTA时钟	
	GPIO_Set(GPIOA,PIN0,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PD);//复用功能,下拉
	GPIO_AF_Set(GPIOA,0,2);	//PA0,AF2
	GPIO_Set(GPIOA,PIN1,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PD);//复用功能,下拉
	GPIO_AF_Set(GPIOA,1,2);	//PA1,AF2
 	TIM5->ARR=arr;  		//设定计数器自动重装值   
	TIM5->PSC=psc;  		//预分频器 
	TIM5->CCMR1|=1<<0;		//CC1S=01 	选择输入端 IC1映射到TI1上
	TIM5->CCMR1|=1<<8;		//CC2S=01 	选择输入端 IC2映射到TI2上
 	TIM5->CCMR1&=~(1<<4); 		//IC1F=0000 配置输入滤波器 不滤波
	TIM5->CCMR1&=~(1<<12); 	//IC2F=0000 配置输入滤波器 不滤波
 	TIM5->CCMR1&=~(1<<2);   	//IC1PS=00 	配置输入分频,不分频 
	TIM5->CCMR1&=~(1<<10); 	//IC2PS=00 	配置输入分频,不分频 

	TIM5->CCER&=~(1<<1); 		//CC1P=0	上升沿触发
	TIM5->CCER&=~(1<<5); 		//CC2P=0	上升沿触发
	TIM5->CCER|=1<<0;
	TIM5->CCER|=1<<4;
	TIM5->EGR=1<<0;			//软件控制产生更新事件,使写入PSC的值立即生效,否则将会要等到定时器溢出才会生效!
	TIM5->SMCR|=3<<0;       //SMS=011，上升沿和下降沿均触发
	TIM5->CNT=0;
	TIM5->CR1|=0x01;    	//使能计数器2
	MY_NVIC_Init(2,0,TIM5_IRQn,2);//抢占2，子优先级0，组2	   
}



