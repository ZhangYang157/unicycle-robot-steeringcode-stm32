#include "IIC_HD.h"
#include "sys.h"
#include "delay.h"
#include "JY901.h"
#include "IIC_HD_right.h"

double T=0.002;//采样时间
//互补滤波器
double Kcomplfilter_A = 0.637;			//前后互补滤波器的参数： 越大值越相信上一次数值  0.986
double Kcomplfilter_B = 0.95;	 //5HZ  0.976前后互补滤波器的参数： 越大值越相信上一次数值  0.976
double Low_K=0.363;

void JY901_Init(void)
{
	IIC_HD_Init();
	IIC_HD_Init_R();
}
/******************JY901读多字节，自动结束***************************************
*函数原型:		u8 JY901_I2C_Read_Bytes(u8* data,u8 Addr,u8 Addr_in,u8 Length)
*功　　能:	    读取多字节，自动结束模式，结束后发送NACK和STOP
输入	data  读取数据的首地址
		Addr  目标地址
		Addr_in 片内地址
		Length 字节数
返回    读取数据长度
*******************************************************************************/
u8 JY901_I2C_Read_Bytes(u8* data,u8 Addr,u8 Addr_in,u8 Length)
{
	u8 count;
	//u8 chrdata[30];
	Write_Byte_Soft(Addr_in,Addr);
	count=Read_Multi_Auto(data,Addr,Length);
	return count;
}

/******************JY901读多字节，自动结束***************************************
*函数原型:		u8 JY901_I2C_Read_Bytes(u8* data,u8 Addr,u8 Addr_in,u8 Length)
*功　　能:	    读取多字节，自动结束模式，结束后发送NACK和STOP
输入	data  读取数据的首地址
		Addr  目标地址
		Addr_in 片内地址
		Length 字节数
返回    读取数据长度
*******************************************************************************/
u8 JY901_I2C_Read_Bytes_DMA(u8* data,u16 Addr,u8 Length)
{
	u8 count;
	//u8 chrdata[30];
	count=Read_Multi_Auto_DMA(data,Addr,Length);
	return count;
}


/******************JY901读多字节，自动结束***************************************
*函数原型:		u8 JY901_I2C_Read_Bytes_R(u8* data,u8 Addr,u8 Addr_in,u8 Length)
*功　　能:	    读取右侧多字节，自动结束模式，结束后发送NACK和STOP
输入	data  读取数据的首地址
		Addr  目标地址
		Addr_in 片内地址
		Length 字节数
返回    读取数据长度
*******************************************************************************/
u8 JY901_I2C_Read_Bytes_R(u8* data,u8 Addr,u8 Addr_in,u8 Length)
{
	u8 count;
	//u8 chrdata[30];
	Write_Byte_Soft_R(Addr_in,Addr);
	count=Read_Multi_Auto_R(data,Addr,Length);
	return count;
}

/******************JY901写多字节，自动结束***************************************
*函数原型:		void JY901_I2C_Write_Bytes(u8* data,u8 Addr,u8 Addr_in,u8 Length)
*功　　能:	    读取多字节，自动结束模式，结束后发送NACK和STOP
输入	data  读取数据的首地址
		Addr  目标地址
		Addr_in 片内地址
		Length 字节数
*******************************************************************************/
void JY901_I2C_Write_Bytes(u8* data,u8 Addr,u8 Addr_in,u8 Length)
{
	//u8 chrdata[30];
	Write_Multi_Auto(data,Addr,Addr_in,Length);
}





/******************互补滤波器***************************************
*函数原型:		ComplementaryFilterAngleX(double Angle_X,double Gyro_Y)
*功　　能:	    加速度计低通，陀螺仪高通
输入	Angle_X  加速度所得角度值度数
		Gyro_Y  陀螺仪数字
*******************************************************************************/
double ComplementaryFilterAngleX(double Angle_X,double Gyro_Y)
{
	double AngleXFilter=0;
	static double OldAngleX;	
	
	AngleXFilter = Kcomplfilter_B *( OldAngleX + Gyro_Y*T) + (1-Kcomplfilter_B) * Angle_X;
	
	OldAngleX= AngleXFilter;
	return AngleXFilter;
}

/******************低通滤波器***************************************
*函数原型:		LowPassFiler(double value)
*功　　能:	    低通滤波器设计
输入	value  要低通的值
*******************************************************************************/
double LowPassFiler(double value)
{
	double ValueFilered=0;
	static double LastValue;
	ValueFilered=value*Low_K+LastValue*(1-Low_K);
	return ValueFilered;	
}

