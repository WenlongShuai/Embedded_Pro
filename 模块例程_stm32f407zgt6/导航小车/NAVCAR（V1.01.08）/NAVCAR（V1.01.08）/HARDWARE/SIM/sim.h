#ifndef __SIM_H
#define __SIM_H			    
#include "sys.h"  

//#define MSISDN  "00310033003200300031003600310038003200350033"     //���ԵĽ��ܶ��ź͵绰���ֻ�����
#define MSISDN  "17866914036"     //���ԵĽ��ܶ��ź͵绰���ֻ�����

u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime);

u8 sim900a_check_status(void);

u8 si900a_sms_test(u8* msisdn);


#endif
















