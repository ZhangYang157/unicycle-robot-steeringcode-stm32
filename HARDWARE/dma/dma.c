#include "dma.h"																	   	  
#include "delay.h"

//DMAx�ĸ�ͨ������
//����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
//�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7
//chx:DMAͨ��ѡ��,��Χ:0~7
//par:�����ַ
//mar:�洢����ַ
//ndtr:���ݴ����� 
void MYDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx,u32 par,u32 mar,u16 ndtr,u8 dr)
{ 
	DMA_TypeDef *DMAx;
	u8 streamx;
	if((u32)DMA_Streamx>(u32)DMA2)//�õ���ǰstream������DMA2����DMA1
	{
		DMAx=DMA2;
		RCC->AHB1ENR|=1<<22;//DMA2ʱ��ʹ�� 
	}else 
	{
		DMAx=DMA1; 
 		RCC->AHB1ENR|=1<<21;//DMA1ʱ��ʹ�� 
	}
	while(DMA_Streamx->CR&0X01);//�ȴ�DMA������ 
	streamx=(((u32)DMA_Streamx-(u32)DMAx)-0X10)/0X18;		//�õ�streamͨ����
 	if(streamx>=6)DMAx->HIFCR|=0X3D<<(6*(streamx-6)+16);	//���֮ǰ��stream�ϵ������жϱ�־
	else if(streamx>=4)DMAx->HIFCR|=0X3D<<6*(streamx-4);    //���֮ǰ��stream�ϵ������жϱ�־
	else if(streamx>=2)DMAx->LIFCR|=0X3D<<(6*(streamx-2)+16);//���֮ǰ��stream�ϵ������жϱ�־
	else DMAx->LIFCR|=0X3D<<6*streamx;						//���֮ǰ��stream�ϵ������жϱ�־
	
	DMA_Streamx->PAR=par;		//DMA�����ַ
	DMA_Streamx->M0AR=mar;		//DMA �洢��0��ַ
	DMA_Streamx->NDTR=ndtr;		//DMA �洢��0��ַ
	DMA_Streamx->CR=0;			//��ȫ����λCR�Ĵ���ֵ 
	
	DMA_Streamx->CR|=dr<<6;		//�洢��������ģʽ
	DMA_Streamx->CR|=0<<8;		//��ѭ��ģʽ(��ʹ����ͨģʽ)    0Ϊ��ѭ����1Ϊѭ��ģʽ
	DMA_Streamx->CR|=0<<9;		//���������ģʽ
	DMA_Streamx->CR|=1<<10;		//�洢������ģʽ
	DMA_Streamx->CR|=0<<11;		//�������ݳ���:8λ
	DMA_Streamx->CR|=0<<13;		//�洢�����ݳ���:8λ
	DMA_Streamx->CR|=1<<16;		//�е����ȼ�
	DMA_Streamx->CR|=0<<21;		//����ͻ�����δ���
	DMA_Streamx->CR|=0<<23;		//�洢��ͻ�����δ���
	DMA_Streamx->CR|=(u32)chx<<25;//ͨ��ѡ��
	DMA_Streamx->FCR=0X21;	//FIFO���ƼĴ���
} 

//����һ��DMA��������
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7 
//ndtr:���ݴ�����  
void DMA_Send_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr)
{
	DMA_Streamx->CR&=~(1<<0); 	//�ر�DMA���� 
	while(DMA_Streamx->CR&0X1);	//ȷ��DMA���Ա����� 
	DMA_Streamx->NDTR=ndtr;		//DMA Ҫ������������Ŀ 
	DMA_Streamx->CR|=1<<0;		//����DMA����
}



