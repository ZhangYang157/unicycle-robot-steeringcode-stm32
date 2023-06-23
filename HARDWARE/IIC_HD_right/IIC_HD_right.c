#include "IIC_HD_right.h"
#include "sys.h"
#include "delay.h"

//IIC_HD��ʼ��

void IIC_HD_Init_R(void)
{
	I2C1->CR1&=~(1<<0);    //PEλ���㣬����IIC4
	
	//RCCʱ������
	RCC->AHB1ENR|=1<<1;    //GPIOBʱ��ʹ��
	GPIO_Set(GPIOB,PIN6,GPIO_MODE_AF,GPIO_OTYPE_OD,GPIO_SPEED_50M,GPIO_PUPD_PU);//����B6���ù��ܣ�����
	GPIO_Set(GPIOB,PIN7,GPIO_MODE_AF,GPIO_OTYPE_OD,GPIO_SPEED_50M,GPIO_PUPD_PU);//����B7���ù��ܣ�����
	GPIO_AF_Set(GPIOB,6,4);//B6Ӧ�ø��ù���4
	GPIO_AF_Set(GPIOB,7,4);//B7Ӧ�ø��ù���4
	RCC->APB1ENR|=1<<21;   //IIC1ʹ��
	RCC->APB1RSTR|=1<<21;  //IIC1��λ
	RCC->APB1RSTR&=~(1<<21);  //IIC1��λ
	
	//�����˲�����
	I2C1->CR1&=~(1<<12);   //ģ���˲���ʹ��
	I2C1->CR1|=0x00<<8;    //���������˲���
	
	//ʱ������
	I2C1->TIMINGR=0x10830D3C;//����ģʽ��400KHz,I2Cʱ��Ƶ��54000KHz������ʱ��200ns���½�ʱ��200ns
	
	//I2C2->CR1|=1<<2;       //ʹ��RX�жϣ�RXNE
	//I2C2->CR1|=1<<1;       //ʹ��TX�жϣ�TXIS
	//I2C2->CR1|=1<<15;       //ʹ��DMA����
	//I2C2->CR1|=1<<14;       //ʹ��DMA����
	//I2C2->CR1|=1<<7;       //ʹ�ܴ����ж�
	//I2C2->CR1|=1<<6;       //ʹ�ܴ�������ж�
	//I2C2->CR1|=1<<5;       //ʹ��ֹͣλ����ж�
	//I2C2->CR1|=1<<4;       //ʹ�ܷ�Ӧ���ж�

	I2C1->CR1|=1<<0;       //PEΪ1������IIC4
}

void Master_Communication_Init_R(u8 Addr,u8 WRN)
{
	I2C1->CR1&=~(1<<0);    //PEλ���㣬������λ��λ
	delay_us(30);
	I2C1->CR1|=1<<0;       //PEΪ1������IIC1
	
	I2C1->CR2&=~(1<<13);   //STARTλֹͣ
	I2C1->CR2&=~(1<<11);   //ADD10=0����7λѰַģʽ��
	

	I2C1->CR2&=~(0xFF07FF);    //��ַλ,����λ�����ݳ�������
	I2C1->CR2|=Addr<<1;    //��ַ����
	I2C1->CR2|=WRN<<10;    //���䷽������	
}

void Master_Communication_Init_10_R(u16 Addr,u8 WRN)
{
	I2C1->CR1&=~(1<<0);    //PEλ���㣬������λ��λ
	delay_us(30);
	I2C1->CR1|=1<<0;       //PEΪ1������IIC1
	
	I2C1->CR2&=~(1<<13);   //STARTλֹͣ
	I2C1->CR2|=1<<11;   //ADD10=1����10λѰַģʽ��
	
	I2C1->CR2&=~(0x7FF);    //��ַλ�ͷ���λ����
	I2C1->CR2|=Addr<<0;    //��ַ����
	I2C1->CR2|=WRN<<10;    //���䷽������	
}

u8 Read_Byte_Auto_R(u8 Addr)
{
	u8 data;
	if(Addr>127)
	{
		Master_Communication_Init_10_R((0x500<<2)+Addr,1);
		I2C1->CR2&=~(1<<12);
		
	}
	else Master_Communication_Init_R(Addr,1);
	I2C1->CR2|=1<<16;//�������ݳ���
	I2C1->CR2|=1<<25;      //�Զ�������ʽ
	
	//I2C2->CR2|=1<<14;          //ֹͣλ��1
	I2C1->CR2|=1<<13;         //��ʼ
	while(!(I2C1->ISR&(1<<2)))
	{
		
	}
	data=I2C1->RXDR;
	
	while((I2C1->ISR&(1<<6)))
	{
		
	}
	
	return data;
}

u8 Read_Byte_Soft_R(u8 Addr)
{
	u8 data;
	if(Addr>127)
	{
		Master_Communication_Init_10_R(Addr,1);
		I2C4->CR2&=~(1<<12);
		
	}
	else Master_Communication_Init_R(Addr,1);
	I2C1->CR2|=1<<16;//�������ݳ���
	I2C1->CR2&=~(1<<25);      //���������ʽ
	
	//I2C2->CR2|=1<<14;          //ֹͣλ��1
	I2C1->CR2|=1<<13;         //��ʼ
	while(!(I2C1->ISR&(1<<2)))
	{
		
	}
	data=I2C1->RXDR;
	
	while(!(I2C1->ISR&(1<<6)))
	{
		
	}
	
	return data;
}

