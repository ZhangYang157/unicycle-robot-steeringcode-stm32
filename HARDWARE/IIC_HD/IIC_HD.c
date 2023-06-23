#include "IIC_HD.h"
#include "sys.h"
#include "delay.h"

//IIC_HD��ʼ��

void IIC_HD_Init(void)
{
	I2C4->CR1&=~(1<<0);    //PEλ���㣬����IIC4
	
	//RCCʱ������
	RCC->AHB1ENR|=1<<1;    //GPIOBʱ��ʹ��
	GPIO_Set(GPIOB,PIN8,GPIO_MODE_AF,GPIO_OTYPE_OD,GPIO_SPEED_50M,GPIO_PUPD_PU);//����B8���ù��ܣ�����
	GPIO_Set(GPIOB,PIN9,GPIO_MODE_AF,GPIO_OTYPE_OD,GPIO_SPEED_50M,GPIO_PUPD_PU);//����B9���ù��ܣ�����
	GPIO_AF_Set(GPIOB,8,1);//B8Ӧ�ø��ù���1
	GPIO_AF_Set(GPIOB,9,1);//B9Ӧ�ø��ù���1
	RCC->APB1ENR|=1<<24;   //IIC4ʹ��
	RCC->APB1RSTR|=1<<24;  //IIC4��λ
	RCC->APB1RSTR&=~(1<<24);  //IIC4��λ
	
	//�����˲�����
	I2C4->CR1&=~(1<<12);   //ģ���˲���ʹ��
	I2C4->CR1|=0x00<<8;    //���������˲���
	
	//ʱ������
	I2C4->TIMINGR=0x10830D3C;//����ģʽ��400KHz,I2Cʱ��Ƶ��54000KHz������ʱ��200ns���½�ʱ��200ns.��0x10830D3C����0x10830D25
	
	//I2C2->CR1|=1<<2;       //ʹ��RX�жϣ�RXNE
	//I2C2->CR1|=1<<1;       //ʹ��TX�жϣ�TXIS
	//I2C2->CR1|=1<<15;       //ʹ��DMA����
	//I2C2->CR1|=1<<14;       //ʹ��DMA����
	//I2C2->CR1|=1<<7;       //ʹ�ܴ����ж�
	//I2C2->CR1|=1<<6;       //ʹ�ܴ�������ж�
	//I2C2->CR1|=1<<5;       //ʹ��ֹͣλ����ж�
	//I2C2->CR1|=1<<4;       //ʹ�ܷ�Ӧ���ж�

	I2C4->CR1|=1<<0;       //PEΪ1������IIC4
}