void MYDMA_Config_IIC(DMA_Stream_TypeDef *DMA_Streamx,u8 chx,u32 par,u32 mar,u16 ndtr,u8 dr)
{ 
	DMA_TypeDef *DMAx;
	u8 streamx;
	if((u32)DMA_Streamx>(u32)DMA2)//�õ���ǰstream������DMA2����DMA1
	{
		DMAx=DMA2;
		RCC->AHB1ENR|=1<<22;//DMA2ʱ��ʹ�� 
	}else 
	{
		DMAx=DMA1; 
 		RCC->AHB1ENR|=1<<21;//DMA1ʱ��ʹ�� 
	}
	while(DMA_Streamx->CR&0X01);//�ȴ�DMA������ 
	streamx=(((u32)DMA_Streamx-(u32)DMAx)-0X10)/0X18;		//�õ�streamͨ����
 	if(streamx>=6)DMAx->HIFCR|=0X3D<<(6*(streamx-6)+16);	//���֮ǰ��stream�ϵ������жϱ�־
	else if(streamx>=4)DMAx->HIFCR|=0X3D<<6*(streamx-4);    //���֮ǰ��stream�ϵ������жϱ�־
	else if(streamx>=2)DMAx->LIFCR|=0X3D<<(6*(streamx-2)+16);//���֮ǰ��stream�ϵ������жϱ�־
	else DMAx->LIFCR|=0X3D<<6*streamx;						//���֮ǰ��stream�ϵ������жϱ�־
	
	DMA_Streamx->PAR=par;		//DMA�����ַ
	DMA_Streamx->M0AR=mar;		//DMA �洢��0��ַ
	DMA_Streamx->NDTR=ndtr;		//DMA �洢��0��ַ
	DMA_Streamx->CR=0;			//��ȫ����λCR�Ĵ���ֵ 
	
	DMA_Streamx->CR|=dr<<6;		//1Ϊ�洢��������ģʽ��0Ϊ���赽�洢��
	DMA_Streamx->CR|=0<<8;		//��ѭ��ģʽ(��ʹ����ͨģʽ)
	DMA_Streamx->CR|=0<<9;		//���������ģʽ
	DMA_Streamx->CR|=1<<10;		//�洢������ģʽ
	DMA_Streamx->CR|=0<<11;		//�������ݳ���:8λ
	DMA_Streamx->CR|=0<<13;		//�洢�����ݳ���:8λ
	DMA_Streamx->CR|=2<<16;		//�ߵ����ȼ�
	DMA_Streamx->CR|=0<<21;		//����ͻ�����δ���
	DMA_Streamx->CR|=0<<23;		//�洢��ͻ�����δ���
	//DMA_Streamx->CR|=1<<4;      //��������ж�ʹ��
	DMA_Streamx->CR|=(u32)chx<<25;//ͨ��ѡ��
	DMA_Streamx->FCR=0X21;	//FIFO���ƼĴ���
	
	
	MY_NVIC_Init(1,3,DMA1_Stream2_IRQn,2);     //DMA1_S2���ж����ȼ����ã���ռ1�������ȼ�3����2
} 

