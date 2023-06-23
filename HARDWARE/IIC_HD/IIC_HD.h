#ifndef __IIC_HD_H    //防止出现同时定义相同名头文件的重定义
#define __IIC_HD_H
#include "sys.h" 

#define J_SAVE  0x00;
#define J_AX  0x34;
#define J_AY  0x35;
#define J_AZ  0x36;
#define J_GX  0x37;
#define J_GY  0x38;
#define J_GZ  0x39;
#define J_HX  0x3a;
#define J_HY  0x3b;
#define J_HZ  0x3c;
#define Roll  0x3d;
#define Pitch  0x3e;
#define Yaw  0x3f;
#define TEMP  0x40;

void IIC_HD_Init(void);
void IIC_HD_Init_DMA(u16 Addr,u8 Length);
void Master_Communication_Init(u8 Addr,u8 WRN);
void Master_Communication_Init_10(u16 Addr,u8 WRN);
u8 Read_Byte_Auto(u8 Addr);
void Write_Byte_Auto(u8 data,u8 Addr);
u8 Read_Byte_Soft(u8 Addr);
void Write_Byte_Soft(u8 data,u8 Addr);
void Write_Long_Auto(u8 data,u8 Addr,u8 Addr_in);
void Write_Multi_Auto(u8* data,u8 Addr,u8 Addr_in,u8 Length);
u8 Read_Multi_Auto(u8* data,u8 Addr,u8 Length);
u8 Read_Multi_Auto_DMA(u8* data,u16 Addr,u8 Length);
short CharToShort(unsigned char cData[]);
	
#endif