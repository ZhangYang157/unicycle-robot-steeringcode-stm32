#include "dma.h"																	   	  
#include "delay.h"

//DMAx的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7
//chx:DMA通道选择,范围:0~7
//par:外设地址
//mar:存储器地址
//ndtr:数据传输量 
void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx,u32 par,u32 mar,u16 ndtr,u8 dr)
{ 
	DMA_TypeDef *DMAx;
	u8 streamx;
	if((u32)DMA_Streamx>(u32)DMA2)//得到当前stream是属于DMA2还是DMA1
	{
		DMAx=DMA2;
		RCC->AHB1ENR|=1<<22;//DMA2时钟使能 
	}else 
	{
		DMAx=DMA1; 
 		RCC->AHB1ENR|=1<<21;//DMA1时钟使能 
	}
	while(DMA_Streamx->CR&0X01);//等待DMA可配置 
	streamx=(((u32)DMA_Streamx-(u32)DMAx)-0X10)/0X18;		//得到stream通道号
 	if(streamx>=6)DMAx->HIFCR|=0X3D<<(6*(streamx-6)+16);	//清空之前该stream上的所有中断标志
	else if(streamx>=4)DMAx->HIFCR|=0X3D<<6*(streamx-4);    //清空之前该stream上的所有中断标志
	else if(streamx>=2)DMAx->LIFCR|=0X3D<<(6*(streamx-2)+16);//清空之前该stream上的所有中断标志
	else DMAx->LIFCR|=0X3D<<6*streamx;						//清空之前该stream上的所有中断标志
	
	DMA_Streamx->PAR=par;		//DMA外设地址
	DMA_Streamx->M0AR=mar;		//DMA 存储器0地址
	DMA_Streamx->NDTR=ndtr;		//DMA 存储器0地址
	DMA_Streamx->CR=0;			//先全部复位CR寄存器值 
	
	DMA_Streamx->CR|=dr<<6;		//存储器到外设模式
	DMA_Streamx->CR|=0<<8;		//非循环模式(即使用普通模式)    0为非循环，1为循环模式
	DMA_Streamx->CR|=0<<9;		//外设非增量模式
	DMA_Streamx->CR|=1<<10;		//存储器增量模式
	DMA_Streamx->CR|=0<<11;		//外设数据长度:8位
	DMA_Streamx->CR|=0<<13;		//存储器数据长度:8位
	DMA_Streamx->CR|=1<<16;		//中等优先级
	DMA_Streamx->CR|=0<<21;		//外设突发单次传输
	DMA_Streamx->CR|=0<<23;		//存储器突发单次传输
	DMA_Streamx->CR|=(u32)chx<<25;//通道选择
	DMA_Streamx->FCR=0X21;	//FIFO控制寄存器
} 

//开启一次DMA发送数据
//DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7 
//ndtr:数据传输量  
void DMA_Send_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr)
{
	DMA_Streamx->CR&=~(1<<0); 	//关闭DMA传输 
	while(DMA_Streamx->CR&0X1);	//确保DMA可以被设置 
	DMA_Streamx->NDTR=ndtr;		//DMA 要传输数据项数目 
	DMA_Streamx->CR|=1<<0;		//开启DMA传输
}



void MYDMA_Config_IIC(DMA_Stream_TypeDef *DMA_Streamx,u8 chx,u32 par,u32 mar,u16 ndtr,u8 dr)
{ 
	DMA_TypeDef *DMAx;
	u8 streamx;
	if((u32)DMA_Streamx>(u32)DMA2)//得到当前stream是属于DMA2还是DMA1
	{
		DMAx=DMA2;
		RCC->AHB1ENR|=1<<22;//DMA2时钟使能 
	}else 
	{
		DMAx=DMA1; 
 		RCC->AHB1ENR|=1<<21;//DMA1时钟使能 
	}
	while(DMA_Streamx->CR&0X01);//等待DMA可配置 
	streamx=(((u32)DMA_Streamx-(u32)DMAx)-0X10)/0X18;		//得到stream通道号
 	if(streamx>=6)DMAx->HIFCR|=0X3D<<(6*(streamx-6)+16);	//清空之前该stream上的所有中断标志
	else if(streamx>=4)DMAx->HIFCR|=0X3D<<6*(streamx-4);    //清空之前该stream上的所有中断标志
	else if(streamx>=2)DMAx->LIFCR|=0X3D<<(6*(streamx-2)+16);//清空之前该stream上的所有中断标志
	else DMAx->LIFCR|=0X3D<<6*streamx;						//清空之前该stream上的所有中断标志
	
	DMA_Streamx->PAR=par;		//DMA外设地址
	DMA_Streamx->M0AR=mar;		//DMA 存储器0地址
	DMA_Streamx->NDTR=ndtr;		//DMA 存储器0地址
	DMA_Streamx->CR=0;			//先全部复位CR寄存器值 
	
	DMA_Streamx->CR|=dr<<6;		//1为存储器到外设模式，0为外设到存储器
	DMA_Streamx->CR|=0<<8;		//非循环模式(即使用普通模式)
	DMA_Streamx->CR|=0<<9;		//外设非增量模式
	DMA_Streamx->CR|=1<<10;		//存储器增量模式
	DMA_Streamx->CR|=0<<11;		//外设数据长度:8位
	DMA_Streamx->CR|=0<<13;		//存储器数据长度:8位
	DMA_Streamx->CR|=2<<16;		//高等优先级
	DMA_Streamx->CR|=0<<21;		//外设突发单次传输
	DMA_Streamx->CR|=0<<23;		//存储器突发单次传输
	//DMA_Streamx->CR|=1<<4;      //传输完成中断使能
	DMA_Streamx->CR|=(u32)chx<<25;//通道选择
	DMA_Streamx->FCR=0X21;	//FIFO控制寄存器
	
	
	MY_NVIC_Init(1,3,DMA1_Stream2_IRQn,2);     //DMA1_S2的中断优先级设置，抢占1，子优先级3，组2
} 

