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
	float pitch,roll,yaw; 		                              //欧拉角
	float roll_set=0,pitch_set=0,yaw_set=0;                                           //设定欧拉角
	float pitch_filter,roll_filter;                           //互补滤波后的角度值
	float angle_precession=0,angle_ground=0,angle_ground_d=0,old_angle_ground=0,angle_ground_a=0,old_angle_ground_d=0;
	float roll_uart;
	float pitch_r,roll_r,yaw_r;
	float pitch_old,roll_old;
	float aacx,aacy,aacz,ax,ay,az;		                              //加速度传感器原始数据
	float gyrox,gyroy,gyroz,gyrox_old;	                      //陀螺仪原始数据 
	float gyrox_r,gyroy_r,gyroz_r,gyrox_old_r;
	float Sum_Roll=0,Sum_Pitch=0;                                         //横滚角积分项值
	short precessionPWM,groundPWM,groundPWMold,groundPWM_current;               //进动角PWM信号
	int encoder=0,encoder_ground=0;
	float r1_pitch,r2_pitch,r3_pitch,r_pitch,e_pitch_d;
	float a1_est_pitch=42,a2_est_pitch=0,b_est_pitch=5;
	float r1_roll,r2_roll,r3_roll,r_roll,e_roll_d;
	float a_est_roll=6,b_est_roll=3;
	float cos_alpha;
	//滑模控制参数
	float slide_surface=0;
	short sng=0;
	
	//参数数值化
	short a_adapt_roll=0,b_adapt_roll=0;
	float a_adapt_roll_f=0,b_adapt_roll_f=0;
	
	short a_adapt_pitch=0,b_adapt_pitch=0;
	float a_adapt_pitch_f=0,b_adapt_pitch_f=0;
	