void IIC_HD_Init_DMA(u16 Addr,u8 Length)
{
	u8 count=0;
	I2C4->CR1&=~(1<<0);    //PEλ���㣬����IIC4
	
	//RCCʱ������
	RCC->AHB1ENR|=1<<1;    //GPIOBʱ��ʹ��
	GPIO_Set(GPIOB,PIN8,GPIO_MODE_AF,GPIO_OTYPE_OD,GPIO_SPEED_50M,GPIO_PUPD_PU);//����B8���ù��ܣ�����
	GPIO_Set(GPIOB,PIN9,GPIO_MODE_AF,GPIO_OTYPE_OD,GPIO_SPEED_50M,GPIO_PUPD_PU);//����B9���ù��ܣ�����
	GPIO_AF_Set(GPIOB,8,1);//B8Ӧ�ø��ù���1
	GPIO_AF_Set(GPIOB,9,1);//B9Ӧ�ø��ù���1
	RCC->APB1ENR|=1<<24;   //IIC4ʹ��
	RCC->APB1RSTR|=1<<24;  //IIC4��λ
	RCC->APB1RSTR&=~(1<<24);  //IIC4��λ
	
	//�����˲�����
	I2C4->CR1&=~(1<<12);   //ģ���˲���ʹ��
	I2C4->CR1|=0x00<<8;    //���������˲���
	
	//ʱ������
	I2C4->TIMINGR=0x10830D3C;//����ģʽ��400KHz,I2Cʱ��Ƶ��54000KHz������ʱ��200ns���½�ʱ��200ns.��0x10830D3C����0x10830D25
	
	//I2C2->CR1|=1<<2;       //ʹ��RX�жϣ�RXNE
	//I2C2->CR1|=1<<1;       //ʹ��TX�жϣ�TXIS
	//I2C2->CR1|=1<<15;       //ʹ��DMA����
	//I2C2->CR1|=1<<14;       //ʹ��DMA����
	//I2C2->CR1|=1<<7;       //ʹ�ܴ����ж�
	//I2C2->CR1|=1<<6;       //ʹ�ܴ�������ж�
	//I2C2->CR1|=1<<5;       //ʹ��ֹͣλ����ж�
	//I2C2->CR1|=1<<4;       //ʹ�ܷ�Ӧ���ж�

	I2C4->CR1|=1<<0;       //PEΪ1������IIC4
	
	I2C4->CR2&=~(1<<13);   //STARTλֹͣ
	I2C4->CR2=0;	
	//I2C4->CR2&=~(0x7FF);    //��ַλ�ͷ���λ����
	I2C4->CR2|=1<<11;   //ADD10=1����10λѰַģʽ��
	//I2C4->CR2&=~(1<<12);   //HEAD10R=0
	
	I2C4->CR2|=0x234;    //��ַ����
	I2C4->CR2|=1<<10;    //���䷽������
	
	
	I2C4->CR2|=Length<<16;//�������ݳ���
	I2C4->CR2|=1<<25;      //���������ʽ
	
	//I2C4->CR1|=1<<15;     //DMA����ʹ�� RXNE=1
	
	//I2C2->CR2|=1<<14;          //ֹͣλ��1
	I2C4->CR2|=1<<13;         //��ʼ
	
}

void Master_Communication_Init(u8 Addr,u8 WRN)
{
	I2C4->CR1&=~(1<<0);    //PEλ���㣬������λ��λ
	delay_us(30);
	I2C4->CR1|=1<<0;       //PEΪ1������IIC4
	
	I2C4->CR2&=~(1<<13);   //STARTλֹͣ
	I2C4->CR2&=~(1<<11);   //ADD10=0����7λѰַģʽ��
	

	I2C4->CR2&=~(0xFF07FF);    //��ַλ,����λ�����ݳ�������
	I2C4->CR2|=Addr<<1;    //��ַ����
	I2C4->CR2|=WRN<<10;    //���䷽������	
}

