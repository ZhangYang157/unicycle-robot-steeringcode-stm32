#ifndef __ENCODER_H
#define __ENCODER_H
#include "sys.h"
				  
////////////////////////////////////////////////////////////////////////////////// 	 

//TIM2Ϊ�����Ǳ�������ȡ����Ϊ16λ��ʱ��
void TIM2_Int_Init(u16 arr,u16 psc);
void TIM2_PWM_Init(u32 arr,u32 psc);
void TIM2_CH1_Cap_Init(u16 arr,u16 psc);

//TIM5Ϊ���ֽǱ�������ȡ����Ϊ32λ��ʱ��
void TIM5_Int_Init(u16 arr,u16 psc);
void TIM5_PWM_Init(u32 arr,u32 psc);
void TIM5_CH1_Cap_Init(u32 arr,u16 psc);

#endif