int main(void)
{   
	
	Stm32_Clock_Init(432,25,2,9);                             //设置时钟,216Mhz
    delay_init(216);			                              //延时初始化  
	LED_Init();
	TIM2_CH1_Cap_Init(0XFFFF,108-1);                          //进动角度编码器TIM2
	TIM5_CH1_Cap_Init(0XFFFF,108-1);                          //底轮角度编码器TIM5
	uart_init(54,115200);	                                  //串口初始化为115200
	atk_8266_sta_zhang();                                     //无线连接（atk8266模块）
	//JY901_Init();
	
	//DMA设置
	//1.IIC中DMA设置与IIC初始化，需注释掉JY901_Init();
	/*MYDMA_Config_IIC(DMA1_Stream2,2,(u32)&I2C4->RXDR,(u32)datatemp,25,0);
	DMA_Send_Enable(DMA1_Stream2,25);
	IIC_HD_Init_DMA(0x234,24);*/
	IIC_DMA_INIT((u32)datatemp);                              //新的IIC和DMA初始程序
	IIC_DMA_READ();
	
	//2.串口的DMA设置
	MYDMA_Config(DMA1_Stream3,4,(u32)&USART3->TDR,(u32)printa,11,1);//DMA1,STEAM3,CH4,外设为串口3,存储器为SendBuff,长度为:SEND_BUF_SIZE.
	USART3->CR3=1<<7;
	DMA_Send_Enable(DMA1_Stream3,11);
	
	//初始化各PWM信号	
	TIM1_PWM_Init(1000-1,9-1);		                         //进动角的PWM   500,18为6MHz计数频率，12KHz的PWM波。1000,18为6MHz计数频率，6KHz的PWM波。1000,9为12MHz计数频率，12KHz的PWM波。1000,6为18MHz计数频率，18KHz的PWM波。频率再提高报错
	TIM3_PWM_Init(1000-1,216-1);                             //自转角的PWM
	//TIM8_PWM_Init(1000-1,9-1);		                         //12Mhz的计数频率,12Khz的PWM. 
	TIM8_PWM_Init(1000-1,18-1);		                         //底轮的PWM    36Mhz的计数频率,72Khz的PWM. 
	
	TIM_interrupt_Init(10000-1,10800-1);                     //时间中断
	
	//给陀螺转动缓慢加速
	gyrol=500;
	gyror=500;
	delay_ms(3000);
	for(i=0;i<=8;i++)
	{
		gyrol=500+i*50;
		gyror=500+i*50;
		delay_ms(100);
	}
	gyror=gyror-18;               //右侧陀螺减速，使2变转速相同
	//gyrol=gyrol+4.5;
	//gyrol=185;//185
	//gyror=185;
	delay_ms(3000);


	//float转char型定义
	/*printa[0]='r';
	printa[1]='o';
	printa[2]='l';
	printa[3]='l';
	printa[8]=0x0d;
	printa[9]=0x0a;*/
	
	
    while(1)
    {
        GPIOA->BSRR=0x200;                                   //使能PA9，也就是将D12置位，进动角使能位置位		
		D22(1);                                              //置位D22，使底轮电机使能
		D24(1);
		
		encoder=(short)TIM2->CNT;                            //进动角度编码器值
			//dir=TIM2->CR1&0x0010;
			if((encoder>=600)||(encoder<=-600))              //600，进动角度过大保护，一周A信号512脉冲，共2信号，且设定上下沿都跳，因此一周信号为2048
			{
				D12(0);                                      //进动电机关闭使能
				LED0(0);
			}		
			angle_precession=encoder*0.17578;                //0.17578=360（度）/2048（一圈测到的AB项上下沿数量）
			
			encoder_ground=(short)TIM5->CNT;                 //底轮编码器读取
			angle_ground=encoder_ground*0.17578;             //底轮转动角度值
			angle_ground_d=(angle_ground-old_angle_ground)/dt;
			old_angle_ground=angle_ground;
			angle_ground_a=(angle_ground_d-old_angle_ground_d)/dt;
			old_angle_ground_d=angle_ground_d;
			
		
			//IIC用DMA调试区域
			//1.以DMA中传输完成为节点，重新设置
			if(DMA1->LISR&(1<<21))	//等待DMA2_Steam7传输完成
				{
					SCB_DisableDCache();    //只有停止Dcache才能进行IIC的DMA传输功能，之后会对Dcache进行再开启
					DMA1->LIFCR|=1<<21;	//清除DMA2_Steam7传输完成标志
					DMA1->LIFCR|=1<<20;	//清除DMA2_Steam7半传输完成标志
					//delay_ms(1000);
					//MYDMA_Config_IIC(DMA1_Stream2,2,(u32)&I2C4->RXDR,(u32)datatemp,24,0);
					//DMA_Send_Enable(DMA1_Stream2,24);
					//IIC_HD_Init_DMA(0x234,24);
					//I2C4->CR1|=1<<15;     //DMA接收使能 RXNE=1
					//DMA_Send_Enable(DMA1_Stream2,25);
					//I2C4->CR2|=24<<16;//接收数据长度
					//JY901_I2C_Read_Bytes_DMA(datatemp,0x234,24);
					IIC_DMA_READ();
		        }
				SCB_EnableDCache();
			//2.以IIC停止位收到为节点设置
			/*if((I2C4->ISR&(1<<5)))
			{
				if(DMA1->LISR &(1<<21))
				{
					DMA1->LIFCR|=1<<21;
				}
				I2C4->ICR|=1<<5;   //stop位清零

				
				I2C4->CR1&=~(1<<0);    //PE位清零，各配置位复位
				delay_us(30);
				I2C4->CR1|=1<<0;       //PE为1，启用IIC2
				I2C4->CR1|=1<<15;     //DMA接收使能
				I2C4->CR2|=0x234;    //地址设置
				I2C4->CR2|=1<<10;    //传输方向设置-
				I2C4->CR2|=24<<16;//接收数据长度
				
				
				DMA_Send_Enable(DMA1_Stream2,24);
				I2C4->CR2|=1<<13;         //开始-
				
			}*/
			
			
			/*
			//先读取欧拉角，再读取角速度
			JY901_I2C_Read_Bytes(datatemp,0x7A,0x3d,6);
			roll=(float)CharToShort(&datatemp[0])/32768*180;
			pitch=(float)CharToShort(&datatemp[2])/32768*180;
			yaw=(float)CharToShort(&datatemp[4])/32768*180;
			printf("roll角为%f\n",roll);
			
			
			JY901_I2C_Read_Bytes(&datatemp_g[0],0x7A,0x37,6);
			gyrox=(float)CharToShort(&datatemp_g[0])/32768*2000;
			gyroy=(float)CharToShort(&datatemp_g[2])/32768*2000;
			gyroz=(float)CharToShort(&datatemp_g[4])/32768*2000;
			
			printf("gx角为%f\n",gyrox);*/
			
			//同时读取全部数据
			//JY901_I2C_Read_Bytes(datatemp,0x7A,0x34,24);      //地址为0x55
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
			
			
			//右边陀螺接收信号
/*			//JY901_I2C_Read_Bytes_R(datatemp_r,0x51,0x37,24);
			gyrox_r=(float)CharToShort(&datatemp_r[0])/32768*2000;
			gyroy_r=(float)CharToShort(&datatemp_r[2])/32768*2000;
			gyroz_r=(float)CharToShort(&datatemp_r[4])/32768*2000;
			roll_r=(float)CharToShort(&datatemp_r[12])/32768*180;
			pitch_r=(float)CharToShort(&datatemp_r[14])/32768*180;
			yaw_r=(float)CharToShort(&datatemp_r[16])/32768*180;*/
			
			if((gyrox-gyrox_old)>=8||(gyrox-gyrox_old)<=-8)       //过滤掉杂波
				gyrox=gyrox_old;
			
			//互补滤波
			//ComplementaryFilterAngleX();
			roll_filter=ComplementaryFilterAngleX((double)ay,(double)gyrox); 
			pitch_filter=ComplementaryFilterAngleX((double)ax,(double)gyroy);
			
			roll=roll+1.2;    //5Hz：0.95到0.98。20Hz：0.3。200Hz：1.2-1.3     1.23（角度程序补偿）   +1.2
			pitch=pitch-1.5;   //-1.4暂时可用    -1.8     -1.0
			yaw=yaw+27;       //
						
			
			//DMA写数据
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
			//非循环方式进行DMA发送数据
			if(DMA1->LISR&(1<<27))
			{
				DMA1->LIFCR|=1<<27;
				sprintf(printa,"roll:%2.2f;pitch:%2.2f;yaw:%2.2f;an_gr:%2.2f;an_pr:%2.2f;\n",roll,pitch,yaw,angle_ground,angle_precession);//横滚角，仰俯角，滤波后横滚角，滤波后仰俯角
				//sprintf(printa,"roll:%2.2f;a1_pi:%2.2f;a2_pi:%2.2f;b_pit:%2.2f;\n",roll,a1_est_pitch,a2_est_pitch,b_est_pitch);//横滚角，仰俯a1，仰俯a2，仰俯b
				//sprintf(printa,"roll:%2.2f;pitch:%2.2f;a1_ro:%2.2f;b1_ro:%2.2f;\n",roll,pitch,a_est_roll,b_est_roll);//横滚角，仰俯角，滤波后横滚角，滤波后仰俯角
				
				//sprintf(printa,"a_roll%2.2fb_roll%2.2fa1_pi%2.2fa2_pi%2.2f\n",a_est_roll,b_est_roll,a1_est_pitch,a2_est_pitch);//横滚角自适应系数a和b，仰俯角自适应系数a1和阿
				//sprintf(printa,"ground%dalpha%d\n",groundPWM_current,precessionPWM);//底轮电流PWM，进动角速度PWM
				USART3->CR3=1<<7;
				j=strlen(printa);
				DMA_Send_Enable(DMA1_Stream3 ,j);
			}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
			
				
			//float转char型
			//方式三，字符型，可用，但数据长度变化时无效
			//sprintf(printa,"roll%c%2.2f\n",roll_mark,roll_uart);
			//方式一，union型，可用，但读取不容易，是已IEEE754标准输出
			/*printangle.angle=roll;
			for(int s=0;s<4;s++)
			{
				printa[4+s]=printangle.a[3-s];
			}*/
			//方式二，float地址读，也是IEEE754标准输出
			/*for(int q=0;q<4;q++)
			{
				printa[4+q]=((u8*)(&roll))[3-q];
			}*/				
				
			//串口输出
			//printf("roll为%f,\n",roll);
			//printf("滤波后roll为%f,",AngleYFiler);
			//printf("roll_r为%f",roll_r);
			//printf("encoder为%d",encoder);
			//printf("gx角为%f\n",gyrox);
			
			gyrox_old=gyrox;
			Sum_Roll=Sum_Roll+roll;
			
			Sum_Pitch=Sum_Pitch+pitch;
			
			//************************偏航角yaw方向*************************
			if((num_time>=800)&&(num_time<850))
			{
				yaw_set=10;
			}
			else
				yaw_set=0;
			
			
			

			//************************横滚角roll方向*************************
			//++++++1.PID控制+++++
			//precessionPWM=489+(short)(-precession_KP*roll-precession_KD*gyrox-precession_KI*Sum_Roll-precessiondegree_KP*angle_precession);     //500和18起始为244。1000和18起始为494.1000和6起始为488.1000和9起始为489。 论文2.5再用PD算法，新。
			//precessionPWM=489+(short)(-precession_KP*roll_filter-precession_KD*gyrox-precession_KI*Sum_Roll-precessiondegree_KP*angle_precession);     //应用滤波 500和18起始为244。1000和18起始为494.1000和6起始为488.1000和9起始为489。
			//++++++1.自适应模糊控制器（2）++++++
			//输入量
			r1_roll=0;//.001*sin(angle_precession);
			r2_roll=-0.0002*Realize_roll((pitch+0.2*gyroy),angle_ground_a);
			r3_roll=-0.08*(0-angle_precession);
			r_roll=r1_roll+r2_roll+r3_roll;
			
			//参数计算
			e_roll_d=(0-gyrox);
			a_est_roll=a_est_roll-0.1*e_roll_d*roll*dt;
			b_est_roll=b_est_roll-0.02*e_roll_d*r_roll*cos(angle_precession)*dt;
			
			//变量数值化
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
			
			//进动角速度输出
			cos_alpha=cos(angle_precession);
			if(cos_alpha<=0)
				cos_alpha=0.01;
			precessionPWM=489+a_adapt_roll+b_adapt_roll+1*yaw_set;                 //0位置附近细化法(新，在用)   增加偏航控制变量
			//precessionPWM=489+(short)(k_adaptive_roll*(0-gyrox)-a_est_roll*roll)+(short)b_est_roll*r_roll;              ///cos(angle_precession)/cos(angle_precession)
			//++++++1.滑模控制器（3）++++++
			//滑模面设置
			slide_surface=roll+0.8*gyrox;
			if(slide_surface>=0)
				sng=1;
			else
				sng=-1;
			//进动角速度输出
			//precessionPWM=489-(6*roll+6*gyrox-0.24*angle_precession-0.8*sng);
			
			//++++++2.进动角边界设定++++++
			roll_old=roll;
			pitch_old=pitch;
			if(precessionPWM>=630)     //500和18的极限为180和3201
				precessionPWM=630;
			if(precessionPWM<=360)
				precessionPWM=360;
			//++++++3.进动角速度PWM给定++++++
			PROCESSION_PWM=precessionPWM;
			
			//************************仰俯角pitch方向*************************
			//++++++1.PID控制（1）++++++
			//底轮速度模式
			//groundPWM=488+(short)(-ground_KP*pitch-ground_KD*gyroy-ground_KI*Sum_Pitch-groundspeed_KP*angle_ground);      //1000和9起始为489
			//groundPWM=489+(short)(-ground_KP*pitch_filter-ground_KD*gyroy);      //1000和9起始为489
			//底轮电流模式
			groundPWM_current=(short)(ground_KP*(pitch+0.3)+ground_KD*gyroy+groundspeed_KP*(angle_ground_d-10))+5*yaw_set;
			//++++++1.自适应模糊控制器（2）++++++
			//输入量
			r1_pitch=-0;//.2*sin(num_time*dt*6);
			r2_pitch=0;//*Realize_pitch(roll+20*gyrox,angle_ground_d);
			r3_pitch=0.01*(0-angle_ground_d);
			r_pitch=r1_pitch+r2_pitch+r3_pitch;
			//参数计算
			e_pitch_d=(0-gyroy);
			a1_est_pitch=a1_est_pitch-0.01*e_pitch_d*pitch*dt;
			a2_est_pitch=a2_est_pitch+0*e_pitch_d*gyroy*dt;  //*fabs(sin(angle_precession))   0.003
			b_est_pitch=b_est_pitch-0.003*e_pitch_d*r_pitch*dt;
			
			//变量数值化
			a_adapt_pitch_f=-k_adaptive_pitch*(0-gyroy)+a1_est_pitch*pitch+a2_est_pitch*gyroy;      //*sin(angle_precession)
			b_adapt_pitch_f=b_est_pitch*r_pitch;
			if((a_adapt_pitch_f>0)&&(a_adapt_pitch_f<=3))
				a_adapt_pitch=3;
			else if((a_adapt_pitch_f<0)&&(a_adapt_pitch_f>=-3))
				a_adapt_pitch=-3;
			else
				a_adapt_pitch=(short)a_adapt_pitch_f;
			b_adapt_pitch=(short)b_adapt_pitch_f;
			
			//底轮力矩输出
			//groundPWM_current=a_adapt_pitch+b_adapt_pitch;      //变量数值化输出（自适应再用，新）
			//groundPWM_current=-k_adaptive_pitch*(0-gyroy)+a1_est_pitch*pitch+a2_est_pitch*gyroy*sin(angle_precession)+b_est_pitch*r_pitch;      //*fabs(sin(angle_precession))
			
			//++++++2.底轮力矩驱动器设定++++++ 
			/*if(groundPWM_current<=0)
			{
				D24(1);
				groundPWM_current=-groundPWM_current;
			}
			else
				D24(0);*/
			//++++++3.底轮边界设定(力矩)++++++
			if(groundPWM_current>250)                //380
				groundPWM_current=250;
			if(groundPWM_current<-250)
				groundPWM_current=-250;
			//if(groundPWM_current<200)
				//groundPWM_current=200;
			//++++++4.底轮PWM给定++++++
			//GROUND_PWM=150+groundPWM_current;        //290为10%PWM   680位90%PWM（使能&旋向）      143
			GROUND_PWM=500-groundPWM_current;
			groundPWMold=groundPWM;
			
			//******速度模式边界设定
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
	if(TIM4->SR&0X0001)//溢出中断
	{
					    				   				     	    	
	}				   
	TIM4->SR&=~(1<<0);//清除中断标志位
}
