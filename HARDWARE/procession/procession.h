#ifndef __PROCESSION_H
#define __PROCESSION_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK STM32������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/7/11
//�汾��V1.3
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2014-2024
//All rights reserved					
//********************************************************************************
//�޸�˵��
//V1.1 20160711
//����TIM3_PWM_Init����,����PWM���	
//V1.2 20160711
//����TIM5_CH1_Cap_Init����,�������벶��	
//V1.3 20160713
//����TIM9_CH2_PWM_Init����,����PWM DACʵ��					  
////////////////////////////////////////////////////////////////////////////////// 	 

//ͨ���ı�TIM3->CCR4��ֵ���ı�ռ�ձȣ��Ӷ�����LED0������
#define PROCESSION_PWM TIM1->CCR1    
//TIM9 CH2��ΪPWM DAC�����ͨ�� 
//#define PWM_DAC_VAL  TIM9->CCR2  

void TIM1_Int_Init(u16 arr,u16 psc);
void TIM1_PWM_Init(u32 arr,u32 psc);
//void TIM5_CH1_Cap_Init(u32 arr,u16 psc);
//void TIM9_CH2_PWM_Init(u16 arr,u16 psc);	
#endif
