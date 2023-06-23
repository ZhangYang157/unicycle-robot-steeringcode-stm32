#ifndef __GROUND_H
#define __GROUND_H
#include "sys.h"




//通过改变TIM3->CCR4的值来改变占空比，从而控制LED0的亮度
#define GROUND_PWM TIM8->CCR1    
//TIM9 CH2作为PWM DAC的输出通道 
//#define PWM_DAC_VAL  TIM9->CCR2  

void TIM8_Int_Init(u16 arr,u16 psc);
void TIM8_PWM_Init(u32 arr,u32 psc);
#endif
