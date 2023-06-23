#ifndef __ADAPTIVE_CONTROLLER_H
#define __ADAPTIVE_CONTROLLER_H
#include "sys.h"






float Membership_Trimf(float x,u16 a,u16 b,u16 c);
float Membership_Gaussmf(float x,u16 ave,u16 sigma);
float Membership_Trapf(float x,u16 a,u16 b,u16 c,u16 d);
float Membership_Sigmoid(float x,u16 a,u16 b);
float Realize_pitch(float A1,float A2);
float Realize_roll(float A1,float A2);





#endif