//硬件IIC与DMA同时初始化，mar为存储器的地址（需给定指针地址）
void IIC_DMA_INIT(u32 mar)
{
	u8 count=0;
	
	//1 DMA设置
	//1.1 DMA时钟、长度等设置
	RCC->AHB1ENR|=1<<21;                         //DMA1时钟使能 
	while(DMA1_Stream2->CR&0x01);                //等待DMA能够配置
	DMA1->LIFCR|=0x3D<<16;                       //清空stream2上的所有中断标志
	DMA1_Stream2->PAR=(u32)&I2C4->RXDR;          //DMA的外设地址
	DMA1_Stream2->M0AR=mar;                      //DMA的存储器0地址
	//DMA1_Stream2->M1AR=mar;                      //DMA的存储器1地址
	DMA1_Stream2->NDTR|=24;                       //DMA传输数据长度
	//1.2 CR控制寄存器设置
	DMA1_Stream2->CR=0;
	//DMA1_Stream2->CR|=1<<5;                      //1为外设是流控制器，0为DMA是流控制器
	DMA1_Stream2->CR&=~(1<<6);                   //设置数据传输方向，00为从外设到存储器
	DMA1_Stream2->CR|=0<<8;                      //循环模式设置，1为使能循环，0为禁止循环
	DMA1_Stream2->CR|=0<<9;                      //外设地址增加模式设置，1位每次数据传输后，外设地址指针递增（增量为PSIZE，CR中11和12），0位外设地址指针固定
	DMA1_Stream2->CR&=~(1<<11);                  //外设数据大小PSIZE，00为字节（8位），01为半字（16位），10为字（32位）
	DMA1_Stream2->CR|=1<<10;                     //存储器地址增加模式设置，1为每次数据传输后，存储器地址指针递增（增量为MSIZE，CR中13和14），0为地址指针固定
	DMA1_Stream2->CR&=~(1<<13);                  //存储器数据大小MSIZE，00为字节（8位），01为半字（16位），10为字（32位）
	DMA1_Stream2->CR|=3<<16;                     //优先级设置，00为低，01为中，10为高
	DMA1_Stream2->CR&=~(1<<21);                  //外设突发传输设置，00位单次传输
	DMA1_Stream2->CR&=~(1<<23);                  //存储器突发传输设置，00位单次传输
	DMA1_Stream2->CR|=(u32)2<<25;                //通道选择，这里为通道2
	//1.3 FIFO设置
	//DMA1_Stream2->FCR|=4<<3;                     //FIFO状态设置，100是FIFO为空
	DMA1_Stream2->FCR|=1<<5;                     //FIFO为空
	DMA1_Stream2->FCR&=~(1<<2);                  //直接模式设置，0为使能直接模式，1为禁止直接模式
	DMA1_Stream2->FCR|=1<<0;                     //FIFO阈值选择，01位容量的1/2，整体FIFO有4个32位（4个字）
	
	//2 IIC硬件设置
	I2C4->CR1&=~(1<<0);                          //PE位清零，禁用IIC4
	while(I2C4->CR1&0x01);
	//2.1 RCC时钟设置
	RCC->AHB1ENR|=1<<1;                          //GPIOB时钟使能
	GPIO_Set(GPIOB,PIN8,GPIO_MODE_AF,GPIO_OTYPE_OD,GPIO_SPEED_50M,GPIO_PUPD_PU);//设置B8复用功能，上拉
	GPIO_Set(GPIOB,PIN9,GPIO_MODE_AF,GPIO_OTYPE_OD,GPIO_SPEED_50M,GPIO_PUPD_PU);//设置B9复用功能，上拉
	GPIO_AF_Set(GPIOB,8,1);                      //B8应用复用功能1
	GPIO_AF_Set(GPIOB,9,1);                      //B9应用复用功能1
	RCC->APB1ENR|=1<<24;                         //IIC4使能
	RCC->APB1RSTR|=1<<24;                        //IIC4复位
	RCC->APB1RSTR&=~(1<<24);                     //IIC4复位
	//2.2 噪声滤波设置
	I2C4->CR1&=~(1<<12);                         //模拟滤波器使能
	I2C4->CR1|=0x00<<8;                          //禁用数字滤波器
	//2.3时序设置
	I2C4->TIMINGR=0x10830D3C;                    //快速模式，400KHz,I2C时钟频率54000KHz，上升时间200ns，下降时间200ns.旧0x10830D3C，新0x10830D25
	//2.4 IIC控制寄存器设置
	//I2C2->CR1|=1<<2;                             //使能RX中断，RXNE
	//I2C2->CR1|=1<<1;                             //使能TX中断，TXIS
	//I2C2->CR1|=1<<15;                            //使能DMA接收
	//I2C2->CR1|=1<<14;                            //使能DMA发送
	//I2C2->CR1|=1<<7;                             //使能错误中断
	//I2C2->CR1|=1<<6;                             //使能传输完成中断
	//I2C2->CR1|=1<<5;                             //使能停止位检测中断
	//I2C2->CR1|=1<<4;                             //使能否定应答中断
	I2C4->CR1|=1<<15;                              //DMA接收使能 RXNE=1
	//I2C4->CR1|=1<<14;                              //DMA发送使能 TXNE=1
	I2C4->CR1|=1<<0;                               //PE为1，启用IIC4

}

