#include "MS5611.h"
#include "GY86_IIC.h"
#include "sys.h"
#include "delay.h"
#include "math.h"

uint16_t Cal_C[8];                                                                 //用于存放PROM中的6组数据 
int32_t  Temperature;                                                              //当前实际温度
float dT;                                                                          //实际和参考温度之间的差异 
float Pressure=0;                                                             //当前实际大气压值
 float  H=0,a,b,VREF,altitude,altitude1;                                                                   //当前高度（米）
unsigned char i=1,c=20;
	
//**********MS5611初始化，主要获取出厂校准参数，用于校准温度和气压数据************************//
void Init_MS561101BA()
{
  u8 data[2];
	u8 i=0;
	
  GY86_Write_commendByte(MS561101BA_Addr,MS561101BA_RST);	                         //MS5611复位
	delay_ms(100); 
	for(i=0;i<7;i++)                                                                 //读取出厂校准数据（共三组，六个）
	{
	GY86_Read_Byte(MS561101BA_Addr,(MS561101BA_PROM_RD+i*2),2,data);            
	Cal_C[i]=data[0]<<8|data[1];
	}
	delay_ms(100); 
}

//**********获取MS5611的温度原始数据并处理成实际温度，用于二介校准气压的温度补偿**************//
void MS561101BA_GetTemperature(u8 OSR_Temp)
{
	 uint32_t D2_Temp;                                                               // 存放数字温度 数据

	 D2_Temp= MS561101BA_DO_CONVERSION(OSR_Temp);
//	 dT=D2_Temp - (Cal_C[5]*256);  
//   Temperature=2000+(dT*(Cal_C[6]/8388608));  //算出温度值的100倍，例如2001表示20.01°
	
	dT=D2_Temp - ((uint32_t)Cal_C[5]*pow(2,8));                                      //参考英文数据手册Page 7
	Temperature=2000+((dT*Cal_C[6])/pow(2,23));                                      //公式TEMP=2000+dT*C6/8388608
	
}


//*************************读取数字压力值并采用二阶温度补偿计算实时压力**********************//
u8 MS561101BA_getPressure(u8 OSR_Temp) 
{
  uint32_t  D1_Pres;                                                               //存放数字气压数据
	int64_t OFF, SENS;
  signed int  Temperature2;  
	const double T1 = 15.0 + 273.15;	/* temperature at base height in Kelvin */
	const double a  = -6.5 / 1000;	/* temperature gradient in degrees per metre */
	const double g  = 9.80665;	/* gravity constant in m/s/s */
	const double R  = 287.05;	/* ideal gas constant in J/kg/K */
	double p1 = 101325 / 1000.0;/* the _msl_pressure  default value is 101325 */
		/* measured pressure in kPa */
		double p =0.0;
	float q ,w ,e ,r;
	
	u32 Aux ,Aux2,OFF2,SENS2;	
	D1_Pres= MS561101BA_DO_CONVERSION(OSR_Temp);                                     //读取数字气压数据
	OFF=Cal_C[2]*pow(2,16)+((Cal_C[4]*dT)/pow(2,7));                                 //计算实际温度抵消数据
	SENS=(Cal_C[1]*pow(2,15))+((Cal_C[3]*dT)/pow(2,8));                              //计算实际温度灵敏度数据
	
 /*开始二阶温度补偿气压*/
 if(Temperature<2000)                                                              //是否是低温（<20.00℃）                                   
 {
  Temperature2 = (dT*dT)/pow(2,31); 
  Aux = (Temperature-2000)*(Temperature-2000);  
  OFF2 = 5*Aux/2; 
  SENS2 = 1.25*Aux; 
	 if(Temperature<-1500)                                                           //是否地非常低的温度（<-15.00℃）
	 {
		 Aux2 = (Temperature+1500)*(Temperature+1500);
	   OFF2 = OFF2+7*Aux2;
		 SENS2 = SENS+11*Aux2/2;
	 }
     Temperature = (Temperature - Temperature2);                                   //计算实际温度
     OFF = OFF - OFF2;                                                             //计算温度抵消数据
     SENS = SENS - SENS2;                                                          //计算温度灵敏度
  } 
 Pressure=((D1_Pres*SENS)/pow(2,21)-OFF)/pow(2,15);                                //(扩大了100倍)温度补偿后的大气压力，单位mbar(1mbar=100pa)
	
	p=Pressure/1000;
	//altitude=(((pow((), (-(a * R) / g))) ) - T1) / a;
	q=p / p1;
	w=a * R/ g;
	e=pow(q,-w);
	r=e* T1-T1;
	altitude=r/a;
	
	//altitude (44330.0f*(1.0f - pow((float)Pressure/101325.0f, 0.190295f))) ;
//  a= Pressure/101325.0;                                                               //简单计算当前高度
//	b= pow(a, 0.190295);
//	if (i==1)
//	{
//	   i=0;
//		VREF= 44330 * (1.0 - b);
//	}
//	if(c>0)
//	{
//	c--;
//	H= 44330 * (1.0 - b)-VREF;
//		altitude1=altitude1+H;
//		if(c==0)
//		{
//			c=20;
//		altitude=altitude1/20;
//			altitude1=0;
//		}
//	}


	
 return 0;
}



//************************************开启AD转换并读取数据*********************************//
uint32_t MS561101BA_DO_CONVERSION(uint8_t command)  
{  
   u8 conversiontable[3];                                                    //存放温差DT数据的数组
   u8 conversion; 	                                                         //存放温差DT的变量
  
   GY86_Write_commendByte(MS561101BA_Addr,command);                                //读AD转换结果的命令      
   delay_ms(20);                                                                   //延时,去掉数据错误  
   GY86_Read_Byte(MS561101BA_Addr,0x00,3,conversiontable);                         //读AD转换结果
   conversion=conversiontable[0]<<16|conversiontable[1]<<8|conversiontable[2];     //数组提取数据合并
   return conversion;  
  
}  

