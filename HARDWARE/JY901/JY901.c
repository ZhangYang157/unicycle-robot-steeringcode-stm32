#include "IIC_HD.h"
#include "sys.h"
#include "delay.h"
#include "JY901.h"
#include "IIC_HD_right.h"

double T=0.002;//����ʱ��
//�����˲���
double Kcomplfilter_A = 0.637;			//ǰ�󻥲��˲����Ĳ����� Խ��ֵԽ������һ����ֵ  0.986
double Kcomplfilter_B = 0.95;	 //5HZ  0.976ǰ�󻥲��˲����Ĳ����� Խ��ֵԽ������һ����ֵ  0.976
double Low_K=0.363;

void JY901_Init(void)
{
	IIC_HD_Init();
	IIC_HD_Init_R();
}
/******************JY901�����ֽڣ��Զ�����***************************************
*����ԭ��:		u8 JY901_I2C_Read_Bytes(u8* data,u8 Addr,u8 Addr_in,u8 Length)
*��������:	    ��ȡ���ֽڣ��Զ�����ģʽ����������NACK��STOP
����	data  ��ȡ���ݵ��׵�ַ
		Addr  Ŀ���ַ
		Addr_in Ƭ�ڵ�ַ
		Length �ֽ���
����    ��ȡ���ݳ���
*******************************************************************************/
u8 JY901_I2C_Read_Bytes(u8* data,u8 Addr,u8 Addr_in,u8 Length)
{
	u8 count;
	//u8 chrdata[30];
	Write_Byte_Soft(Addr_in,Addr);
	count=Read_Multi_Auto(data,Addr,Length);
	return count;
}

/******************JY901�����ֽڣ��Զ�����***************************************
*����ԭ��:		u8 JY901_I2C_Read_Bytes(u8* data,u8 Addr,u8 Addr_in,u8 Length)
*��������:	    ��ȡ���ֽڣ��Զ�����ģʽ����������NACK��STOP
����	data  ��ȡ���ݵ��׵�ַ
		Addr  Ŀ���ַ
		Addr_in Ƭ�ڵ�ַ
		Length �ֽ���
����    ��ȡ���ݳ���
*******************************************************************************/
u8 JY901_I2C_Read_Bytes_DMA(u8* data,u16 Addr,u8 Length)
{
	u8 count;
	//u8 chrdata[30];
	count=Read_Multi_Auto_DMA(data,Addr,Length);
	return count;
}


/******************JY901�����ֽڣ��Զ�����***************************************
*����ԭ��:		u8 JY901_I2C_Read_Bytes_R(u8* data,u8 Addr,u8 Addr_in,u8 Length)
*��������:	    ��ȡ�Ҳ���ֽڣ��Զ�����ģʽ����������NACK��STOP
����	data  ��ȡ���ݵ��׵�ַ
		Addr  Ŀ���ַ
		Addr_in Ƭ�ڵ�ַ
		Length �ֽ���
����    ��ȡ���ݳ���
*******************************************************************************/
u8 JY901_I2C_Read_Bytes_R(u8* data,u8 Addr,u8 Addr_in,u8 Length)
{
	u8 count;
	//u8 chrdata[30];
	Write_Byte_Soft_R(Addr_in,Addr);
	count=Read_Multi_Auto_R(data,Addr,Length);
	return count;
}

/******************JY901д���ֽڣ��Զ�����***************************************
*����ԭ��:		void JY901_I2C_Write_Bytes(u8* data,u8 Addr,u8 Addr_in,u8 Length)
*��������:	    ��ȡ���ֽڣ��Զ�����ģʽ����������NACK��STOP
����	data  ��ȡ���ݵ��׵�ַ
		Addr  Ŀ���ַ
		Addr_in Ƭ�ڵ�ַ
		Length �ֽ���
*******************************************************************************/
void JY901_I2C_Write_Bytes(u8* data,u8 Addr,u8 Addr_in,u8 Length)
{
	//u8 chrdata[30];
	Write_Multi_Auto(data,Addr,Addr_in,Length);
}





/******************�����˲���***************************************
*����ԭ��:		ComplementaryFilterAngleX(double Angle_X,double Gyro_Y)
*��������:	    ���ٶȼƵ�ͨ�������Ǹ�ͨ
����	Angle_X  ���ٶ����ýǶ�ֵ����
		Gyro_Y  ����������
*******************************************************************************/
double ComplementaryFilterAngleX(double Angle_X,double Gyro_Y)
{
	double AngleXFilter=0;
	static double OldAngleX;	
	
	AngleXFilter = Kcomplfilter_B *( OldAngleX + Gyro_Y*T) + (1-Kcomplfilter_B) * Angle_X;
	
	OldAngleX= AngleXFilter;
	return AngleXFilter;
}

/******************��ͨ�˲���***************************************
*����ԭ��:		LowPassFiler(double value)
*��������:	    ��ͨ�˲������
����	value  Ҫ��ͨ��ֵ
*******************************************************************************/
double LowPassFiler(double value)
{
	double ValueFilered=0;
	static double LastValue;
	ValueFilered=value*Low_K+LastValue*(1-Low_K);
	return ValueFilered;	
}

