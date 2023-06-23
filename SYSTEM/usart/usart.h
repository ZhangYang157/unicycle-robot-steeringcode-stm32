#ifndef __USART_H
#define __USART_H 
#include "sys.h"
#include "delay.h"
#include "stdio.h"	
#include "string.h"
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

#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define USART_MAX_SEND_LEN		800					//最大发送缓存字节数
#define EN_USART3_RX 			1		//使能（1）/禁止（0）串口1接收
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	

void uart_init(u32 pclk2,u32 bound); 

u8* atk_8266_check_cmd(u8 *str);
void atk_8266_quit_trans(void);
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
u8 atk_8266_send_data(u8 *data,u8 *ack,u16 waittime);
void atk_8266_sta_zhang(void);
void u_printf(char* fmt,...) ;
//void TIM7_Int_Init(u16 arr,u16 psc);
#endif	   
















