#include "sys.h"
#include "usart.h"	 
#include "stdarg.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	   
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化 
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/7/8
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
//修改说明 
//V1.1 20160708 
//移植自F429的例程,针对F7做了相应修改
////////////////////////////////////////////////////////////////////////////////// 

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)  
//解决HAL库使用时,某些情况可能报错的bug
int _ttywrch(int ch)    
{
    ch=ch;
	return ch;
}
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART3->ISR&0X40)==0);//循环发送,直到发送完毕   
	USART3->TDR = (u8) ch;      
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////

#if EN_USART3_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
//串口发送缓存区 	
__align(8) u8 USART_TX_BUF[USART_MAX_SEND_LEN]; 	//发送缓冲,最大USART3_MAX_SEND_LEN字节
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  
  
void USART3_IRQHandler(void)
{
/*	u8 res;	     
	if(USART3->ISR&(1<<5))//接收到数据
	{	 
		res=USART3->RDR; 			 
		if((USART_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据
		{ 
			if(USART_RX_STA<USART_REC_LEN)	//还可以接收数据
			{
				TIM7->CNT=0;         				//计数器清空
				if(USART_RX_STA==0) 				//使能定时器7的中断 
				{
					TIM7->CR1|=1<<0;     			//使能定时器7
				}
				USART_RX_BUF[USART_RX_STA++]=res;	//记录接收到的值	 
			}else 
			{
				USART_RX_STA|=1<<15;				//强制标记接收完成
			} 
		}
	}  	*/		
	u8 res;	
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	if(USART3->ISR&(1<<5))//接收到数据
	{	 
		res=USART3->RDR; 
		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{
				if(res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
			}else //还没收到0X0D
			{	
				if(res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=res;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}  		 									     
	} 
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
} 
#endif										 
//初始化IO 串口1
//pclk2:PCLK2时钟频率(Mhz)
//bound:波特率 
void uart_init(u32 pclk2,u32 bound)
{  	 
	u32	temp;
	u16 mantissa;
	u16 fraction;	
	temp=(float)(pclk2*1000000)/(bound*16);//得到USARTDIV,OVER8设置为0
	mantissa=temp;				 	//得到整数部分
	fraction=(temp-mantissa)*16; 	//得到小数部分,OVER8设置为0	
	mantissa<<=4;
	mantissa+=fraction;
	//temp=(pclk2*1000000+bound/2)/bound;	//得到USARTDIV@OVER8=0,采用四舍五入计算
	RCC->AHB1ENR|=1<<2;   	//使能PORTC口时钟  
	RCC->APB1ENR|=1<<18;  	//使能串口3时钟 
	GPIO_Set(GPIOC,PIN10|PIN11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PA9,PA10,复用功能,上拉输出
 	GPIO_AF_Set(GPIOC,10,7);	//PC10,AF7
	GPIO_AF_Set(GPIOC,11,7);//PC11,AF7  	   
	//波特率设置
	USART3->BRR=mantissa; 			// 波特率设置
 	//USART3->BRR=temp; 		//波特率设置@OVER8=0 	
	USART3->CR1=0;		 	//清零CR1寄存器
	//USART3->CR1|=0<<28;	 	//设置M1=0
	//USART3->CR1|=0<<12;	 	//设置M0=0&M1=0,选择8位字长 
	//USART3->CR1|=0<<15; 	//设置OVER8=0,16倍过采样 
	USART3->CR1|=1<<3;  	//串口发送使能 
//#if EN_USART1_RX		  	//如果使能了接收
	//使能接收中断 
	USART3->CR1|=1<<2;  	//串口接收使能
	USART3->CR1|=1<<5;    	//接收缓冲区非空中断使能	 
	USART3->CR1|=1<<0;  	//串口使能   
	MY_NVIC_Init(0,0,USART3_IRQn,2);//组2，最低优先级 	
	//MY_NVIC_Init(3,3,USART3_IRQn,2);//组2，最低优先级 
//#endif
	//USART3->CR1|=1<<0;  	//串口使能
//	TIM7_Int_Init(1000-1,8400-1);	//100ms中断一次
//	TIM7->CR1&=~(1<<0);				//关闭定时器7
	USART_RX_STA=0;				//清零 
}


//ATK-ESP8266发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
u8* atk_8266_check_cmd(u8 *str)
{
	char *check=NULL;
	if(USART_RX_STA&0x8000)
		{				
			USART_RX_BUF[USART_RX_STA&0x7FFF]=0;
			check=strstr((const char*)str,(const char*)USART_RX_BUF);
		}
		return (u8*)check;
}

//ATK-ESP8266退出透传模式
//返回值:0,退出成功;
//       1,退出失败
void atk_8266_quit_trans(void)
{
	while((USART3->ISR&0X40)==0);	//等待发送空
	USART3->TDR='+';      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART3->ISR&0X40)==0);	//等待发送空
	USART3->TDR='+';      
	delay_ms(15);					//大于串口组帧时间(10ms)
	while((USART3->ISR&0X40)==0);	//等待发送空
	USART3->TDR='+';      
	delay_ms(500);					//等待500ms
	atk_8266_send_cmd("AT","OK",20);//退出透传判断.
}

//向ATK-ESP8266发送命令
//cmd:发送的命令字符串
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART_RX_STA=0;
	u_printf("%s\r\n",cmd);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(atk_8266_check_cmd(ack))
				{
					//printf("ack:%s\r\n",(u8*)ack);
					break;//得到有效数据 
				}
					USART_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 
	
//向ATK-ESP8266发送指定数据
//data:发送的数据(不需要添加回车了)
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)luojian
u8 atk_8266_send_data(u8 *data,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART_RX_STA=0;
	printf("%s",data);	//发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(10);
			if(USART_RX_STA&0X8000)//接收到期待的应答结果
			{
				if(atk_8266_check_cmd(ack))break;//得到有效数据 
				USART_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
}

void atk_8266_sta_zhang(void)
{
	atk_8266_quit_trans();
/*	printf("AT+CWMODE=1\r\n");
	delay_ms(1000);
	printf("AT+RST\r\n");
	delay_ms(3000);
	printf("AT+CWJAP=\"k108\",\"robot108\"\r\n");
	delay_ms(3000);
	printf("AT+CIPSTART=\"TCP\",\"192.168.1.103\",8080\r\n");
	delay_ms(1000);
	printf("AT+CIPMODE=1\r\n");
	delay_ms(1000);
	printf("AT+CIPSEND\r\n");*/
	
	/*u_printf("%s\r\n","AT+CWMODE=1");
	delay_ms(1000);
	u_printf("%s\r\n","AT+RST");
	delay_ms(3000);
	u_printf("%s\r\n","AT+CWJAP=\"k108\",\"robot108\"");
	delay_ms(3000);
	u_printf("%s\r\n","AT+CIPSTART=\"TCP\",\"192.168.1.103\",8080");
	delay_ms(1000);
	u_printf("%s\r\n","AT+CIPMODE=1");
	delay_ms(1000);
	u_printf("%s\r\n","AT+CIPSEND");*/
	
	atk_8266_send_cmd("AT+CWMODE=1","OK",50);		//设置WIFI STA模式
	atk_8266_send_cmd("AT+RST","OK",200);		//重新启动
	delay_ms(1000);         //延时3S等待重启成功
	delay_ms(1000);
	delay_ms(1000);
	//delay_ms(1000);
	atk_8266_send_cmd("AT+CWJAP=\"k108\",\"robot108\"","WIFI",800);
	//while(atk_8266_send_cmd("AT+CWJAP=\"k108\",\"robot108\"","WIFI",800));					//连接目标路由器,并且获得IP
	//atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0：单连接，1：多连接
	atk_8266_send_cmd("AT+CIPSTART=\"TCP\",\"192.168.1.112\",8080","OK",200);
	//while(atk_8266_send_cmd("AT+CIPSTART=\"TCP\",\"192.168.1.103\",8080","OK",200));
	atk_8266_send_cmd("AT+CIFSR","OK",50);
	atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //传输模式为：透传
	atk_8266_send_cmd("AT+CIPSEND","OK",200);      //传输模式为：透传
}

//串口3,printf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
void u_printf(char* fmt,...)  
{  
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART_TX_BUF);//此次发送数据的长度
	for(j=0;j<i;j++)//循环发送数据
	{
		while((USART3->ISR&0X40)==0);//循环发送,直到发送完毕   
		USART3->TDR=USART_TX_BUF[j];  
	}
}
/*
//定时器7中断服务程序		    
void TIM7_IRQHandler(void)
{ 	  		    
	if(TIM7->SR&0X01)//是更新中断
	{	 			   
		USART_RX_STA|=1<<15;	//标记接收完成
		TIM7->SR&=~(1<<0);		//清除中断标志位		   
		TIM7->CR1&=~(1<<0);		//关闭定时器7	  
	}	      											 
} 
//通用定时器7中断初始化
//这里时钟选择为APB1的2倍，而APB1为42M
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz 
void TIM7_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<5;	//TIM7时钟使能    
 	TIM7->ARR=arr;  	//设定计数器自动重装值 
	TIM7->PSC=psc;  	//预分频器	  
	TIM7->CNT=0;  		//计数器清零	  
	TIM7->DIER|=1<<0;   //允许更新中断	  
	TIM7->CR1|=0x01;    //使能定时器7
  MY_NVIC_Init(0,1,TIM7_IRQn,2);	//抢占0，子优先级1，组2									 
} */



