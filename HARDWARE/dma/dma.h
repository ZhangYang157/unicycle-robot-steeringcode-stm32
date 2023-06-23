#ifndef __DMA_H
#define	__DMA_H	   
#include "sys.h"
#include "IIC_HD.h"

 

void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx,u32 par,u32 mar,u16 ndtr,u8 dr);//配置DMAx_CHx
void DMA_Send_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr);	//使能一次DMA传输	
void MYDMA_Config_IIC(DMA_Stream_TypeDef *DMA_Streamx,u8 chx,u32 par,u32 mar,u16 ndtr,u8 dr);
//void DMA1_Stream2_IRQHandler(void);
void IIC_DMA_INIT(u32 mar);
void IIC_DMA_READ();


#endif




