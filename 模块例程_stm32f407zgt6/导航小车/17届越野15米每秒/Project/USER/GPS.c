#include "GPS.h"
#include "headfile.h"
uint8 uart2_get_buffer;
unsigned char  flag_rec=0;    //接收数据标志
unsigned char num_rec=0;      // 计数标志      
unsigned char flag_data;	//date flag
//only displaty cmd $GPGGA information
unsigned char JD[16];		//longitude
unsigned char JD_a;		//经度方向
unsigned char WD[15];		//latitude
unsigned char WD_a;		// 纬度方向
unsigned char date[6];		//date
unsigned char time[6];		//date
unsigned char time1[6];		//date
unsigned char speed[5]={'0','0','0','0','0'};		// 速度
unsigned char high[6];		// 高度
unsigned char angle[5];		//方位角
unsigned char use_sat[2];	// 卫星计数器
unsigned char total_sat[2];	//卫星总数
unsigned char lock;			//位置状态

//date handing variable
unsigned char seg_count;	// 逗号计数器
unsigned char dot_count;	//小数点计数器
unsigned char byte_count;	// 位计数器
unsigned char cmd_number;	// 命令模式
unsigned char mode;			//
unsigned char buf_full;		
unsigned char cmd[5];		// 存储命令模式
int byte_count_2 = 0;
char location[100] = {'0'};

void GPS_Handle(unsigned char tmp) 
{
	if(1)
	{
		
//		tmp=SBUF;            // 从缓冲区接收数据
		switch(tmp)   //if $GPGGA,$GNGSW,$GNRMC,get data then processing it
		{
      //date start with $
			case '$':
				cmd_number=0;		// 清除命令模式
				mode=1;				// 选项命令接收模式
				byte_count=0;		//清除位计数器
				flag_data=1;     // 设置数据标志
				flag_rec=1;		// 设置数据接收标志
//				byte_count_2 = 0;
			break;

			case ',':         //Eg:$GNRMC,134645.000,A,2603.964436,N,11912.410232,E,0.000,15.744,030718,,E,A*0B
				seg_count++;		// 计数器增加
				byte_count=0;
				break;

			case '*':
				switch(cmd_number)
				{
					case 1:
						buf_full|=0x01;   //00000001
						break;
					case 2:
						buf_full|=0x02;  //00000010
						break;
					case 3:
						buf_full|=0x04;  //00000100
						break;
				}

				mode=0;         //clear mode
				break;
			default:
// receive date cmd
				if(mode==1)	
				{
					cmd[byte_count]=tmp;	// 获取数据和存储缓冲区					if(byte_count>=4)          //overlook cmd which less 4 bit
					{			
						if(cmd[0]=='G')           // 第一个字符
						{
							if(cmd[1]=='N')
							{
								if(cmd[2]=='G')
								{
									if(cmd[3]=='G')
									{
										if(cmd[4]=='A')//判断$GNGGA
										{
											cmd_number=1;      //数据类型
											mode=2;            //接收日期
											seg_count=0;       //comma counter clear
											byte_count=0;      //位计数器清除
											byte_count_2 = 0;
//											send_string((char*)cmd,5);
											cmd[4] = '0';
										}
									}
									else if(cmd[3]=='S')       //命令模式$GNGSV
									{
										if(cmd[4]=='V')
										{
											cmd_number=2;
											mode=2;                //获取数据
											seg_count=0;
											byte_count=0;
										}
									}
								}
								else if(cmd[2]=='R')   //命令模式 $GNRMC
								{
									if(cmd[3]=='M')
									{
										if(cmd[4]=='C')
										{
											cmd_number=3;
											mode=2;         //存储数据
											seg_count=0;
											byte_count=0;
										}
									}
								}
							}
						}
					}
				}
//日期处理
			else if(mode==2)
			{
				
				switch (cmd_number)  //if receive data
				{
					case 1:				//get and store data,$GPGGA,[],[],[],[],[],[],[],[],[].....
						switch(seg_count)   //  comma 计数器
						{
							case 2:		// 2rd逗号后的纬度
								if(byte_count<11)
								{
									WD[byte_count]=tmp;   //获取纬度
								}
								break;
							case 3:		//纬度方向
								if(byte_count<1)
								{
									WD_a=tmp;
								}
								break;
							case 4:		//经度
								if(byte_count<11)
								{
									JD[byte_count]=tmp; //存储
								}
								break;
							case 5:		//经度方向
								if(byte_count<1)
								{
									JD_a=tmp;
								}
								break;
							case 6:		//location
								if(byte_count<1)
								{
									lock=tmp;
								}
								break;
							case 7:		
								if(byte_count<2)
								{
									use_sat[byte_count]=tmp;
								}
								break;
							case 9:		// 高度
								if(byte_count<6)
								{
									high[byte_count]=tmp;
								}
								break;
						}
//						location[byte_count_2] = tmp;
//						byte_count_2++;
						break;

					case 2:	//命令模式  $GPGSV
						switch(seg_count)
						{
							case 3:		// 卫星总数
								if(byte_count<2)
								{
									total_sat[byte_count]=tmp;
								}
								break;
						}
						break;
//命令模式3：无SUE
						case 3:				//$GPRMC
							switch(seg_count)
							{
								case 1:		//time
									if(byte_count<6)
									{				
										time[byte_count]=tmp;	
									}
									break;
								case 2:		// 位置			
									if(byte_count<1)
									{
									  if (tmp=='V') {lock=0;}
									  else
									  {
									    lock=1;
									   }
									}
									break;
								case 3:		//lititude			
//									if(byte_count<9)
//									{
//										WD[byte_count]=tmp;//我们只需要一次
//									}
									break;
								case 4:		//					
									if(byte_count<1)
									{
										WD_a=tmp;
									}
									break;
								case 5:		//			
//									if(byte_count<10)
//									{
//										JD[byte_count]=tmp;  //do not get again
//									}
									break;
								case 6:		// 直线方向	
									if(byte_count<1)
									{
										JD_a=tmp;
									}
									break;
								case 7:		// 速度处理		
									if(byte_count<5)
									{
										speed[byte_count]=tmp;
									}
									break;
								case 8:		// 方向角				
									if(byte_count<5)
									{
										angle[byte_count]=tmp;
									}
									break;
								case 9:		//other			
									if(byte_count<6)
									{
										date[byte_count]=tmp;
									}
									break;

							}
							break;
					}
				}
				byte_count++;		// 位计数器++
				break;
		}
	}
	
}
void uart_interrupt_handler (void)													// 这个函数在 isr.c 的 UART2_IRQHandler 中调用
{													// UART2 中断标志置位
	uart_getchar(UART_2, &uart2_get_buffer);										// 读取数据
//	uart_putchar(UART_1, uart2_get_buffer);	
	GPS_Handle(uart2_get_buffer);
//	printf("%c",uart2_get_buffer);
}
void send_string(char* ch,int num){
	int i = 0;
	while(i < num){
		uart_putchar(UART_1, ch[i]);
		i++;
	}
}