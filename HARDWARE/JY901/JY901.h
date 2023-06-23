#ifndef __JY901_H    //防止出现同时定义相同名头文件的重定义
#define __JY901_H
#include "IIC_HD.h" 
#include "sys.h"
#include "IIC_HD_right.h"



//JY901中I2C各地址位，一个地址位有2个字节
//各设置位
#define SAVE 		0x00
#define CALSW 		0x01
#define RSW 		0x02
#define RRATE		0x03
#define BAUD 		0x04
#define AXOFFSET	0x05
#define AYOFFSET	0x06
#define AZOFFSET	0x07
#define GXOFFSET	0x08
#define GYOFFSET	0x09
#define GZOFFSET	0x0a
#define HXOFFSET	0x0b
#define HYOFFSET	0x0c
#define HZOFFSET	0x0d
#define D0MODE		0x0e
#define D1MODE		0x0f
#define D2MODE		0x10
#define D3MODE		0x11
#define D0PWMH		0x12
#define D1PWMH		0x13
#define D2PWMH		0x14
#define D3PWMH		0x15
#define D0PWMT		0x16
#define D1PWMT		0x17
#define D2PWMT		0x18
#define D3PWMT		0x19
#define IICADDR		0x1a
#define LEDOFF 		0x1b
#define GPSBAUD		0x1c

//年月日设置
#define YYMM		0x30
#define DDHH		0x31
#define MMSS		0x32
#define MS			0x33

//数据位
#define AX			0x34
#define AY			0x35
#define AZ			0x36
#define GX			0x37
#define GY			0x38
#define GZ			0x39
#define HX			0x3a
#define HY			0x3b
#define HZ			0x3c			
#define Roll		0x3d
#define Pitch		0x3e
#define Yaw			0x3f
#define TEMP		0x40

//端口状态
#define D0Status		0x41
#define D1Status		0x42
#define D2Status		0x43
#define D3Status		0x44

//其他数据，如气压，经纬度，GPS
#define PressureL		0x45
#define PressureH		0x46
#define HeightL			0x47
#define HeightH			0x48
#define LonL			0x49
#define LonH			0x4a
#define LatL			0x4b
#define LatH			0x4c
#define GPSHeight   	0x4d
#define GPSYAW      	0x4e
#define GPSVL			0x4f
#define GPSVH			0x50

void JY901_Init(void);
u8 JY901_I2C_Read_Bytes(u8* data,u8 Addr,u8 Addr_in,u8 Length);
u8 JY901_I2C_Read_Bytes_R(u8* data,u8 Addr,u8 Addr_in,u8 Length);
void JY901_I2C_Write_Bytes(u8* data,u8 Addr,u8 Addr_in,u8 Length);
u8 JY901_I2C_Read_Bytes_DMA(u8* data,u16 Addr,u8 Length);

double ComplementaryFilterAngleX(double Angle_X,double Gyro_Y);
double LowPassFiler(double value);
	
#endif