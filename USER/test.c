#include "sys.h"
#include "delay.h" 
#include "usart.h"  
#include "procession.h" 
#include "gyro.h"
#include "encoder.h"
#include "JY901.h"
#include "LED.h"
#include "math.h"
#include "dma.h"
#include "IIC_HD.h"
#include "ground.h"
#include "time_interrpt.h"
#include "adaptive_controller.h"

//#define    groundinitPWM          249
#define    ground_KP              42    //24(2020.7.29)     42
#define    ground_KD              6   //0.6(2020.7.29)      8
#define    ground_KI              0   //0.6(2020.7.29)      
#define    groundspeed_KP         -0.06    //               -0.06


#define    precession_KP          6 //4(roll)  0.01(aacx)    16(2020.7.29)
#define    precession_KD          5.3  //1                     10(2020.7.29)
#define    precession_KI          0     //                0.003(2020.7.29)
#define    precessiondegree_KP    -0.24   //0.0 2

#define    pi                     3.14
#define    dt                     0.005
#define    k_adaptive_pitch       7
#define    k_adaptive_roll        5

union 
{
	float angle;
	u8 a[4];
} printangle;

double AngleXFiler=0,AngleYFiler=0;

	int i=0,j=0,num_time=0;
	u8 datatemp[24],datatemp_g[6],datatemp_r[24],datatemp_r_g[6];
	u8 printa[40]; 
	char roll_mark;
	float pitch,roll,yaw; 		                              //ŷ����
	float roll_set=0,pitch_set=0,yaw_set=0;                                           //�趨ŷ����
	float pitch_filter,roll_filter;                           //�����˲���ĽǶ�ֵ
	float angle_precession=0,angle_ground=0,angle_ground_d=0,old_angle_ground=0,angle_ground_a=0,old_angle_ground_d=0;
	float roll_uart;
	float pitch_r,roll_r,yaw_r;
	float pitch_old,roll_old;
	float aacx,aacy,aacz,ax,ay,az;		                              //���ٶȴ�����ԭʼ����
	float gyrox,gyroy,gyroz,gyrox_old;	                      //������ԭʼ���� 
	float gyrox_r,gyroy_r,gyroz_r,gyrox_old_r;
	float Sum_Roll=0,Sum_Pitch=0;                                         //����ǻ�����ֵ
	short precessionPWM,groundPWM,groundPWMold,groundPWM_current;               //������PWM�ź�
	int encoder=0,encoder_ground=0;
	float r1_pitch,r2_pitch,r3_pitch,r_pitch,e_pitch_d;
	float a1_est_pitch=42,a2_est_pitch=0,b_est_pitch=5;
	float r1_roll,r2_roll,r3_roll,r_roll,e_roll_d;
	float a_est_roll=6,b_est_roll=3;
	float cos_alpha;
	//��ģ���Ʋ���
	float slide_surface=0;
	short sng=0;
	
	//������ֵ��
	short a_adapt_roll=0,b_adapt_roll=0;
	float a_adapt_roll_f=0,b_adapt_roll_f=0;
	
	short a_adapt_pitch=0,b_adapt_pitch=0;
	float a_adapt_pitch_f=0,b_adapt_pitch_f=0;
	


