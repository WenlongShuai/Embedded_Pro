#include "GPS.h"
#include "headfile.h"
uint8 uart2_get_buffer;
unsigned char  flag_rec=0;    //�������ݱ�־
unsigned char num_rec=0;      // ������־      
unsigned char flag_data;	//date flag
//only displaty cmd $GPGGA information
unsigned char JD[16];		//longitude
unsigned char JD_a;		//���ȷ���
unsigned char WD[15];		//latitude
unsigned char WD_a;		// γ�ȷ���
unsigned char date[6];		//date
unsigned char time[6];		//date
unsigned char time1[6];		//date
unsigned char speed[5]={'0','0','0','0','0'};		// �ٶ�
unsigned char high[6];		// �߶�
unsigned char angle[5];		//��λ��
unsigned char use_sat[2];	// ���Ǽ�����
unsigned char total_sat[2];	//��������
unsigned char lock;			//λ��״̬

//date handing variable
unsigned char seg_count;	// ���ż�����
unsigned char dot_count;	//С���������
unsigned char byte_count;	// λ������
unsigned char cmd_number;	// ����ģʽ
unsigned char mode;			//
unsigned char buf_full;		
unsigned char cmd[5];		// �洢����ģʽ
int byte_count_2 = 0;
char location[100] = {'0'};

void GPS_Handle(unsigned char tmp) 
{
	if(1)
	{
		
//		tmp=SBUF;            // �ӻ�������������
		switch(tmp)   //if $GPGGA,$GNGSW,$GNRMC,get data then processing it
		{
      //date start with $
			case '$':
				cmd_number=0;		// �������ģʽ
				mode=1;				// ѡ���������ģʽ
				byte_count=0;		//���λ������
				flag_data=1;     // �������ݱ�־
				flag_rec=1;		// �������ݽ��ձ�־
//				byte_count_2 = 0;
			break;

			case ',':         //Eg:$GNRMC,134645.000,A,2603.964436,N,11912.410232,E,0.000,15.744,030718,,E,A*0B
				seg_count++;		// ����������
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
					cmd[byte_count]=tmp;	// ��ȡ���ݺʹ洢������					if(byte_count>=4)          //overlook cmd which less 4 bit
					{			
						if(cmd[0]=='G')           // ��һ���ַ�
						{
							if(cmd[1]=='N')
							{
								if(cmd[2]=='G')
								{
									if(cmd[3]=='G')
									{
										if(cmd[4]=='A')//�ж�$GNGGA
										{
											cmd_number=1;      //��������
											mode=2;            //��������
											seg_count=0;       //comma counter clear
											byte_count=0;      //λ���������
											byte_count_2 = 0;
//											send_string((char*)cmd,5);
											cmd[4] = '0';
										}
									}
									else if(cmd[3]=='S')       //����ģʽ$GNGSV
									{
										if(cmd[4]=='V')
										{
											cmd_number=2;
											mode=2;                //��ȡ����
											seg_count=0;
											byte_count=0;
										}
									}
								}
								else if(cmd[2]=='R')   //����ģʽ $GNRMC
								{
									if(cmd[3]=='M')
									{
										if(cmd[4]=='C')
										{
											cmd_number=3;
											mode=2;         //�洢����
											seg_count=0;
											byte_count=0;
										}
									}
								}
							}
						}
					}
				}
//���ڴ���
			else if(mode==2)
			{
				
				switch (cmd_number)  //if receive data
				{
					case 1:				//get and store data,$GPGGA,[],[],[],[],[],[],[],[],[].....
						switch(seg_count)   //  comma ������
						{
							case 2:		// 2rd���ź��γ��
								if(byte_count<11)
								{
									WD[byte_count]=tmp;   //��ȡγ��
								}
								break;
							case 3:		//γ�ȷ���
								if(byte_count<1)
								{
									WD_a=tmp;
								}
								break;
							case 4:		//����
								if(byte_count<11)
								{
									JD[byte_count]=tmp; //�洢
								}
								break;
							case 5:		//���ȷ���
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
							case 9:		// �߶�
								if(byte_count<6)
								{
									high[byte_count]=tmp;
								}
								break;
						}
//						location[byte_count_2] = tmp;
//						byte_count_2++;
						break;

					case 2:	//����ģʽ  $GPGSV
						switch(seg_count)
						{
							case 3:		// ��������
								if(byte_count<2)
								{
									total_sat[byte_count]=tmp;
								}
								break;
						}
						break;
//����ģʽ3����SUE
						case 3:				//$GPRMC
							switch(seg_count)
							{
								case 1:		//time
									if(byte_count<6)
									{				
										time[byte_count]=tmp;	
									}
									break;
								case 2:		// λ��			
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
//										WD[byte_count]=tmp;//����ֻ��Ҫһ��
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
								case 6:		// ֱ�߷���	
									if(byte_count<1)
									{
										JD_a=tmp;
									}
									break;
								case 7:		// �ٶȴ���		
									if(byte_count<5)
									{
										speed[byte_count]=tmp;
									}
									break;
								case 8:		// �����				
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
				byte_count++;		// λ������++
				break;
		}
	}
	
}
void uart_interrupt_handler (void)													// ��������� isr.c �� UART2_IRQHandler �е���
{													// UART2 �жϱ�־��λ
	uart_getchar(UART_2, &uart2_get_buffer);										// ��ȡ����
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