void Write_Byte_Auto_R(u8 data,u8 Addr)
{
	if(Addr>127)
	{
		Master_Communication_Init_10_R(Addr,0);
		I2C1->CR2&=~(1<<12);
		
	}
	else Master_Communication_Init_R(Addr,0);
	I2C1->CR2|=1<<16;//�������ݳ���
	I2C1->CR2|=1<<25;      //�Զ�������ʽ
	
	I2C1->CR2|=1<<13;         //��ʼ
	while(!(I2C1->ISR&(1<<1)))
	{
		
	}
	I2C1->TXDR=data;
	while((I2C1->ISR&(1<<6)))
	{
		
	}
}

void Write_Byte_Soft_R(u8 data,u8 Addr)
{
	if(Addr>127)
	{
		Master_Communication_Init_10_R(Addr,0);
		I2C1->CR2&=~(1<<12);
		
	}
	else Master_Communication_Init_R(Addr,0);
	I2C1->CR2|=1<<16;//�������ݳ���
	I2C1->CR2&=~(1<<25);      //���������ʽ
	
	I2C1->CR2|=1<<13;         //��ʼ
	while(!(I2C1->ISR&(1<<1)))
	{
		
	}
	I2C1->TXDR=data;
	while(!(I2C1->ISR&(1<<6)))
	{

	}
}

void Write_Long_Auto_R(u8 data,u8 Addr,u8 Addr_in)
{
	if(Addr>127)
	{
		Master_Communication_Init_10_R(Addr,0);
		I2C1->CR2&=~(1<<12);
		
	}
	else Master_Communication_Init_R(Addr,0);
	I2C1->CR2|=2<<16;//�������ݳ���
	I2C1->CR2|=1<<25;      //���������ʽ
	
	I2C1->CR2|=1<<13;         //��ʼ
	while(!(I2C1->ISR&(1<<1)))
	{
		
	}
	
	I2C1->TXDR=Addr_in;
	
	while(!(I2C1->ISR&(1<<1)))
	{
		
	}
	I2C1->TXDR=data;
	while((I2C1->ISR&(1<<6)))
	{
		
	}
}

/******************I2Cд���ֽڣ��Զ�����***************************************
*����ԭ��:		void Write_Multi_Auto(u8* data,u8 Addr,u8 Addr_in,u8 Length)
*��������:	    д����ֽڣ��Զ�����ģʽ����������ACK��STOP
����	data  д�����ݵ��׵�ַ
		Addr  Ŀ���ַ
		Addr_in Ƭ�ڵ�ַ
		Length �ֽ���
*******************************************************************************/
void Write_Multi_Auto_R(u8* data,u8 Addr,u8 Addr_in,u8 Length)
{
	u8 count;
	if(Addr>127)
	{
		Master_Communication_Init_10_R(Addr,0);
		I2C1->CR2&=~(1<<12);
		
	}
	else Master_Communication_Init_R(Addr,0);
	I2C1->CR2|=(Length+1)<<16;//�������ݳ���
	I2C1->CR2|=1<<25;      //���������ʽ
	
	I2C1->CR2|=1<<13;         //��ʼ
	
	while(!(I2C1->ISR&(1<<1)))
	{
		
	}
	
	I2C1->TXDR=Addr_in;
	
	for(count=0;count<Length;count++)	
	{
		while(!(I2C1->ISR&(1<<1)))
		{
		
		}
		I2C1->TXDR=data[count];
	}
	while((I2C1->ISR&(1<<6)))
	{
		
	}
}

/******************I2C�����ֽڣ��Զ�����***************************************
*����ԭ��:		u8 Read_Multi_Auto(u8* data,u8 Addr,u8 Length)
*��������:	    ��ȡ���ֽڣ��Զ�����ģʽ����������NACK��STOP
����	data  ��ȡ���ݵ��׵�ַ
		Addr  Ŀ���ַ
		Length �ֽ���
����    ��ȡ���ݳ���
*******************************************************************************/
u8 Read_Multi_Auto_R(u8* data,u8 Addr,u8 Length)
{
	u8 count;
	if(Addr>127)
	{
		Master_Communication_Init_10_R((0x500<<2)+Addr,1);
		I2C1->CR2&=~(1<<12);
		
	}
	else Master_Communication_Init_R(Addr,1);
	I2C1->CR2|=Length<<16;//�������ݳ���
	I2C1->CR2|=1<<25;      //�Զ�������ʽ
	
	//I2C2->CR2|=1<<14;          //ֹͣλ��1
	I2C1->CR2|=1<<13;         //��ʼ
	
	for(count=0;count<Length;count++)	
	{
		while(!(I2C1->ISR&(1<<2)))
		{
		
		}
		data[count]=I2C1->RXDR;
	}
	
	while((I2C4->ISR&(1<<6)))
	{
		
	}
	return count;
}