void Master_Communication_Init_10(u16 Addr,u8 WRN)
{
	I2C4->CR1&=~(1<<0);    //PEλ���㣬������λ��λ
	delay_us(30);
	I2C4->CR1|=1<<0;       //PEΪ1������IIC2
	
	I2C4->CR2&=~(1<<13);   //STARTλֹͣ
	I2C4->CR2|=1<<11;   //ADD10=1����10λѰַģʽ��
	I2C4->CR2&=(1<<12);   //HEAD10R=0
	
	I2C4->CR2&=~(0xFF07FF);    //��ַλ�ͷ���λ����
	I2C4->CR2|=Addr<<0;    //��ַ����
	I2C4->CR2|=WRN<<10;    //���䷽������	
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
	I2C4->CR2|=1<<16;//�������ݳ���
	I2C4->CR2|=1<<25;      //�Զ�������ʽ
	
	//I2C2->CR2|=1<<14;          //ֹͣλ��1
	I2C4->CR2|=1<<13;         //��ʼ
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
	I2C4->CR2|=1<<16;//�������ݳ���
	I2C4->CR2&=~(1<<25);      //���������ʽ
	
	//I2C2->CR2|=1<<14;          //ֹͣλ��1
	I2C4->CR2|=1<<13;         //��ʼ
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
	I2C4->CR2|=1<<16;//�������ݳ���
	I2C4->CR2|=1<<25;      //�Զ�������ʽ
	
	I2C4->CR2|=1<<13;         //��ʼ
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
	I2C4->CR2|=1<<16;//�������ݳ���
	I2C4->CR2&=~(1<<25);      //���������ʽ
	
	I2C4->CR2|=1<<13;         //��ʼ
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
	I2C4->CR2|=2<<16;//�������ݳ���
	I2C4->CR2|=1<<25;      //���������ʽ
	
	I2C4->CR2|=1<<13;         //��ʼ
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

/******************I2Cд���ֽڣ��Զ�����***************************************
*����ԭ��:		void Write_Multi_Auto(u8* data,u8 Addr,u8 Addr_in,u8 Length)
*��������:	    д����ֽڣ��Զ�����ģʽ����������ACK��STOP
����	data  д�����ݵ��׵�ַ
		Addr  Ŀ���ַ
		Addr_in Ƭ�ڵ�ַ
		Length �ֽ���
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
	I2C4->CR2|=(Length+1)<<16;//�������ݳ���
	I2C4->CR2|=1<<25;      //���������ʽ
	
	I2C4->CR2|=1<<13;         //��ʼ
	
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

/******************I2C�����ֽڣ��Զ�����***************************************
*����ԭ��:		u8 Read_Multi_Auto(u8* data,u8 Addr,u8 Length)
*��������:	    ��ȡ���ֽڣ��Զ�����ģʽ����������NACK��STOP
����	data  ��ȡ���ݵ��׵�ַ
		Addr  Ŀ���ַ
		Length �ֽ���
����    ��ȡ���ݳ���
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
	I2C4->CR2|=Length<<16;//�������ݳ���
	I2C4->CR2|=1<<25;      //�Զ�������ʽ
	
	//I2C2->CR2|=1<<14;          //ֹͣλ��1
	I2C4->CR2|=1<<13;         //��ʼ
	
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

/******************I2C�����ֽڣ��Զ�������Ӧ��DMA***************************************
*����ԭ��:		u8 Read_Multi_Auto_DMA(u8* data,u8 Addr,u8 Length)
*��������:	    ��ȡ���ֽڣ��Զ�����ģʽ����������NACK��STOP
����	data  ��ȡ���ݵ��׵�ַ
		Addr  Ŀ���ַ
		Length �ֽ���
����    ��ȡ���ݳ���
*******************************************************************************/
u8 Read_Multi_Auto_DMA(u8* data,u16 Addr,u8 Length)
{
	u8 count;
	
	I2C4->CR1&=~(1<<0);    //PEλ���㣬������λ��λ
	delay_us(30);
	I2C4->CR1|=1<<0;       //PEΪ1������IIC2
	
	I2C4->CR2&=~(1<<13);   //STARTλֹͣ
	
	
	I2C4->CR2&=~(0x7FF);    //��ַλ�ͷ���λ����
	I2C4->CR2|=1<<11;   //ADD10=1����10λѰַģʽ��
	I2C4->CR2&=~(1<<12);   //HEAD10R=0
	
	I2C4->CR2|=0x234;    //��ַ����
	I2C4->CR2|=1<<10;    //���䷽������
	
	
	I2C4->CR2|=Length<<16;//�������ݳ���
	I2C4->CR2|=1<<25;      //�Զ�������ʽ
	
	//I2C4->CR1|=1<<15;     //DMA����ʹ��
	
	//I2C2->CR2|=1<<14;          //ֹͣλ��1
	I2C4->CR2|=1<<13;         //��ʼ
	
	//IIC��Ӧ��DMAֻ��������DMA������ַ��Ļ���Ҫ���з��ͣ����������û��DMA�������Ҫ�ӵ�ѭ���ȴ���ȡ��
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
