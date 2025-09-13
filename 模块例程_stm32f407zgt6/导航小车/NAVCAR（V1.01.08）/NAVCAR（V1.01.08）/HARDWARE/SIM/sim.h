#ifndef __SIM_H
#define __SIM_H			    
#include "sys.h"  

//#define MSISDN  "00310033003200300031003600310038003200350033"     //测试的接受短信和电话的手机号码
#define MSISDN  "17866914036"     //测试的接受短信和电话的手机号码

u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime);

u8 sim900a_check_status(void);

u8 si900a_sms_test(u8* msisdn);


#endif
















