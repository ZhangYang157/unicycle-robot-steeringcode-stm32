#ifndef __GROUND_H
#define __GROUND_H
#include "sys.h"




//ͨ���ı�TIM3->CCR4��ֵ���ı�ռ�ձȣ��Ӷ�����LED0������
#define GROUND_PWM TIM8->CCR1    
//TIM9 CH2��ΪPWM DAC�����ͨ�� 
//#define PWM_DAC_VAL  TIM9->CCR2  

void TIM8_Int_Init(u16 arr,u16 psc);
void TIM8_PWM_Init(u32 arr,u32 psc);
#endif
