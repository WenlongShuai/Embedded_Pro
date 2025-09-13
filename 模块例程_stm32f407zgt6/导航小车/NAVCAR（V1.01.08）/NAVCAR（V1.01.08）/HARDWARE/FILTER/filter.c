#include "filter.h" 
#include "usart.h" 
#include "string.h" 
/*******************************************************************************
* ��  �� ��float FindPos(float*a,int low,int high)
* ��  �� ��ȷ��һ��Ԫ��λ��
* ��  �� ��a  �����׵�ַ
*          low������С�±�
*          high��������±�
* ����ֵ ������Ԫ�ص�λ��low
* ��  ע : ��
*******************************************************************************/
int FindPos(double*a,int low,int high)
{
    double val = a[low];                      //ѡ��һ��Ҫȷ��ֵvalȷ��λ��
    while(low<high)
    {
        while(low<high && a[high]>=val)
             high--;                       //����ұߵ�������VAL�±���ǰ��
             a[low] = a[high];             //���ұߵ�ֵС��VAL��ֵ��A[low]

        while(low<high && a[low]<=val)
             low++;                        //�����ߵ���С��VAL�±�������
             a[high] = a[low];             //����ߵ�ֵ����VAL��ֵ���ұ�a[high]
    }
    a[low] = val;
    return low;
}

/*******************************************************************************
* ��  �� ��void QuiteSort(float* a,int low,int high)
* ��  �� ����������
* ��  �� ��a  �����׵�ַ
*          low������С�±�
*          high��������±�
* ����ֵ ����
* ��  ע : ��
*******************************************************************************/
 void QuiteSort(double* a,int low,int high)
 {
     int pos;
     if(low<high)
     {
         pos = FindPos(a,low,high); //����һ��λ��
         QuiteSort(a,low,pos-1);    //�ݹ����
         QuiteSort(a,pos+1,high);
     }
 }
/*******************************************************************************
* ��  �� ��float  SortAver_Filter(float value)
* ��  �� ��ȥ��ֵƽ��ֵ�˲�һ������
* ��  �� ��value ����������
*		   *filter �˲��Ժ�����ݵ�ַ
* ����ֵ ����
* ��  ע : ��
*******************************************************************************/
//typedef struct
//{
//	double buf[100];
//	u8 cnt;
//} FilterPram;	
 

//n���ܸ�����m��ÿһ�߶���������������Ϊn-2*m��8-2*2=4��
void SortAver_Filter_fp(FilterPram *filt_pram,double value,double *filter,u8 n,u8 m)
{
	double temp;	
	u8 i=0;
	filt_pram->buf[(filt_pram->cnt)++] = value;
	printf("cnt=%d",filt_pram->cnt);
	if(filt_pram->cnt<n) 
	{
		return;  //�������������	
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
		return;  //�������������	
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
		return;  //�������������	
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
		return;  //�������������	
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
		return;  //�������������	
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
