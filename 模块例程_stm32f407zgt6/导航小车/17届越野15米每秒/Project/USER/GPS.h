#include "mm32_device.h"
#include "hal_conf.h"
void uart_interrupt_handler (void);
extern unsigned char JD[16];
extern unsigned char WD[15];
extern char location[100];
void send_string(char* ch,int num);