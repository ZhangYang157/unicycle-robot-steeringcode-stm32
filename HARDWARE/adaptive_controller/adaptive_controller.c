#include "adaptive_controller.h"
#include "math.h"

float Membership_Trimf(float x,u16 a,u16 b,u16 c)           //隶属度函数--三角型
{
	float u_Trimf=0;
	if((x>=a)&&(x<=b))
		u_Trimf=(x-a)/(b-a);
	else if((x>b)&&(x<=c))
		u_Trimf=(c-x)/(c-b);
	else
		u_Trimf=0;
	return u_Trimf;
}

float Membership_Gaussmf(float x,u16 ave,u16 sigma)          //隶属度函数--高斯型（正态）
{
	float u_Gaussmf=0;
	u_Gaussmf=exp(-(x-ave)*(x-ave)/(2*sigma*sigma));
	return u_Gaussmf;
}

float Membership_Trapf(float x,u16 a,u16 b,u16 c,u16 d)       //隶属度函数--梯型
{
	float u_Trapf=0;
	if((x>=a)&&(x<b))
		u_Trapf=(x-a)/(b-a);
	else if((x>=b)&&(x<c))
		u_Trapf=1;
	else if((x>=c)&&(x<=d))
		u_Trapf=(d-x)/(d-c);
	else
		u_Trapf=0;
	return u_Trapf;
}

float Membership_Sigmoid(float x,u16 a,u16 b)                 //隶属度函数--sig型
{
	float u_Sigmoid=0;
	u_Sigmoid=1/(1+exp(-a*(x-b)));
	return u_Sigmoid;
}

float Realize_pitch(float A1,float A2)
{
	int i,j=0,k=0,m=0,n=0;
	u8 A1_mf_paras[15]={-6,-2,-1,-6,-1,0,-1,0,1,0,1,6,1,2,6};            //
	
	u8 A2_mf_paras[15]={-6,-2,-1,-6,-1,0,-1,0,1,0,1,6,1,2,6};
	
	u8 u_mf_paras[15]={-6,-2,-1,-6,-1,0,-1,0,1,0,1,6,1,2,6};
	
	u8 ruleMatrix[5][5]={{0,0,3,6,6},{0,0,0,3,3},{-3,0,0,0,3},{-6,-3,0,0,3},{-6,-3,-3,-3,0}};
	
	float umax_pitch=100;
	float u,num=0,den=0;
	float u_A1[5],u_A2[5];
	u8 u_A1_index[3],u_A2_index[3];
	
	if(A1>6)
		A1=6;
	else if(A1<-6)
		A1=-6;
	if(A2>6)
		A2=6;
	else if(A2<-6)
		A2=-6;
	
	for(i=0;i<5;i++)
	{
		u_A1[i]=Membership_Trimf(A1,A1_mf_paras[i*3],A1_mf_paras[i*3+1],A1_mf_paras[i*3+2]);
		if(u_A1[i]!=0)
		{
			u_A1_index[j]=i;
			j++;
		}	
	}
	for(i=j;i<3;i++)
		u_A1_index[i]=4;
	
	for(i=0;i<5;i++)
	{
		u_A2[i]=Membership_Trimf(A2,A2_mf_paras[i*3],A2_mf_paras[i*3+1],A2_mf_paras[i*3+2]);
		if(u_A2[i]!=0)
		{
			u_A2_index[k]=i;
			k++;
		}	
	}
	for(i=k;i<3;i++)
		u_A2_index[i]=4;
	
	for(m=0;m<3;m++)
	{
		for(n=0;n<3;n++)
		{
			num=num+u_A1[u_A1_index[n]]*u_A2[u_A2_index[m]]*ruleMatrix[u_A1_index[n]][u_A2_index[m]];
			den=den+u_A1[u_A1_index[n]]*u_A2[u_A2_index[m]];
		}
	}
		if(den==0)
		den=0.1;
	u=num/den;
	if(u<-umax_pitch)
		u=-umax_pitch;
	if(u>umax_pitch)
		u=umax_pitch;
	
	return u;
}

float Realize_roll(float A1,float A2)
{
	int i,j=0,k=0,m=0,n=0;
	u8 A1_mf_paras[15]={-6,-2,-1,-6,-1,0,-1,0,1,0,1,6,1,2,6};
	
	u8 A2_mf_paras[15]={-6,-2,-1,-6,-1,0,-1,0,1,0,1,6,1,2,6};
	
	u8 u_mf_paras[15]={-6,-2,-1,-6,-1,0,-1,0,1,0,1,6,1,2,6};
	
	u8 ruleMatrix[5][5]={{0,0,3,6,6},{0,0,0,3,3},{-3,0,0,0,3},{-6,-3,0,0,3},{-6,-3,-3,-3,0}};
	
	float umax_roll=100;
	float u,num=0,den=0;
	float u_A1[5],u_A2[5];
	u8 u_A1_index[3],u_A2_index[3];
	
	if(A1>6)
		A1=6;
	else if(A1<-6)
		A1=-6;
	if(A2>6)
		A2=6;
	else if(A2<-6)
		A2=-6;
	
	for(i=0;i<5;i++)
	{
		u_A1[i]=Membership_Trimf(A1,A1_mf_paras[i*3],A1_mf_paras[i*3+1],A1_mf_paras[i*3+2]);
		if(u_A1[i]!=0)
		{
			u_A1_index[j]=i;
			j++;
		}	
	}
	for(i=j;i<3;i++)
		u_A1_index[i]=4;
	
	for(i=0;i<5;i++)
	{
		u_A2[i]=Membership_Trimf(A2,A2_mf_paras[i*3],A2_mf_paras[i*3+1],A2_mf_paras[i*3+2]);
		if(u_A2[i]!=0)
		{
			u_A2_index[k]=i;
			k++;
		}	
	}
	for(i=k;i<3;i++)
		u_A2_index[i]=4;
	
	for(m=0;m<3;m++)
	{
		for(n=0;n<3;n++)
		{
			num=num+u_A1[u_A1_index[n]]*u_A2[u_A2_index[m]]*ruleMatrix[u_A1_index[n]][u_A2_index[m]];
			den=den+u_A1[u_A1_index[n]]*u_A2[u_A2_index[m]];
		}
	}
	if(den==0)
		den=0.1;
	u=num/den;
	if(u<-umax_roll)
		u=-umax_roll;
	if(u>umax_roll)
		u=umax_roll;
	
	return u;
}