int main(void)
{   
	
	Stm32_Clock_Init(432,25,2,9);                             //����ʱ��,216Mhz
    delay_init(216);			                              //��ʱ��ʼ��  
	LED_Init();
	TIM2_CH1_Cap_Init(0XFFFF,108-1);                          //�����Ƕȱ�����TIM2
	TIM5_CH1_Cap_Init(0XFFFF,108-1);                          //���ֽǶȱ�����TIM5
	uart_init(54,115200);	                                  //���ڳ�ʼ��Ϊ115200
	atk_8266_sta_zhang();                                     //�������ӣ�atk8266ģ�飩
	//JY901_Init();
	
	//DMA����
	//1.IIC��DMA������IIC��ʼ������ע�͵�JY901_Init();
	/*MYDMA_Config_IIC(DMA1_Stream2,2,(u32)&I2C4->RXDR,(u32)datatemp,25,0);
	DMA_Send_Enable(DMA1_Stream2,25);
	IIC_HD_Init_DMA(0x234,24);*/
	IIC_DMA_INIT((u32)datatemp);                              //�µ�IIC��DMA��ʼ����
	IIC_DMA_READ();
	
	//2.���ڵ�DMA����
	MYDMA_Config(DMA1_Stream3,4,(u32)&USART3->TDR,(u32)printa,11,1);//DMA1,STEAM3,CH4,����Ϊ����3,�洢��ΪSendBuff,����Ϊ:SEND_BUF_SIZE.
	USART3->CR3=1<<7;
	DMA_Send_Enable(DMA1_Stream3,11);
	
	//��ʼ����PWM�ź�	
	TIM1_PWM_Init(1000-1,9-1);		                         //�����ǵ�PWM   500,18Ϊ6MHz����Ƶ�ʣ�12KHz��PWM����1000,18Ϊ6MHz����Ƶ�ʣ�6KHz��PWM����1000,9Ϊ12MHz����Ƶ�ʣ�12KHz��PWM����1000,6Ϊ18MHz����Ƶ�ʣ�18KHz��PWM����Ƶ������߱���
	TIM3_PWM_Init(1000-1,216-1);                             //��ת�ǵ�PWM
	//TIM8_PWM_Init(1000-1,9-1);		                         //12Mhz�ļ���Ƶ��,12Khz��PWM. 
	TIM8_PWM_Init(1000-1,18-1);		                         //���ֵ�PWM    36Mhz�ļ���Ƶ��,72Khz��PWM. 
	
	TIM_interrupt_Init(10000-1,10800-1);                     //ʱ���ж�
	
	//������ת����������
	gyrol=500;
	gyror=500;
	delay_ms(3000);
	for(i=0;i<=8;i++)
	{
		gyrol=500+i*50;
		gyror=500+i*50;
		delay_ms(100);
	}
	gyror=gyror-18;               //�Ҳ����ݼ��٣�ʹ2��ת����ͬ
	//gyrol=gyrol+4.5;
	//gyrol=185;//185
	//gyror=185;
	delay_ms(3000);


	//floatתchar�Ͷ���
	/*printa[0]='r';
	printa[1]='o';
	printa[2]='l';
	printa[3]='l';
	printa[8]=0x0d;
	printa[9]=0x0a;*/
	
	
    while(1)
    {
        GPIOA->BSRR=0x200;                                   //ʹ��PA9��Ҳ���ǽ�D12��λ��������ʹ��λ��λ		
		D22(1);                                              //��λD22��ʹ���ֵ��ʹ��
		D24(1);
		
		encoder=(short)TIM2->CNT;                            //�����Ƕȱ�����ֵ
			//dir=TIM2->CR1&0x0010;
			if((encoder>=600)||(encoder<=-600))              //600�������Ƕȹ��󱣻���һ��A�ź�512���壬��2�źţ����趨�����ض��������һ���ź�Ϊ2048
			{
				D12(0);                                      //��������ر�ʹ��
				LED0(0);
			}		
			angle_precession=encoder*0.17578;                //0.17578=360���ȣ�/2048��һȦ�⵽��AB��������������
			
			encoder_ground=(short)TIM5->CNT;                 //���ֱ�������ȡ
			angle_ground=encoder_ground*0.17578;             //����ת���Ƕ�ֵ
			angle_ground_d=(angle_ground-old_angle_ground)/dt;
			old_angle_ground=angle_ground;
			angle_ground_a=(angle_ground_d-old_angle_ground_d)/dt;
			old_angle_ground_d=angle_ground_d;
			
		
			//IIC��DMA��������
			//1.��DMA�д������Ϊ�ڵ㣬��������
			if(DMA1->LISR&(1<<21))	//�ȴ�DMA2_Steam7�������
				{
					SCB_DisableDCache();    //ֻ��ֹͣDcache���ܽ���IIC��DMA���书�ܣ�֮����Dcache�����ٿ���
					DMA1->LIFCR|=1<<21;	//���DMA2_Steam7������ɱ�־
					DMA1->LIFCR|=1<<20;	//���DMA2_Steam7�봫����ɱ�־
					//delay_ms(1000);
					//MYDMA_Config_IIC(DMA1_Stream2,2,(u32)&I2C4->RXDR,(u32)datatemp,24,0);
					//DMA_Send_Enable(DMA1_Stream2,24);
					//IIC_HD_Init_DMA(0x234,24);
					//I2C4->CR1|=1<<15;     //DMA����ʹ�� RXNE=1
					//DMA_Send_Enable(DMA1_Stream2,25);
					//I2C4->CR2|=24<<16;//�������ݳ���
					//JY901_I2C_Read_Bytes_DMA(datatemp,0x234,24);
					IIC_DMA_READ();
		        }
				SCB_EnableDCache();
			//2.��IICֹͣλ�յ�Ϊ�ڵ�����
			/*if((I2C4->ISR&(1<<5)))
			{
				if(DMA1->LISR &(1<<21))
				{
					DMA1->LIFCR|=1<<21;
				}
				I2C4->ICR|=1<<5;   //stopλ����

				
				I2C4->CR1&=~(1<<0);    //PEλ���㣬������λ��λ
				delay_us(30);
				I2C4->CR1|=1<<0;       //PEΪ1������IIC2
				I2C4->CR1|=1<<15;     //DMA����ʹ��
				I2C4->CR2|=0x234;    //��ַ����
				I2C4->CR2|=1<<10;    //���䷽������-
				I2C4->CR2|=24<<16;//�������ݳ���
				
				
				DMA_Send_Enable(DMA1_Stream2,24);
				I2C4->CR2|=1<<13;         //��ʼ-
				
			}*/
			
			
			/*
			//�ȶ�ȡŷ���ǣ��ٶ�ȡ���ٶ�
			JY901_I2C_Read_Bytes(datatemp,0x7A,0x3d,6);
			roll=(float)CharToShort(&datatemp[0])/32768*180;
			pitch=(float)CharToShort(&datatemp[2])/32768*180;
			yaw=(float)CharToShort(&datatemp[4])/32768*180;
			printf("roll��Ϊ%f\n",roll);
			
			
			JY901_I2C_Read_Bytes(&datatemp_g[0],0x7A,0x37,6);
			gyrox=(float)CharToShort(&datatemp_g[0])/32768*2000;
			gyroy=(float)CharToShort(&datatemp_g[2])/32768*2000;
			gyroz=(float)CharToShort(&datatemp_g[4])/32768*2000;
			
			printf("gx��Ϊ%f\n",gyrox);*/
			
			//ͬʱ��ȡȫ������
			//JY901_I2C_Read_Bytes(datatemp,0x7A,0x34,24);      //��ַΪ0x55
			//JY901_I2C_Read_Bytes_DMA(datatemp,0x234,24);
			
			aacx=(float)CharToShort(&datatemp[0])/32768*2;
			aacy=(float)CharToShort(&datatemp[2])/32768*2;
			aacz=(float)CharToShort(&datatemp[4])/32768*2;
			gyrox=(float)CharToShort(&datatemp[6])/32768*2000;
			gyroy=(float)CharToShort(&datatemp[8])/32768*2000;
			gyroz=(float)CharToShort(&datatemp[10])/32768*2000;
			roll=(float)CharToShort(&datatemp[18])/32768*180;
			pitch=(float)CharToShort(&datatemp[20])/32768*180;
			yaw=(float)CharToShort(&datatemp[22])/32768*180;
			ax=(float)atan2(aacx,sqrt(aacy*aacy+aacz*aacz));
			//aacx=(float)asin((double)aacx);
			ax=-ax*180/pi;
			ay=(float)atan2(aacy,sqrt(aacx*aacx+aacz*aacz));
			//aacy=(float)asin((double)aacy);
			ay=ay*180/pi;
			//AngleYFiler=ComplementaryFilterAngleX((double)aacy,(double)gyrox);
			gyrox=(float)LowPassFiler((double)gyrox);
			gyroy=(float)LowPassFiler((double)gyroy);
			
			
			//�ұ����ݽ����ź�
/*			//JY901_I2C_Read_Bytes_R(datatemp_r,0x51,0x37,24);
			gyrox_r=(float)CharToShort(&datatemp_r[0])/32768*2000;
			gyroy_r=(float)CharToShort(&datatemp_r[2])/32768*2000;
			gyroz_r=(float)CharToShort(&datatemp_r[4])/32768*2000;
			roll_r=(float)CharToShort(&datatemp_r[12])/32768*180;
			pitch_r=(float)CharToShort(&datatemp_r[14])/32768*180;
			yaw_r=(float)CharToShort(&datatemp_r[16])/32768*180;*/
			
			if((gyrox-gyrox_old)>=8||(gyrox-gyrox_old)<=-8)       //���˵��Ӳ�
				gyrox=gyrox_old;
			
			//�����˲�
			//ComplementaryFilterAngleX();
			roll_filter=ComplementaryFilterAngleX((double)ay,(double)gyrox); 
			pitch_filter=ComplementaryFilterAngleX((double)ax,(double)gyroy);
			
			roll=roll+1.2;    //5Hz��0.95��0.98��20Hz��0.3��200Hz��1.2-1.3     1.23���Ƕȳ��򲹳���   +1.2
			pitch=pitch-1.5;   //-1.4��ʱ����    -1.8     -1.0
			yaw=yaw+27;       //
						
			
			//DMAд����
			/*if(roll<0)	
			{
				sprintf(printa,"roll%2.2f\n",roll);
			}
				
			else
			{
				roll_mark='+';
				roll_uart=roll;
				sprintf(printa,"roll%c%2.2f\n",roll_mark,roll_uart);
			}*/
			//��ѭ����ʽ����DMA��������
			if(DMA1->LISR&(1<<27))
			{
				DMA1->LIFCR|=1<<27;
				sprintf(printa,"roll:%2.2f;pitch:%2.2f;yaw:%2.2f;an_gr:%2.2f;an_pr:%2.2f;\n",roll,pitch,yaw,angle_ground,angle_precession);//����ǣ������ǣ��˲������ǣ��˲���������
				//sprintf(printa,"roll:%2.2f;a1_pi:%2.2f;a2_pi:%2.2f;b_pit:%2.2f;\n",roll,a1_est_pitch,a2_est_pitch,b_est_pitch);//����ǣ�����a1������a2������b
				//sprintf(printa,"roll:%2.2f;pitch:%2.2f;a1_ro:%2.2f;b1_ro:%2.2f;\n",roll,pitch,a_est_roll,b_est_roll);//����ǣ������ǣ��˲������ǣ��˲���������
				
				//sprintf(printa,"a_roll%2.2fb_roll%2.2fa1_pi%2.2fa2_pi%2.2f\n",a_est_roll,b_est_roll,a1_est_pitch,a2_est_pitch);//���������Ӧϵ��a��b������������Ӧϵ��a1�Ͱ�
				//sprintf(printa,"ground%dalpha%d\n",groundPWM_current,precessionPWM);//���ֵ���PWM���������ٶ�PWM
				USART3->CR3=1<<7;
				j=strlen(printa);
				DMA_Send_Enable(DMA1_Stream3 ,j);
			}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
			
				
			//floatתchar��
			//��ʽ�����ַ��ͣ����ã������ݳ��ȱ仯ʱ��Ч
			//sprintf(printa,"roll%c%2.2f\n",roll_mark,roll_uart);
			//��ʽһ��union�ͣ����ã�����ȡ�����ף�����IEEE754��׼���
			/*printangle.angle=roll;
			for(int s=0;s<4;s++)
			{
				printa[4+s]=printangle.a[3-s];
			}*/
			//��ʽ����float��ַ����Ҳ��IEEE754��׼���
			/*for(int q=0;q<4;q++)
			{
				printa[4+q]=((u8*)(&roll))[3-q];
			}*/				
				
			//�������
			//printf("rollΪ%f,\n",roll);
			//printf("�˲���rollΪ%f,",AngleYFiler);
			//printf("roll_rΪ%f",roll_r);
			//printf("encoderΪ%d",encoder);
			//printf("gx��Ϊ%f\n",gyrox);
			
			gyrox_old=gyrox;
			Sum_Roll=Sum_Roll+roll;
			
			Sum_Pitch=Sum_Pitch+pitch;
			
			//************************ƫ����yaw����*************************
			if((num_time>=800)&&(num_time<850))
			{
				yaw_set=10;
			}
			else
				yaw_set=0;
			
			
			

			//************************�����roll����*************************
			//++++++1.PID����+++++
			//precessionPWM=489+(short)(-precession_KP*roll-precession_KD*gyrox-precession_KI*Sum_Roll-precessiondegree_KP*angle_precession);     //500��18��ʼΪ244��1000��18��ʼΪ494.1000��6��ʼΪ488.1000��9��ʼΪ489�� ����2.5����PD�㷨���¡�
			//precessionPWM=489+(short)(-precession_KP*roll_filter-precession_KD*gyrox-precession_KI*Sum_Roll-precessiondegree_KP*angle_precession);     //Ӧ���˲� 500��18��ʼΪ244��1000��18��ʼΪ494.1000��6��ʼΪ488.1000��9��ʼΪ489��
			//++++++1.����Ӧģ����������2��++++++
			//������
			r1_roll=0;//.001*sin(angle_precession);
			r2_roll=-0.0002*Realize_roll((pitch+0.2*gyroy),angle_ground_a);
			r3_roll=-0.08*(0-angle_precession);
			r_roll=r1_roll+r2_roll+r3_roll;
			
			//��������
			e_roll_d=(0-gyrox);
			a_est_roll=a_est_roll-0.1*e_roll_d*roll*dt;
			b_est_roll=b_est_roll-0.02*e_roll_d*r_roll*cos(angle_precession)*dt;
			
			//������ֵ��
			/*if((num_time>=2000)&&(num_time<2010))
				roll_set=5;
			else
				roll_set=0;*/
			a_adapt_roll_f=k_adaptive_roll*(0-gyrox)-a_est_roll*(roll-0);
			b_adapt_roll_f=b_est_roll*r_roll;
			if((a_adapt_roll_f>0)&&(a_adapt_roll_f<=2))
				a_adapt_roll=2;
			else if((a_adapt_roll_f<0)&&(a_adapt_roll_f>=-2))
				a_adapt_roll=-2;
			else
				a_adapt_roll=(short)a_adapt_roll_f;
			b_adapt_roll=(short)b_adapt_roll_f;
			
			//�������ٶ����
			cos_alpha=cos(angle_precession);
			if(cos_alpha<=0)
				cos_alpha=0.01;
			precessionPWM=489+a_adapt_roll+b_adapt_roll+1*yaw_set;                 //0λ�ø���ϸ����(�£�����)   ����ƫ�����Ʊ���
			//precessionPWM=489+(short)(k_adaptive_roll*(0-gyrox)-a_est_roll*roll)+(short)b_est_roll*r_roll;              ///cos(angle_precession)/cos(angle_precession)
			//++++++1.��ģ��������3��++++++
			//��ģ������
			slide_surface=roll+0.8*gyrox;
			if(slide_surface>=0)
				sng=1;
			else
				sng=-1;
			//�������ٶ����
			//precessionPWM=489-(6*roll+6*gyrox-0.24*angle_precession-0.8*sng);
			
			//++++++2.�����Ǳ߽��趨++++++
			roll_old=roll;
			pitch_old=pitch;
			if(precessionPWM>=630)     //500��18�ļ���Ϊ180��3201
				precessionPWM=630;
			if(precessionPWM<=360)
				precessionPWM=360;
			//++++++3.�������ٶ�PWM����++++++
			PROCESSION_PWM=precessionPWM;
			
			//************************������pitch����*************************
			//++++++1.PID���ƣ�1��++++++
			//�����ٶ�ģʽ
			//groundPWM=488+(short)(-ground_KP*pitch-ground_KD*gyroy-ground_KI*Sum_Pitch-groundspeed_KP*angle_ground);      //1000��9��ʼΪ489
			//groundPWM=489+(short)(-ground_KP*pitch_filter-ground_KD*gyroy);      //1000��9��ʼΪ489
			//���ֵ���ģʽ
			groundPWM_current=(short)(ground_KP*(pitch+0.3)+ground_KD*gyroy+groundspeed_KP*(angle_ground_d-10))+5*yaw_set;
			//++++++1.����Ӧģ����������2��++++++
			//������
			r1_pitch=-0;//.2*sin(num_time*dt*6);
			r2_pitch=0;//*Realize_pitch(roll+20*gyrox,angle_ground_d);
			r3_pitch=0.01*(0-angle_ground_d);
			r_pitch=r1_pitch+r2_pitch+r3_pitch;
			//��������
			e_pitch_d=(0-gyroy);
			a1_est_pitch=a1_est_pitch-0.01*e_pitch_d*pitch*dt;
			a2_est_pitch=a2_est_pitch+0*e_pitch_d*gyroy*dt;  //*fabs(sin(angle_precession))   0.003
			b_est_pitch=b_est_pitch-0.003*e_pitch_d*r_pitch*dt;
			
			//������ֵ��
			a_adapt_pitch_f=-k_adaptive_pitch*(0-gyroy)+a1_est_pitch*pitch+a2_est_pitch*gyroy;      //*sin(angle_precession)
			b_adapt_pitch_f=b_est_pitch*r_pitch;
			if((a_adapt_pitch_f>0)&&(a_adapt_pitch_f<=3))
				a_adapt_pitch=3;
			else if((a_adapt_pitch_f<0)&&(a_adapt_pitch_f>=-3))
				a_adapt_pitch=-3;
			else
				a_adapt_pitch=(short)a_adapt_pitch_f;
			b_adapt_pitch=(short)b_adapt_pitch_f;
			
			//�����������
			//groundPWM_current=a_adapt_pitch+b_adapt_pitch;      //������ֵ�����������Ӧ���ã��£�
			//groundPWM_current=-k_adaptive_pitch*(0-gyroy)+a1_est_pitch*pitch+a2_est_pitch*gyroy*sin(angle_precession)+b_est_pitch*r_pitch;      //*fabs(sin(angle_precession))
			
			//++++++2.���������������趨++++++ 
			/*if(groundPWM_current<=0)
			{
				D24(1);
				groundPWM_current=-groundPWM_current;
			}
			else
				D24(0);*/
			//++++++3.���ֱ߽��趨(����)++++++
			if(groundPWM_current>250)                //380
				groundPWM_current=250;
			if(groundPWM_current<-250)
				groundPWM_current=-250;
			//if(groundPWM_current<200)
				//groundPWM_current=200;
			//++++++4.����PWM����++++++
			//GROUND_PWM=150+groundPWM_current;        //290Ϊ10%PWM   680λ90%PWM��ʹ��&����      143
			GROUND_PWM=500-groundPWM_current;
			groundPWMold=groundPWM;
			
			//******�ٶ�ģʽ�߽��趨
			/*if(groundPWM>780)
				groundPWM=780;
			if(groundPWM<200)
				groundPWM=200;
			GROUND_PWM=groundPWM;
			groundPWMold=groundPWM;*/
			
			num_time++;
			delay_ms(5);
	} 
}

void TIM4_IRQHandler(void)
{
	if(TIM4->SR&0X0001)//����ж�
	{
					    				   				     	    	
	}				   
	TIM4->SR&=~(1<<0);//����жϱ�־λ
}