void IIC_DMA_READ()
{
	//DMA设置
	DMA1_Stream2->CR&=~(1<<0);                   //EN位为0，禁止DMA
	while(DMA1_Stream2->CR&0x01);                //等待DMA能够配置
	DMA1_Stream2->NDTR|=24;                       //DMA传输数据长度
	DMA1_Stream2->CR|=1<<0;                      //EN位为1，开启DMA
	
	//IIC读取设置
	I2C4->CR1&=~(1<<0);    //PE位清零，各配置位复位
	delay_us(30);
	I2C4->CR1|=1<<0;       //PE为1，启用IIC2
	I2C4->CR2&=~(1<<13);   //START位停止
	I2C4->CR2&=~(0x7FF);    //地址位和方向位清零
	I2C4->CR2|=1<<11;   //ADD10=1，在10位寻址模式下
	I2C4->CR2&=~(1<<12);   //HEAD10R=0
	I2C4->CR2|=0x234;    //地址设置
	I2C4->CR2|=1<<10;    //传输方向设置
	
	I2C4->CR2|=24<<16;//接收数据长度
	I2C4->CR2|=1<<25;      //自动结束方式
	//I2C4->CR2|=1<<24;      //重载模式NBYES，为1时传输完设定长度后继续等待
	I2C4->CR1|=1<<15;     //DMA接收使能
	//SCB_CleanDCache();
	I2C4->CR2|=1<<13;         //开始
	
	/*//7位寻址方式进行DMA读取
	//Write_Byte_Soft(0x34,0x7A);
	I2C4->CR1&=~(1<<0);    //PE位清零，各配置位复位
	delay_us(30);
	I2C4->CR1|=1<<0;       //PE为1，启用IIC4
	
	I2C4->CR2&=~(1<<13);   //START位停止
	I2C4->CR2&=~(1<<11);   //ADD10=0，在7位寻址模式下
	

	I2C4->CR2&=~(0xFF07FF);    //地址位,方向位及数据长度清零
	I2C4->CR2|=0x7A<<1;    //地址设置
	I2C4->CR2|=1<<10;    //传输方向设置
	I2C4->CR2|=24<<16;//接收数据长度
	I2C4->CR2|=1<<25;      //自动结束方式
	
	//I2C2->CR2|=1<<14;          //停止位置1
	I2C4->CR2|=1<<13;         //开始*/

}
/*
void DMA1_Stream2_IRQHandler(void)
{
	if(DMA1->LISR &(1<<21))
				{
					DMA1->LIFCR|=1<<21;
				}
				I2C4->CR2|=1<<14;         //发送停止

				delay_ms(20);
				
				I2C4->CR1&=~(1<<0);    //PE位清零，各配置位复位
				delay_us(30);
				I2C4->CR1|=1<<0;       //PE为1，启用IIC2
				//I2C4->CR2&=~(0x7FF);    //地址位和方向位清零
				I2C4->CR2|=1<<11;   //ADD10=1，在10位寻址模式下
				I2C4->CR1|=1<<15;     //DMA接收使能
				I2C4->CR2|=0x234;    //地址设置
				I2C4->CR2|=1<<10;    //传输方向设置
				I2C4->CR2|=24<<16;//接收数据长度
				
				
				DMA_Send_Enable(DMA1_Stream2,24);
				I2C4->CR2|=1<<13;         //开始
					//I2C4->CR2|=24<<16;//接收数据长度
				//JY901_I2C_Read_Bytes_DMA(datatemp,0x234,24);
}*/

