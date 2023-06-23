#include "IIC_HD.h"
#include "sys.h"
#include "delay.h"

//IIC_HD初始化

void IIC_HD_Init(void)
{
	I2C4->CR1&=~(1<<0);    //PE位清零，禁用IIC4
	
	//RCC时钟设置
	RCC->AHB1ENR|=1<<1;    //GPIOB时钟使能
	GPIO_Set(GPIOB,PIN8,GPIO_MODE_AF,GPIO_OTYPE_OD,GPIO_SPEED_50M,GPIO_PUPD_PU);//设置B8复用功能，上拉
	GPIO_Set(GPIOB,PIN9,GPIO_MODE_AF,GPIO_OTYPE_OD,GPIO_SPEED_50M,GPIO_PUPD_PU);//设置B9复用功能，上拉
	GPIO_AF_Set(GPIOB,8,1);//B8应用复用功能1
	GPIO_AF_Set(GPIOB,9,1);//B9应用复用功能1
	RCC->APB1ENR|=1<<24;   //IIC4使能
	RCC->APB1RSTR|=1<<24;  //IIC4复位
	RCC->APB1RSTR&=~(1<<24);  //IIC4复位
	
	//噪声滤波设置
	I2C4->CR1&=~(1<<12);   //模拟滤波器使能
	I2C4->CR1|=0x00<<8;    //禁用数字滤波器
	
	//时序设置
	I2C4->TIMINGR=0x10830D3C;//快速模式，400KHz,I2C时钟频率54000KHz，上升时间200ns，下降时间200ns.旧0x10830D3C，新0x10830D25
	
	//I2C2->CR1|=1<<2;       //使能RX中断，RXNE
	//I2C2->CR1|=1<<1;       //使能TX中断，TXIS
	//I2C2->CR1|=1<<15;       //使能DMA接收
	//I2C2->CR1|=1<<14;       //使能DMA发送
	//I2C2->CR1|=1<<7;       //使能错误中断
	//I2C2->CR1|=1<<6;       //使能传输完成中断
	//I2C2->CR1|=1<<5;       //使能停止位检测中断
	//I2C2->CR1|=1<<4;       //使能否定应答中断

	I2C4->CR1|=1<<0;       //PE为1，启用IIC4
}

void IIC_HD_Init_DMA(u16 Addr,u8 Length)
{
	u8 count=0;
	I2C4->CR1&=~(1<<0);    //PE位清零，禁用IIC4
	
	//RCC时钟设置
	RCC->AHB1ENR|=1<<1;    //GPIOB时钟使能
	GPIO_Set(GPIOB,PIN8,GPIO_MODE_AF,GPIO_OTYPE_OD,GPIO_SPEED_50M,GPIO_PUPD_PU);//设置B8复用功能，上拉
	GPIO_Set(GPIOB,PIN9,GPIO_MODE_AF,GPIO_OTYPE_OD,GPIO_SPEED_50M,GPIO_PUPD_PU);//设置B9复用功能，上拉
	GPIO_AF_Set(GPIOB,8,1);//B8应用复用功能1
	GPIO_AF_Set(GPIOB,9,1);//B9应用复用功能1
	RCC->APB1ENR|=1<<24;   //IIC4使能
	RCC->APB1RSTR|=1<<24;  //IIC4复位
	RCC->APB1RSTR&=~(1<<24);  //IIC4复位
	
	//噪声滤波设置
	I2C4->CR1&=~(1<<12);   //模拟滤波器使能
	I2C4->CR1|=0x00<<8;    //禁用数字滤波器
	
	//时序设置
	I2C4->TIMINGR=0x10830D3C;//快速模式，400KHz,I2C时钟频率54000KHz，上升时间200ns，下降时间200ns.旧0x10830D3C，新0x10830D25
	
	//I2C2->CR1|=1<<2;       //使能RX中断，RXNE
	//I2C2->CR1|=1<<1;       //使能TX中断，TXIS
	//I2C2->CR1|=1<<15;       //使能DMA接收
	//I2C2->CR1|=1<<14;       //使能DMA发送
	//I2C2->CR1|=1<<7;       //使能错误中断
	//I2C2->CR1|=1<<6;       //使能传输完成中断
	//I2C2->CR1|=1<<5;       //使能停止位检测中断
	//I2C2->CR1|=1<<4;       //使能否定应答中断

	I2C4->CR1|=1<<0;       //PE为1，启用IIC4
	
	I2C4->CR2&=~(1<<13);   //START位停止
	I2C4->CR2=0;	
	//I2C4->CR2&=~(0x7FF);    //地址位和方向位清零
	I2C4->CR2|=1<<11;   //ADD10=1，在10位寻址模式下
	//I2C4->CR2&=~(1<<12);   //HEAD10R=0
	
	I2C4->CR2|=0x234;    //地址设置
	I2C4->CR2|=1<<10;    //传输方向设置
	
	
	I2C4->CR2|=Length<<16;//接收数据长度
	I2C4->CR2|=1<<25;      //软件结束方式
	
	//I2C4->CR1|=1<<15;     //DMA接收使能 RXNE=1
	
	//I2C2->CR2|=1<<14;          //停止位置1
	I2C4->CR2|=1<<13;         //开始
	
}

