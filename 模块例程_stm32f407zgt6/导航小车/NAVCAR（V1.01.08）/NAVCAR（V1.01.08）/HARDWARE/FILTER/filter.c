#include "filter.h" 
#include "usart.h" 
#include "string.h" 
/*******************************************************************************
* 函  数 ：float FindPos(float*a,int low,int high)
* 功  能 ：确定一个元素位序
* 参  数 ：a  数组首地址
*          low数组最小下标
*          high数组最大下标
* 返回值 ：返回元素的位序low
* 备  注 : 无
*******************************************************************************/
int FindPos(double*a,int low,int high)
{
    double val = a[low];                      //选定一个要确定值val确定位置
    while(low<high)
    {
        while(low<high && a[high]>=val)
             high--;                       //如果右边的数大于VAL下标往前移
             a[low] = a[high];             //当右边的值小于VAL则复值给A[low]

        while(low<high && a[low]<=val)
             low++;                        //如果左边的数小于VAL下标往后移
             a[high] = a[low];             //当左边的值大于VAL则复值给右边a[high]
    }
    a[low] = val;
    return low;
}

/*******************************************************************************
* 函  数 ：void QuiteSort(float* a,int low,int high)
* 功  能 ：快速排序
* 参  数 ：a  数组首地址
*          low数组最小下标
*          high数组最大下标
* 返回值 ：无
* 备  注 : 无
*******************************************************************************/
 void QuiteSort(double* a,int low,int high)
 {
     int pos;
     if(low<high)
     {
         pos = FindPos(a,low,high); //排序一个位置
         QuiteSort(a,low,pos-1);    //递归调用
         QuiteSort(a,pos+1,high);
     }
 }
/*******************************************************************************
* 函  数 ：float  SortAver_Filter(float value)
* 功  能 ：去最值平均值滤波一组数据
* 参  数 ：value 采样的数据
*		   *filter 滤波以后的数据地址
* 返回值 ：无
* 备  注 : 无
*******************************************************************************/
//typedef struct
//{
//	double buf[100];
//	u8 cnt;
//} FilterPram;	
 

//n是总个数，m是每一边丢弃个数，本例中为n-2*m（8-2*2=4）
void SortAver_Filter_fp(FilterPram *filt_pram,double value,double *filter,u8 n,u8 m)
{
	double temp;	
	u8 i=0;
	filt_pram->buf[(filt_pram->cnt)++] = value;
	printf("cnt=%d",filt_pram->cnt);
	if(filt_pram->cnt<n) 
	{
		return;  //数组填不满不计算	
	} 
	QuiteSort(filt_pram->buf,0,n-1);
	for(i=0+m;i<n-m;i++)
	{
		temp += filt_pram->buf[i];
		printf("buf[%d]=%.5lf",i,filt_pram->buf[i]);
	}
	printf("temp=%.5lf",temp);
	
	filt_pram->cnt = 0;
	
	*filter = temp/(n-2*m);
}












#if 1
 void SortAver_Filter_JD(double value,double *filter,u8 n)
{
	static double buf[100] = {0.0};
	static u8 cnt=0;
	double temp;	
	u8 i=0;
	buf[cnt++] = value;
	if(cnt<n) 
	{
		return;  //数组填不满不计算	
	} 
	QuiteSort(buf,0,n-1);
	for(i=0+1;i<n-1;i++)
	{
		temp += buf[i];
		//printf("buf[%d]=%.5lf",i,buf[i]);
	}
	
	cnt = 0;
	
	*filter = temp/(n-2);
}
#endif

#if 1
void SortAver_Filter_WD(double value,double *filter,u8 n)
{
	static double buf[100] = {0.0};
	static u8 cnt=0;
	double temp;	
	u8 i=0;
	buf[cnt++] = value;
	if(cnt<n) 
	{
		return;  //数组填不满不计算	
	} 
	QuiteSort(buf,0,n-1);
	for(i=0+1;i<n-1;i++)
	{
		temp += buf[i];
		//printf("buf[%d]=%.5lf",i,buf[i]);
	}
	
	cnt = 0;
	
	*filter = temp/(n-2);
}
#endif

#if 1
void SortAver_Filter_YAW(double value,double *filter,u8 n)
{
	static double buf[100] = {0.0};
	static u8 cnt=0;
	double temp;	
	u8 i=0;
	buf[cnt++] = value;
	if(cnt<n) 
	{
		return;  //数组填不满不计算	
	} 
	QuiteSort(buf,0,n-1);
	for(i=0+1;i<n-1;i++)
	{
		temp += buf[i];
		//printf("buf[%d]=%.5lf",i,buf[i]);
	}
	
	cnt = 0;
	
	*filter = temp/(n-2);
}
#endif



























#if 0
 void SortAver_Filter(double value,double *filter,u8 n)
{
	static double buf[100] = {0.0};
	static u8 cnt=0;
	double temp;	
	u8 i=0;
	buf[cnt++] = value;
	if(cnt<n) 
	{
		return;  //数组填不满不计算	
	} 
	QuiteSort(buf,0,n-1);
	for(i=0+2;i<n-2;i++)
	{
		temp += buf[i];
		printf("buf[%d]=%.5lf",i,buf[i]);
	}
	
	cnt = 0;
	
	*filter = temp/(n-2);
}
#endif