//Ӳ��IIC��DMAͬʱ��ʼ����marΪ�洢���ĵ�ַ�������ָ���ַ��
void IIC_DMA_INIT(u32 mar)
{
	u8 count=0;
	
	//1 DMA����
	//1.1 DMAʱ�ӡ����ȵ�����
	RCC->AHB1ENR|=1<<21;                         //DMA1ʱ��ʹ�� 
	while(DMA1_Stream2->CR&0x01);                //�ȴ�DMA�ܹ�����
	DMA1->LIFCR|=0x3D<<16;                       //���stream2�ϵ������жϱ�־
	DMA1_Stream2->PAR=(u32)&I2C4->RXDR;          //DMA�������ַ
	DMA1_Stream2->M0AR=mar;                      //DMA�Ĵ洢��0��ַ
	//DMA1_Stream2->M1AR=mar;                      //DMA�Ĵ洢��1��ַ
	DMA1_Stream2->NDTR|=24;                       //DMA�������ݳ���
	//1.2 CR���ƼĴ�������
	DMA1_Stream2->CR=0;
	//DMA1_Stream2->CR|=1<<5;                      //1Ϊ����������������0ΪDMA����������
	DMA1_Stream2->CR&=~(1<<6);                   //�������ݴ��䷽��00Ϊ�����赽�洢��
	DMA1_Stream2->CR|=0<<8;                      //ѭ��ģʽ���ã�1Ϊʹ��ѭ����0Ϊ��ֹѭ��
	DMA1_Stream2->CR|=0<<9;                      //�����ַ����ģʽ���ã�1λÿ�����ݴ���������ַָ�����������ΪPSIZE��CR��11��12����0λ�����ַָ��̶�
	DMA1_Stream2->CR&=~(1<<11);                  //�������ݴ�СPSIZE��00Ϊ�ֽڣ�8λ����01Ϊ���֣�16λ����10Ϊ�֣�32λ��
	DMA1_Stream2->CR|=1<<10;                     //�洢����ַ����ģʽ���ã�1Ϊÿ�����ݴ���󣬴洢����ַָ�����������ΪMSIZE��CR��13��14����0Ϊ��ַָ��̶�
	DMA1_Stream2->CR&=~(1<<13);                  //�洢�����ݴ�СMSIZE��00Ϊ�ֽڣ�8λ����01Ϊ���֣�16λ����10Ϊ�֣�32λ��
	DMA1_Stream2->CR|=3<<16;                     //���ȼ����ã�00Ϊ�ͣ�01Ϊ�У�10Ϊ��
	DMA1_Stream2->CR&=~(1<<21);                  //����ͻ���������ã�00λ���δ���
	DMA1_Stream2->CR&=~(1<<23);                  //�洢��ͻ���������ã�00λ���δ���
	DMA1_Stream2->CR|=(u32)2<<25;                //ͨ��ѡ������Ϊͨ��2
	//1.3 FIFO����
	//DMA1_Stream2->FCR|=4<<3;                     //FIFO״̬���ã�100��FIFOΪ��
	DMA1_Stream2->FCR|=1<<5;                     //FIFOΪ��
	DMA1_Stream2->FCR&=~(1<<2);                  //ֱ��ģʽ���ã�0Ϊʹ��ֱ��ģʽ��1Ϊ��ֱֹ��ģʽ
	DMA1_Stream2->FCR|=1<<0;                     //FIFO��ֵѡ��01λ������1/2������FIFO��4��32λ��4���֣�
	
	//2 IICӲ������
	I2C4->CR1&=~(1<<0);                          //PEλ���㣬����IIC4
	while(I2C4->CR1&0x01);
	//2.1 RCCʱ������
	RCC->AHB1ENR|=1<<1;                          //GPIOBʱ��ʹ��
	GPIO_Set(GPIOB,PIN8,GPIO_MODE_AF,GPIO_OTYPE_OD,GPIO_SPEED_50M,GPIO_PUPD_PU);//����B8���ù��ܣ�����
	GPIO_Set(GPIOB,PIN9,GPIO_MODE_AF,GPIO_OTYPE_OD,GPIO_SPEED_50M,GPIO_PUPD_PU);//����B9���ù��ܣ�����
	GPIO_AF_Set(GPIOB,8,1);                      //B8Ӧ�ø��ù���1
	GPIO_AF_Set(GPIOB,9,1);                      //B9Ӧ�ø��ù���1
	RCC->APB1ENR|=1<<24;                         //IIC4ʹ��
	RCC->APB1RSTR|=1<<24;                        //IIC4��λ
	RCC->APB1RSTR&=~(1<<24);                     //IIC4��λ
	//2.2 �����˲�����
	I2C4->CR1&=~(1<<12);                         //ģ���˲���ʹ��
	I2C4->CR1|=0x00<<8;                          //���������˲���
	//2.3ʱ������
	I2C4->TIMINGR=0x10830D3C;                    //����ģʽ��400KHz,I2Cʱ��Ƶ��54000KHz������ʱ��200ns���½�ʱ��200ns.��0x10830D3C����0x10830D25
	//2.4 IIC���ƼĴ�������
	//I2C2->CR1|=1<<2;                             //ʹ��RX�жϣ�RXNE
	//I2C2->CR1|=1<<1;                             //ʹ��TX�жϣ�TXIS
	//I2C2->CR1|=1<<15;                            //ʹ��DMA����
	//I2C2->CR1|=1<<14;                            //ʹ��DMA����
	//I2C2->CR1|=1<<7;                             //ʹ�ܴ����ж�
	//I2C2->CR1|=1<<6;                             //ʹ�ܴ�������ж�
	//I2C2->CR1|=1<<5;                             //ʹ��ֹͣλ����ж�
	//I2C2->CR1|=1<<4;                             //ʹ�ܷ�Ӧ���ж�
	I2C4->CR1|=1<<15;                              //DMA����ʹ�� RXNE=1
	//I2C4->CR1|=1<<14;                              //DMA����ʹ�� TXNE=1
	I2C4->CR1|=1<<0;                               //PEΪ1������IIC4

}