void Master_Communication_Init(u8 Addr,u8 WRN)
{
	I2C4->CR1&=~(1<<0);    //PE位清零，各配置位复位
	delay_us(30);
	I2C4->CR1|=1<<0;       //PE为1，启用IIC4
	
	I2C4->CR2&=~(1<<13);   //START位停止
	I2C4->CR2&=~(1<<11);   //ADD10=0，在7位寻址模式下
	

	I2C4->CR2&=~(0xFF07FF);    //地址位,方向位及数据长度清零
	I2C4->CR2|=Addr<<1;    //地址设置
	I2C4->CR2|=WRN<<10;    //传输方向设置	
}

void Master_Communication_Init_10(u16 Addr,u8 WRN)
{
	I2C4->CR1&=~(1<<0);    //PE位清零，各配置位复位
	delay_us(30);
	I2C4->CR1|=1<<0;       //PE为1，启用IIC2
	
	I2C4->CR2&=~(1<<13);   //START位停止
	I2C4->CR2|=1<<11;   //ADD10=1，在10位寻址模式下
	I2C4->CR2&=(1<<12);   //HEAD10R=0
	
	I2C4->CR2&=~(0xFF07FF);    //地址位和方向位清零
	I2C4->CR2|=Addr<<0;    //地址设置
	I2C4->CR2|=WRN<<10;    //传输方向设置	
}

u8 Read_Byte_Auto(u8 Addr)
{
	u8 data;
	if(Addr>127)
	{
		Master_Communication_Init_10((0x500<<2)+Addr,1);
		I2C4->CR2&=~(1<<12);
		
	}
	else Master_Communication_Init(Addr,1);
	I2C4->CR2|=1<<16;//接收数据长度
	I2C4->CR2|=1<<25;      //自动结束方式
	
	//I2C2->CR2|=1<<14;          //停止位置1
	I2C4->CR2|=1<<13;         //开始
	while(!(I2C4->ISR&(1<<2)))
	{
		
	}
	data=I2C4->RXDR;
	
	while((I2C4->ISR&(1<<6)))
	{
		
	}
	
	return data;
}

u8 Read_Byte_Soft(u8 Addr)
{
	u8 data;
	if(Addr>127)
	{
		Master_Communication_Init_10(Addr,1);
		I2C4->CR2&=~(1<<12);
		
	}
	else Master_Communication_Init(Addr,1);
	I2C4->CR2|=1<<16;//接收数据长度
	I2C4->CR2&=~(1<<25);      //软件结束方式
	
	//I2C2->CR2|=1<<14;          //停止位置1
	I2C4->CR2|=1<<13;         //开始
	while(!(I2C4->ISR&(1<<2)))
	{
		
	}
	data=I2C4->RXDR;
	
	while(!(I2C4->ISR&(1<<6)))
	{
		
	}
	
	return data;
}

void Write_Byte_Auto(u8 data,u8 Addr)
{
	if(Addr>127)
	{
		Master_Communication_Init_10(Addr,0);
		I2C4->CR2&=~(1<<12);
		
	}
	else Master_Communication_Init(Addr,0);
	I2C4->CR2|=1<<16;//发送数据长度
	I2C4->CR2|=1<<25;      //自动结束方式
	
	I2C4->CR2|=1<<13;         //开始
	while(!(I2C4->ISR&(1<<1)))
	{
		
	}
	I2C4->TXDR=data;
	while((I2C4->ISR&(1<<6)))
	{
		
	}
}

void Write_Byte_Soft(u8 data,u8 Addr)
{
	if(Addr>127)
	{
		Master_Communication_Init_10(Addr,0);
		I2C4->CR2&=~(1<<12);
		
	}
	else Master_Communication_Init(Addr,0);
	I2C4->CR2|=1<<16;//发送数据长度
	I2C4->CR2&=~(1<<25);      //软件结束方式
	
	I2C4->CR2|=1<<13;         //开始
	while(!(I2C4->ISR&(1<<1)))
	{
		
	}
	I2C4->TXDR=data;
	while(!(I2C4->ISR&(1<<6)))
	{

	}
}

void Write_Long_Auto(u8 data,u8 Addr,u8 Addr_in)
{
	if(Addr>127)
	{
		Master_Communication_Init_10(Addr,0);
		I2C4->CR2&=~(1<<12);
		
	}
	else Master_Communication_Init(Addr,0);
	I2C4->CR2|=2<<16;//发送数据长度
	I2C4->CR2|=1<<25;      //软件结束方式
	
	I2C4->CR2|=1<<13;         //开始
	while(!(I2C4->ISR&(1<<1)))
	{
		
	}
	
	I2C4->TXDR=Addr_in;
	
	while(!(I2C4->ISR&(1<<1)))
	{
		
	}
	I2C4->TXDR=data;
	while((I2C4->ISR&(1<<6)))
	{
		
	}
}

