#ifndef __IIC_HD_RIGHT_H    //��ֹ����ͬʱ������ͬ��ͷ�ļ����ض���
#define __IIC_HD_RIGHT_H
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

void IIC_HD_Init_R(void);
void Master_Communication_Init_R(u8 Addr,u8 WRN);
void Master_Communication_Init_10_R(u16 Addr,u8 WRN);
u8 Read_Byte_Auto_R(u8 Addr);
void Write_Byte_Auto_R(u8 data,u8 Addr);
u8 Read_Byte_Soft_R(u8 Addr);
void Write_Byte_Soft_R(u8 data,u8 Addr);
void Write_Long_Auto_R(u8 data,u8 Addr,u8 Addr_in);
void Write_Multi_Auto_R(u8* data,u8 Addr,u8 Addr_in,u8 Length);
u8 Read_Multi_Auto_R(u8* data,u8 Addr,u8 Length);
	
#endif