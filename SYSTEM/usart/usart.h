#ifndef __USART_H
#define __USART_H 
#include "sys.h"
#include "delay.h"
#include "stdio.h"	
#include "string.h"
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

#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define USART_MAX_SEND_LEN		800					//����ͻ����ֽ���
#define EN_USART3_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	

void uart_init(u32 pclk2,u32 bound); 

u8* atk_8266_check_cmd(u8 *str);
void atk_8266_quit_trans(void);
u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
u8 atk_8266_send_data(u8 *data,u8 *ack,u16 waittime);
void atk_8266_sta_zhang(void);
void u_printf(char* fmt,...) ;
//void TIM7_Int_Init(u16 arr,u16 psc);
#endif	   
















