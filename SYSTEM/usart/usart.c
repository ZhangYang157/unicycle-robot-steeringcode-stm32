#include "sys.h"
#include "usart.h"	 
#include "stdarg.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	   
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ�� 
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/7/8
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//********************************************************************************
//�޸�˵�� 
//V1.1 20160708 
//��ֲ��F429������,���F7������Ӧ�޸�
////////////////////////////////////////////////////////////////////////////////// 

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)  
//���HAL��ʹ��ʱ,ĳЩ������ܱ����bug
int _ttywrch(int ch)    
{
    ch=ch;
	return ch;
}
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART3->ISR&0X40)==0);//ѭ������,ֱ���������   
	USART3->TDR = (u8) ch;      
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////

#if EN_USART3_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
//���ڷ��ͻ����� 	
__align(8) u8 USART_TX_BUF[USART_MAX_SEND_LEN]; 	//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
  
void USART3_IRQHandler(void)
{
/*	u8 res;	     
	if(USART3->ISR&(1<<5))//���յ�����
	{	 
		res=USART3->RDR; 			 
		if((USART_RX_STA&(1<<15))==0)//�������һ������,��û�б�����,���ٽ�����������
		{ 
			if(USART_RX_STA<USART_REC_LEN)	//�����Խ�������
			{
				TIM7->CNT=0;         				//���������
				if(USART_RX_STA==0) 				//ʹ�ܶ�ʱ��7���ж� 
				{
					TIM7->CR1|=1<<0;     			//ʹ�ܶ�ʱ��7
				}
				USART_RX_BUF[USART_RX_STA++]=res;	//��¼���յ���ֵ	 
			}else 
			{
				USART_RX_STA|=1<<15;				//ǿ�Ʊ�ǽ������
			} 
		}
	}  	*/		
	u8 res;	
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART3->ISR&(1<<5))//���յ�����
	{	 
		res=USART3->RDR; 
		if((USART_RX_STA&0x8000)==0)//����δ���
		{
			if(USART_RX_STA&0x4000)//���յ���0x0d
			{
				if(res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
				else USART_RX_STA|=0x8000;	//��������� 
			}else //��û�յ�0X0D
			{	
				if(res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=res;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}  		 									     
	} 
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
} 
#endif										 
//��ʼ��IO ����1
//pclk2:PCLK2ʱ��Ƶ��(Mhz)
//bound:������ 
void uart_init(u32 pclk2,u32 bound)
{  	 
	u32	temp;
	u16 mantissa;
	u16 fraction;	
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV,OVER8����Ϊ0
	mantissa=temp;				 	//�õ���������
	fraction=(temp-mantissa)*16; 	//�õ�С������,OVER8����Ϊ0	
	mantissa<<=4;
	mantissa+=fraction;
	//temp=(pclk2*1000000+bound/2)/bound;	//�õ�USARTDIV@OVER8=0,���������������
	RCC->AHB1ENR|=1<<2;   	//ʹ��PORTC��ʱ��  
	RCC->APB1ENR|=1<<18;  	//ʹ�ܴ���3ʱ�� 
	GPIO_Set(GPIOC,PIN10|PIN11,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//PA9,PA10,���ù���,�������
 	GPIO_AF_Set(GPIOC,10,7);	//PC10,AF7
	GPIO_AF_Set(GPIOC,11,7);//PC11,AF7  	   
	//����������
	USART3->BRR=mantissa; 			// ����������
 	//USART3->BRR=temp; 		//����������@OVER8=0 	
	USART3->CR1=0;		 	//����CR1�Ĵ���
	//USART3->CR1|=0<<28;	 	//����M1=0
	//USART3->CR1|=0<<12;	 	//����M0=0&M1=0,ѡ��8λ�ֳ� 
	//USART3->CR1|=0<<15; 	//����OVER8=0,16�������� 
	USART3->CR1|=1<<3;  	//���ڷ���ʹ�� 
//#if EN_USART1_RX		  	//���ʹ���˽���
	//ʹ�ܽ����ж� 
	USART3->CR1|=1<<2;  	//���ڽ���ʹ��
	USART3->CR1|=1<<5;    	//���ջ������ǿ��ж�ʹ��	 
	USART3->CR1|=1<<0;  	//����ʹ��   
	MY_NVIC_Init(0,0,USART3_IRQn,2);//��2��������ȼ� 	
	//MY_NVIC_Init(3,3,USART3_IRQn,2);//��2��������ȼ� 
//#endif
	//USART3->CR1|=1<<0;  	//����ʹ��
//	TIM7_Int_Init(1000-1,8400-1);	//100ms�ж�һ��
//	TIM7->CR1&=~(1<<0);				//�رն�ʱ��7
	USART_RX_STA=0;				//���� 
}


//ATK-ESP8266���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//    ����,�ڴ�Ӧ������λ��(str��λ��)
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

//ATK-ESP8266�˳�͸��ģʽ
//����ֵ:0,�˳��ɹ�;
//       1,�˳�ʧ��
void atk_8266_quit_trans(void)
{
	while((USART3->ISR&0X40)==0);	//�ȴ����Ϳ�
	USART3->TDR='+';      
	delay_ms(15);					//���ڴ�����֡ʱ��(10ms)
	while((USART3->ISR&0X40)==0);	//�ȴ����Ϳ�
	USART3->TDR='+';      
	delay_ms(15);					//���ڴ�����֡ʱ��(10ms)
	while((USART3->ISR&0X40)==0);	//�ȴ����Ϳ�
	USART3->TDR='+';      
	delay_ms(500);					//�ȴ�500ms
	atk_8266_send_cmd("AT","OK",20);//�˳�͸���ж�.
}

//��ATK-ESP8266��������
//cmd:���͵������ַ���
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART_RX_STA=0;
	u_printf("%s\r\n",cmd);	//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(10);
			if(USART_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(atk_8266_check_cmd(ack))
				{
					//printf("ack:%s\r\n",(u8*)ack);
					break;//�õ���Ч���� 
				}
					USART_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 
	
//��ATK-ESP8266����ָ������
//data:���͵�����(����Ҫ��ӻس���)
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)luojian
u8 atk_8266_send_data(u8 *data,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART_RX_STA=0;
	printf("%s",data);	//��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(10);
			if(USART_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(atk_8266_check_cmd(ack))break;//�õ���Ч���� 
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
	
	atk_8266_send_cmd("AT+CWMODE=1","OK",50);		//����WIFI STAģʽ
	atk_8266_send_cmd("AT+RST","OK",200);		//��������
	delay_ms(1000);         //��ʱ3S�ȴ������ɹ�
	delay_ms(1000);
	delay_ms(1000);
	//delay_ms(1000);
	atk_8266_send_cmd("AT+CWJAP=\"k108\",\"robot108\"","WIFI",800);
	//while(atk_8266_send_cmd("AT+CWJAP=\"k108\",\"robot108\"","WIFI",800));					//����Ŀ��·����,���һ��IP
	//atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0�������ӣ�1��������
	atk_8266_send_cmd("AT+CIPSTART=\"TCP\",\"192.168.1.112\",8080","OK",200);
	//while(atk_8266_send_cmd("AT+CIPSTART=\"TCP\",\"192.168.1.103\",8080","OK",200));
	atk_8266_send_cmd("AT+CIFSR","OK",50);
	atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //����ģʽΪ��͸��
	atk_8266_send_cmd("AT+CIPSEND","OK",200);      //����ģʽΪ��͸��
}

//����3,printf ����
//ȷ��һ�η������ݲ�����USART3_MAX_SEND_LEN�ֽ�
void u_printf(char* fmt,...)  
{  
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART_TX_BUF);//�˴η������ݵĳ���
	for(j=0;j<i;j++)//ѭ����������
	{
		while((USART3->ISR&0X40)==0);//ѭ������,ֱ���������   
		USART3->TDR=USART_TX_BUF[j];  
	}
}
/*
//��ʱ��7�жϷ������		    
void TIM7_IRQHandler(void)
{ 	  		    
	if(TIM7->SR&0X01)//�Ǹ����ж�
	{	 			   
		USART_RX_STA|=1<<15;	//��ǽ������
		TIM7->SR&=~(1<<0);		//����жϱ�־λ		   
		TIM7->CR1&=~(1<<0);		//�رն�ʱ��7	  
	}	      											 
} 
//ͨ�ö�ʱ��7�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ42M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz 
void TIM7_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<5;	//TIM7ʱ��ʹ��    
 	TIM7->ARR=arr;  	//�趨�������Զ���װֵ 
	TIM7->PSC=psc;  	//Ԥ��Ƶ��	  
	TIM7->CNT=0;  		//����������	  
	TIM7->DIER|=1<<0;   //��������ж�	  
	TIM7->CR1|=0x01;    //ʹ�ܶ�ʱ��7
  MY_NVIC_Init(0,1,TIM7_IRQn,2);	//��ռ0�������ȼ�1����2									 
} */



