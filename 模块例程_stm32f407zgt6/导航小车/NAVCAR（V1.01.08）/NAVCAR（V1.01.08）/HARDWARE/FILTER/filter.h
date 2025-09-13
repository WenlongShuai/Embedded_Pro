#ifndef __FILTER_H
#define __FILTER_H

#include "sys.h"

typedef struct
{
	double buf[100];
	u8 cnt;
} FilterPram;

void SortAver_Filter_JD(double value,double *filter,u8 n);
void SortAver_Filter_WD(double value,double *filter,u8 n);
void SortAver_Filter_YAW(double value,double *filter,u8 n);
void SortAver_Filter_fp(FilterPram *filt_pram,double value,double *filter,u8 n,u8 m);

#endif