/******************I2C写多字节，自动结束***************************************
*函数原型:		void Write_Multi_Auto(u8* data,u8 Addr,u8 Addr_in,u8 Length)
*功　　能:	    写入多字节，自动结束模式，结束后发送ACK和STOP
输入	data  写入数据的首地址
		Addr  目标地址
		Addr_in 片内地址
		Length 字节数
*******************************************************************************/
void Write_Multi_Auto(u8* data,u8 Addr,u8 Addr_in,u8 Length)
{
	u8 count;
	if(Addr>127)
	{
		Master_Communication_Init_10(Addr,0);
		I2C4->CR2&=~(1<<12);
		
	}
	else Master_Communication_Init(Addr,0);
	I2C4->CR2|=(Length+1)<<16;//发送数据长度
	I2C4->CR2|=1<<25;      //软件结束方式
	
	I2C4->CR2|=1<<13;         //开始
	
	while(!(I2C4->ISR&(1<<1)))
	{
		
	}
	
	I2C4->TXDR=Addr_in;
	
	for(count=0;count<Length;count++)	
	{
		while(!(I2C4->ISR&(1<<1)))
		{
		
		}
		I2C4->TXDR=data[count];
	}
	while((I2C4->ISR&(1<<6)))
	{
		
	}
}

/******************I2C读多字节，自动结束***************************************
*函数原型:		u8 Read_Multi_Auto(u8* data,u8 Addr,u8 Length)
*功　　能:	    读取多字节，自动结束模式，结束后发送NACK和STOP
输入	data  读取数据的首地址
		Addr  目标地址
		Length 字节数
返回    读取数据长度
*******************************************************************************/
u8 Read_Multi_Auto(u8* data,u8 Addr,u8 Length)
{
	u8 count;
	if(Addr>127)
	{
		Master_Communication_Init_10((0x500<<2)+Addr,1);
		I2C4->CR2&=~(1<<12);
		
	}
	else Master_Communication_Init(Addr,1);
	I2C4->CR2|=Length<<16;//接收数据长度
	I2C4->CR2|=1<<25;      //自动结束方式
	
	//I2C2->CR2|=1<<14;          //停止位置1
	I2C4->CR2|=1<<13;         //开始
	
	for(count=0;count<Length;count++)	
	{
		while(!(I2C4->ISR&(1<<2)))
		{
		
		}
		data[count]=I2C4->RXDR;
	}
	
	while((I2C4->ISR&(1<<6)))
	{
		
	}
	return count;
}

/******************I2C读多字节，自动结束，应用DMA***************************************
*函数原型:		u8 Read_Multi_Auto_DMA(u8* data,u8 Addr,u8 Length)
*功　　能:	    读取多字节，自动结束模式，结束后发送NACK和STOP
输入	data  读取数据的首地址
		Addr  目标地址
		Length 字节数
返回    读取数据长度
*******************************************************************************/
u8 Read_Multi_Auto_DMA(u8* data,u16 Addr,u8 Length)
{
	u8 count;
	
	I2C4->CR1&=~(1<<0);    //PE位清零，各配置位复位
	delay_us(30);
	I2C4->CR1|=1<<0;       //PE为1，启用IIC2
	
	I2C4->CR2&=~(1<<13);   //START位停止
	
	
	I2C4->CR2&=~(0x7FF);    //地址位和方向位清零
	I2C4->CR2|=1<<11;   //ADD10=1，在10位寻址模式下
	I2C4->CR2&=~(1<<12);   //HEAD10R=0
	
	I2C4->CR2|=0x234;    //地址设置
	I2C4->CR2|=1<<10;    //传输方向设置
	
	
	I2C4->CR2|=Length<<16;//接收数据长度
	I2C4->CR2|=1<<25;      //自动结束方式
	
	//I2C4->CR1|=1<<15;     //DMA接收使能
	
	//I2C2->CR2|=1<<14;          //停止位置1
	I2C4->CR2|=1<<13;         //开始
	
	//IIC中应用DMA只有数据走DMA流，地址类的还需要进行发送，下面代码是没有DMA情况下需要加的循环等待读取。
	for(count=0;count<Length;count++)	
	{
		while(!(I2C4->ISR&(1<<2)))
		{
		
		}
		data[count]=I2C4->RXDR;
	}
	
	while((I2C4->ISR&(1<<6)))
	{
		
	}
	return count;
}


short CharToShort(unsigned char cData[])
{
	return ((short)cData[1]<<8)|cData[0];
}