void IIC_DMA_READ()
{
	//DMA����
	DMA1_Stream2->CR&=~(1<<0);                   //ENλΪ0����ֹDMA
	while(DMA1_Stream2->CR&0x01);                //�ȴ�DMA�ܹ�����
	DMA1_Stream2->NDTR|=24;                       //DMA�������ݳ���
	DMA1_Stream2->CR|=1<<0;                      //ENλΪ1������DMA
	
	//IIC��ȡ����
	I2C4->CR1&=~(1<<0);    //PEλ���㣬������λ��λ
	delay_us(30);
	I2C4->CR1|=1<<0;       //PEΪ1������IIC2
	I2C4->CR2&=~(1<<13);   //STARTλֹͣ
	I2C4->CR2&=~(0x7FF);    //��ַλ�ͷ���λ����
	I2C4->CR2|=1<<11;   //ADD10=1����10λѰַģʽ��
	I2C4->CR2&=~(1<<12);   //HEAD10R=0
	I2C4->CR2|=0x234;    //��ַ����
	I2C4->CR2|=1<<10;    //���䷽������
	
	I2C4->CR2|=24<<16;//�������ݳ���
	I2C4->CR2|=1<<25;      //�Զ�������ʽ
	//I2C4->CR2|=1<<24;      //����ģʽNBYES��Ϊ1ʱ�������趨���Ⱥ�����ȴ�
	I2C4->CR1|=1<<15;     //DMA����ʹ��
	//SCB_CleanDCache();
	I2C4->CR2|=1<<13;         //��ʼ
	
	/*//7λѰַ��ʽ����DMA��ȡ
	//Write_Byte_Soft(0x34,0x7A);
	I2C4->CR1&=~(1<<0);    //PEλ���㣬������λ��λ
	delay_us(30);
	I2C4->CR1|=1<<0;       //PEΪ1������IIC4
	
	I2C4->CR2&=~(1<<13);   //STARTλֹͣ
	I2C4->CR2&=~(1<<11);   //ADD10=0����7λѰַģʽ��
	

	I2C4->CR2&=~(0xFF07FF);    //��ַλ,����λ�����ݳ�������
	I2C4->CR2|=0x7A<<1;    //��ַ����
	I2C4->CR2|=1<<10;    //���䷽������
	I2C4->CR2|=24<<16;//�������ݳ���
	I2C4->CR2|=1<<25;      //�Զ�������ʽ
	
	//I2C2->CR2|=1<<14;          //ֹͣλ��1
	I2C4->CR2|=1<<13;         //��ʼ*/

}
/*
void DMA1_Stream2_IRQHandler(void)
{
	if(DMA1->LISR &(1<<21))
				{
					DMA1->LIFCR|=1<<21;
				}
				I2C4->CR2|=1<<14;         //����ֹͣ

				delay_ms(20);
				
				I2C4->CR1&=~(1<<0);    //PEλ���㣬������λ��λ
				delay_us(30);
				I2C4->CR1|=1<<0;       //PEΪ1������IIC2
				//I2C4->CR2&=~(0x7FF);    //��ַλ�ͷ���λ����
				I2C4->CR2|=1<<11;   //ADD10=1����10λѰַģʽ��
				I2C4->CR1|=1<<15;     //DMA����ʹ��
				I2C4->CR2|=0x234;    //��ַ����
				I2C4->CR2|=1<<10;    //���䷽������
				I2C4->CR2|=24<<16;//�������ݳ���
				
				
				DMA_Send_Enable(DMA1_Stream2,24);
				I2C4->CR2|=1<<13;         //��ʼ
					//I2C4->CR2|=24<<16;//�������ݳ���
				//JY901_I2C_Read_Bytes_DMA(datatemp,0x234,24);
}*/

