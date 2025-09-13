#include "GPS.h"
#include "headfile.h"
#include <stdio.h>
#include <math.h>
uint8 uart2_get_buffer;
gps_info_struct gps_tau1201;
uint8 gps_tau1201_buffer1[100];                                                 // ����ȡ������ת�Ƶ������飬Ȼ��ʼ�����������
uint8 gps_tau1201_buffer2[100];                                                 // ���ڽ��ջ�����
uint8 gps_tau1201_num;                                                          // ��ǰ�����ַ�����
uint8 gps_tau1201_flag;


//-------------------------------------------------------------------------------------------------------------------
// @brief       �ַ���ת������ ��Ч�ۼƾ���ΪС������λ
// @param       str             �����ַ��� �ɴ�����
// @return      double          ת���������          
// Sample usage:                double dat = str_to_double("-100.2");
//-------------------------------------------------------------------------------------------------------------------
double str_to_double (char *str)
{
    uint8 sign = 0;                                                             // ��Ƿ��� 0-���� 1-����
    double temp = 0.0;                                                          // ��ʱ������� ��������
    double temp_point = 0.0;                                                    // ��ʱ������� С������
    double point_bit = 1;                                                       // С���ۼƳ���

    if('-' == *str)                                                             // ����
    {
        sign = 1;                                                               // ��Ǹ���
        str ++;
    }
    else if('+' == *str)                                                        // �����һ���ַ�������
    {
        str ++;
    }

    // ��ȡ��������
    while(('0' <= *str) && ('9' >= *str))                                       // ȷ�����Ǹ�����
    {
        temp = temp * 10 + ((uint8)(*str) - 0x30);                              // ����ֵ��ȡ����
        str ++;
    }
    if('.' == *str)
    {
        str ++;
        while(('0' <= *str) && ('9' >= *str) && point_bit < 1000000000.0)       // ȷ�����Ǹ����� ���Ҿ��ȿ��ƻ�û����λ
        {
            temp_point = temp_point * 10 + ((uint8)(*str) - 0x30);              // ��ȡС��������ֵ
            point_bit *= 10;                                                    // �����ⲿ��С���ĳ���
            str ++;
        }
        temp_point /= point_bit;                                                // ����С��
    }
    temp += temp_point;                                                         // ����ֵƴ��

    if(sign)
        return -temp;
    return temp;

}

//-------------------------------------------------------------------------------------------------------------------
// @brief       �ַ���ת�������� ���ݷ�Χ�� [-32768,32767]
// @param       str             �����ַ��� �ɴ�����
// @return      int             ת���������          
// Sample usage:                int32 dat = str_to_int("-100");
//-------------------------------------------------------------------------------------------------------------------
int32 str_to_int (char *str)
{
    uint8 sign = 0;                                                             // ��Ƿ��� 0-���� 1-����
    int32 temp = 0;                                                             // ��ʱ�������

    if('-' == *str)                                                             // �����һ���ַ��Ǹ���
    {
        sign = 1;                                                               // ��Ǹ���
        str ++;
    }
    else if('+' == *str)                                                        // �����һ���ַ�������
    {
        str ++;
    }

    while(('0' <= *str) && ('9' >= *str))                                       // ȷ�����Ǹ�����
    {
        temp = temp * 10 + ((uint8)(*str) - 0x30);                              // ������ֵ
        str ++;
    }

    if(sign)
        return -temp;
    return temp;
}
//-------------------------------------------------------------------------------------------------------------------
// @brief		GPS���ڻص�����
// @param		void			
// @return		void            
// Sample usage:				�˺�����Ҫ�ڴ��ڽ����ж��ڽ��е���
//-------------------------------------------------------------------------------------------------------------------

void uart_interrupt_handler (void)													// ��������� isr.c �� UART2_IRQHandler �е���
{
	
	uint8 dat;
    
    uart_query(GPS_TAU1201_UART, &dat);
    if('$' == dat || ('$' != gps_tau1201_buffer2[0]))                           // ֡ͷУ��
    {
        gps_tau1201_num = 0;
    }
    gps_tau1201_buffer2[gps_tau1201_num ++] = dat;
    
    if('\n' == dat)
    {
        // �յ�һ�����
        gps_tau1201_buffer2[gps_tau1201_num] = 0;                               // ��ĩβ���\0
        gps_tau1201_num ++;
        // �������ݵ� gps_tau1201_buffer1
        memcpy(gps_tau1201_buffer1, gps_tau1201_buffer2, gps_tau1201_num);
        gps_tau1201_flag = 1;
    }

}

//-------------------------------------------------------------------------------------------------------------------
// @brief		��ȡָ��,���������
// @param		num             �ڼ�������
// @param		*str            �ַ���           
// @return		uint8           ��������
// Sample usage:				
//-------------------------------------------------------------------------------------------------------------------
static uint8 get_parameter_index (uint8 num, char *str)
{
	uint8 i, j = 0;
    char *temp;
    uint8 len = 0, len1;
    
    temp = strchr(str, '\n');
    if(NULL != temp)
    {
        len = (uint32)temp - (uint32)str + 1;
    }

	for(i = 0; i < len; i ++)
	{
		if(str[i] == ',')
        {
            j ++;
        }
		if(j == num)
        {
            len1 =  i + 1;	
            break;
        }
	}

	return len1;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		�����ַ�����һ��,֮ǰ������ת��Ϊint
// @param		*s              �ַ���
// @return		float           ������ֵ
// Sample usage:				
//-------------------------------------------------------------------------------------------------------------------
static int get_int_number (char *s)
{
	char buf[10];
	uint8 i;
	int return_value;
	i = get_parameter_index(1, s);
	i = i - 1;
	strncpy(buf, s, i);
	buf[i] = 0;
	return_value = str_to_int(buf);
	return return_value;
}
												
//-------------------------------------------------------------------------------------------------------------------
// @brief		�����ַ�����һ��,֮ǰ������ת��Ϊfloat
// @param		*s              �ַ���
// @return		float           ������ֵ
// Sample usage:				
//-------------------------------------------------------------------------------------------------------------------
static float get_float_number (char *s)
{
    uint8 i;
	char buf[10];
	float return_value;
    
	i=get_parameter_index(1, s);
	i = i - 1;
	strncpy(buf, s, i);
	buf[i] = 0;
	return_value = (float)str_to_double(buf);
	return return_value;	
}
									
//-------------------------------------------------------------------------------------------------------------------
// @brief		�����ַ�����һ��,֮ǰ������ת��Ϊdouble	
// @param		*s              �ַ���
// @return		double          ������ֵ
// Sample usage:				
//-------------------------------------------------------------------------------------------------------------------
static double get_double_number (char *s)
{
    uint8 i;
	char buf[10];
	double return_value;
    
	i = get_parameter_index(1, s);
	i = i - 1;
	strncpy(buf, s, i);
	buf[i] = 0;
	return_value = str_to_double(buf);
	return return_value;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		����ʱ��ת��Ϊ����ʱ��	
// @param		*time           �����ʱ��
// @return		void           
// Sample usage:				
//-------------------------------------------------------------------------------------------------------------------
static void utc_to_btc (gps_time_struct *time)
{
    uint8 day_num;
    
    time->hour = time->hour + 8;
	if(time->hour > 23)
	{
		time->hour -= 24;
		time->day += 1;

        if(2 == time->month)
        {
            day_num = 28;
            if((time->year % 4 == 0 && time->year % 100 != 0) || time->year % 400 == 0) // �ж��Ƿ�Ϊ���� 
            {
                day_num ++;                                                     // ���� 2��Ϊ29��
            }
        }
        else
        {
            day_num = 31;                                                       // 1 3 5 7 8 10 12��Щ�·�Ϊ31��
            if(4  == time->month || 6  == time->month || 9  == time->month || 11 == time->month )
            {
                day_num = 30;
            }
        }
        
        if(time->day > day_num)
        {
            time->day = 1;
            time->month ++;
            if(time->month > 12)
            {
                time->month -= 12;
                time->year ++;
            }
        }
	}
}
//-------------------------------------------------------------------------------------------------------------------
// @brief		RMC������
// @param		*line	        ���յ��������Ϣ		
// @param		*gps            ��������������
// @return		uint8           1�������ɹ� 0�����������ⲻ�ܽ���
// Sample usage:				
//-------------------------------------------------------------------------------------------------------------------
static uint8 gps_gnrmc_parse (char *line, gps_info_struct *gps)
{
	uint8 state, temp;
    
    double  latitude;                                                           // ����
    double  longitude;                                                          // γ��
    
	float lati_cent_tmp, lati_second_tmp;
	float long_cent_tmp, long_second_tmp;
	float speed_tmp;
	char *buf = line;
    uint8 return_value = 0;

	state = buf[get_parameter_index(2, buf)];

    gps->state = 0;
    if (state == 'A')                                                           // ���������Ч ���������
    {
        return_value = 1;
        gps->state = 1;
        gps -> ns       = buf[get_parameter_index(4, buf)];
        gps -> ew       = buf[get_parameter_index(6, buf)];

        latitude   = get_double_number(&buf[get_parameter_index(3, buf)]);
        longitude  = get_double_number(&buf[get_parameter_index(5, buf)]);

        gps->latitude_degree  = (int)latitude / 100;                            // γ��ת��Ϊ�ȷ���
        lati_cent_tmp         = (latitude - gps->latitude_degree * 100);
        gps->latitude_cent    = (int)lati_cent_tmp;
        lati_second_tmp       = (lati_cent_tmp - gps->latitude_cent) * 10000;
        gps->latitude_second  = (int)lati_second_tmp;

        gps->longitude_degree = (int)longitude / 100;	                        // ����ת��Ϊ�ȷ���
        long_cent_tmp         = (longitude - gps->longitude_degree * 100);
        gps->longitude_cent   = (int)long_cent_tmp;
        long_second_tmp       = (long_cent_tmp - gps->longitude_cent) * 10000;
        gps->longitude_second = (int)long_second_tmp;
        
        gps->latitude = gps->latitude_degree + (double)gps->latitude_cent / 60 + (double)gps->latitude_second / 600000;
        gps->longitude = gps->longitude_degree + (double)gps->longitude_cent / 60 + (double)gps->longitude_second / 600000;

        speed_tmp      = get_float_number(&buf[get_parameter_index(7, buf)]);   // �ٶ�(����/Сʱ)
        gps->speed     = speed_tmp * 1.85f;                                     // ת��Ϊ����/Сʱ
        gps->direction = get_float_number(&buf[get_parameter_index(8, buf)]);   // �Ƕ�			
    }

    // �ڶ�λû����ЧǰҲ����ʱ�����ݵģ�����ֱ�ӽ���
    gps->time.hour    = (buf[7] - '0') * 10 + (buf[8] - '0');		            // ʱ��
    gps->time.minute  = (buf[9] - '0') * 10 + (buf[10] - '0');
    gps->time.second  = (buf[11] - '0') * 10 + (buf[12] - '0');
    temp = get_parameter_index(9, buf);
    gps->time.day     = (buf[temp + 0] - '0') * 10 + (buf[temp + 1] - '0');     // ����
    gps->time.month   = (buf[temp + 2] - '0') * 10 + (buf[temp + 3] - '0');
    gps->time.year    = (buf[temp + 4] - '0') * 10 + (buf[temp + 5] - '0') + 2000;

    utc_to_btc(&gps->time);

	return return_value;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief		GGA������
// @param		*line	        ���յ��������Ϣ		
// @param		*gps            ��������������
// @return		uint8           1�������ɹ� 0�����������ⲻ�ܽ���
// Sample usage:				
//-------------------------------------------------------------------------------------------------------------------
static uint8 gps_gngga_parse (char *line, gps_info_struct *gps)
{
	uint8 state;
	char *buf = line;
    uint8 return_value = 0;

	state = buf[get_parameter_index(2, buf)];

    if (state != ',')
    {
        gps->satellite_used = get_int_number(&buf[get_parameter_index(7, buf)]);
        gps->height         = get_float_number(&buf[get_parameter_index(9, buf)]) + get_float_number(&buf[get_parameter_index(11, buf)]);  // �߶� = ���θ߶� + ������������Դ��ˮ׼��ĸ߶� 
        return_value = 1;
    }
	
	return return_value;
}


//-------------------------------------------------------------------------------------------------------------------
// @brief		����GPS����
// @param		void
// @return		void
// Sample usage:				gps_data_parse();
//-------------------------------------------------------------------------------------------------------------------
void gps_data_parse (void)
{
	if(gps_tau1201_flag)
    {
		gps_tau1201_flag = 0;

		if(0 == strncmp((char *)&gps_tau1201_buffer1[3], "RMC", 3))
		{
			gps_gnrmc_parse((char *)gps_tau1201_buffer1, &gps_tau1201);
		}
		
		else if(0 == strncmp((char *)&gps_tau1201_buffer1[3], "GGA", 3))
		{
			gps_gngga_parse((char *)gps_tau1201_buffer1, &gps_tau1201);
		}
	}
}
void send_string(char* ch,int num){
	int i = 0;
	while(i < num){
		uart_putchar(UART_1, ch[i]);
		i++;
	}
}

//���ݾ�γ�Ƚ���ѿ������꣬�����temp����
//��˹-������ͶӰ
void GeodeticToCartesian(double longitude,double latitude,double temp[])
{
	double b;//γ�ȶ���
	double L;//���ȶ���
	double L0;//���뾭�߶���
	double L1;//L - L0
	double t;//tanB
	double m;//ltanB
	double N;//î��Ȧ���ʰ뾶
	double q2;
	double X;// ��˹ƽ��������
	double Y;// ��˹ƽ�������
	double s;// �����γ��B�ľ��߻���
	double f; // �ο����������
	double e2;// �����һƫ����
	double a; // �ο������峤����
	//
	double a1;
	double a2;
	double a3;
	double a4;
	double 	 b1;
	double	 b2;
	double	 b3;
	double	 b4;
	double	c0;
	double	c1;
	double	c2;
	double	c3;
	//
	int Datum, prjno, zonewide;
	double IPI;
	//
	Datum = 84;// ͶӰ��׼�����ͣ�����54��׼��Ϊ54������80��׼��Ϊ80��WGS84��׼��Ϊ84
	prjno = 0;// ͶӰ����
	zonewide = 3;
	IPI = 0.0174532925199433333333; // 3.1415926535898/180.0
	b = latitude; //γ��
	L = longitude;//����
	if (zonewide == 6)
	{
		prjno = trunc(L / zonewide) + 1;
		L0 = prjno * zonewide - 3;
	}
	else
	{
		prjno = trunc((L - 1.5) / 3) + 1;
		L0 = prjno * 3;
	}
	if (Datum == 54)
	{
		a = 6378245;
		f = 1 / 298.3;
	}
	else if (Datum == 84)
	{
		a = 6378137;
		f = 1 / 298.257223563;
	}
	//
	L0 = L0 * IPI;
	L = L * IPI;
	b = b * IPI;
 
	e2 = 2 * f - f * f; // (a*a-b*b)/(a*a);
	L1 = L - L0;
	t = tan(b);
	m = L1 * cos(b);
	N = a / sqrt(1 - e2 * sin(b) * sin(b));
	q2 = e2 / (1 - e2) * cos(b) * cos(b);
	a1 = 1 + 3 / 4 * e2 + 45 / 64 * e2 * e2 + 175 / 256 * e2 * e2 * e2 + 11025 /
		16384 * e2 * e2 * e2 * e2 + 43659 / 65536 * e2 * e2 * e2 * e2 * e2;
	a2 = 3 / 4 * e2 + 15 / 16 * e2 * e2 + 525 / 512 * e2 * e2 * e2 + 2205 /
		2048 * e2 * e2 * e2 * e2 + 72765 / 65536 * e2 * e2 * e2 * e2 * e2;
	a3 = 15 / 64 * e2 * e2 + 105 / 256 * e2 * e2 * e2 + 2205 / 4096 * e2 * e2 *
		e2 * e2 + 10359 / 16384 * e2 * e2 * e2 * e2 * e2;
	a4 = 35 / 512 * e2 * e2 * e2 + 315 / 2048 * e2 * e2 * e2 * e2 + 31185 /
		13072 * e2 * e2 * e2 * e2 * e2;
	b1 = a1 * a * (1 - e2);
	b2 = -1 / 2 * a2 * a * (1 - e2);
	b3 = 1 / 4 * a3 * a * (1 - e2);
	b4 = -1 / 6 * a4 * a * (1 - e2);
	c0 = b1;
	c1 = 2 * b2 + 4 * b3 + 6 * b4;
	c2 = -(8 * b3 + 32 * b4);
	c3 = 32 * b4;
	s = c0 * b + cos(b) * (c1 * sin(b) + c2 * sin(b) * sin(b) * sin(b) + c3 * sin
		(b) * sin(b) * sin(b) * sin(b) * sin(b));
	X = s + 1 / 2 * N * t * m * m + 1 / 24 * (5 - t * t + 9 * q2 + 4 * q2 * q2)
		* N * t * m * m * m * m + 1 / 720 * (61 - 58 * t * t + t * t * t * t)
		* N * t * m * m * m * m * m * m;
	Y = N * m + 1 / 6 * (1 - t * t + q2) * N * m * m * m + 1 / 120 *
		(5 - 18 * t * t + t * t * t * t - 14 * q2 - 58 * q2 * t * t)
		* N * m * m * m * m * m;
	Y = Y + 1000000 * prjno + 500000;
	temp[0] = X;
	temp[1] = Y;
}

double home_x,home_y;
double my_x,my_y;

//���ݾ�γ�Ȼ�ȡ���趨��home��Ϊԭ��ĵѿ�������
void GET_POSI(double longitude,double latitude,POSI_ST* posi){
	double temp[2];
	GeodeticToCartesian(longitude,latitude,temp);//����
	posi->x = temp[1] - home_x;//�������home�������
	posi->y = temp[0] - home_y;//�������home�������
}

//��ȡ����ѿ�������
void GET_MY_POSI(POSI_ST* posi){
	GET_POSI(gps_tau1201.longitude,gps_tau1201.latitude,posi);
}

void show_gps(void){
	char x[20],y[20];
	char JIN[20],WEI[20];
	double longitude,latitude;
	POSI_ST posi;
	longitude = gps_tau1201.longitude;
	latitude = gps_tau1201.latitude;
	GET_POSI(longitude,latitude,&posi);
	sprintf(x,"%.2lf",posi.x);
	sprintf(y,"%.2lf",posi.y);
	sprintf(JIN,"%lf",longitude);
	sprintf(WEI,"%lf",latitude);
	lcd_showstr(5,0,(char*)x);
	lcd_showstr(5,1,(char*)y);
	lcd_showstr(10,3,(char*)JIN);
	lcd_showstr(10,5,(char*)WEI);
}

void GPS_INIT(void){
	const uint8 set_rate[]      = {0xF1, 0xD9, 0x06, 0x42, 0x14, 0x00, 0x00, 0x0A, 0x05, 0x00, 0x64, 0x00, 0x00, 0x00, 0x60, 0xEA, 0x00, 0x00, 0xD0, 0x07, 0x00, 0x00, 0xC8, 0x00, 0x00, 0x00, 0xB8, 0xED};
    const uint8 open_gga[]      = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x00, 0x01, 0xFB, 0x10};
    const uint8 open_rmc[]      = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x05, 0x01, 0x00, 0x1A};
    
    const uint8 close_gll[]     = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x01, 0x00, 0xFB, 0x11};
    const uint8 close_gsa[]     = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x02, 0x00, 0xFC, 0x13};
    const uint8 close_grs[]     = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x03, 0x00, 0xFD, 0x15};
    const uint8 close_gsv[]     = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x04, 0x00, 0xFE, 0x17};
    const uint8 close_vtg[]     = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x06, 0x00, 0x00, 0x1B};
    const uint8 close_zda[]     = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x07, 0x00, 0x01, 0x1D};
    const uint8 close_gst[]     = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x08, 0x00, 0x02, 0x1F};
    const uint8 close_txt[]     = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x40, 0x00, 0x3A, 0x8F};
    const uint8 close_txt_ant[] = {0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x20, 0x00, 0x1A, 0x4F};
    
    systick_delay_ms(500);                                                       // �ȴ�GPS������ʼ��ʼ��
    uart_init(GPS_TAU1201_UART, 115200, GPS_TAU1201_RX, GPS_TAU1201_TX);

    uart_putbuff(GPS_TAU1201_UART, (uint8 *)set_rate, sizeof(set_rate));        // ����GPS��������Ϊ10hz ��������ô������Ĭ��Ϊ1hz
    systick_delay_ms(200);
    
    uart_putbuff(GPS_TAU1201_UART, (uint8 *)open_rmc, sizeof(open_rmc));        // ����rmc���
    systick_delay_ms(50);
    uart_putbuff(GPS_TAU1201_UART, (uint8 *)open_gga, sizeof(open_gga));        // ����gga���
    systick_delay_ms(50);
    uart_putbuff(GPS_TAU1201_UART, (uint8 *)close_gll, sizeof(close_gll));
    systick_delay_ms(50);
    uart_putbuff(GPS_TAU1201_UART, (uint8 *)close_gsa, sizeof(close_gsa));
    systick_delay_ms(50);
    uart_putbuff(GPS_TAU1201_UART, (uint8 *)close_grs, sizeof(close_grs));
    systick_delay_ms(50);
    uart_putbuff(GPS_TAU1201_UART, (uint8 *)close_gsv, sizeof(close_gsv));
    systick_delay_ms(50);
    uart_putbuff(GPS_TAU1201_UART, (uint8 *)close_vtg, sizeof(close_vtg));
    systick_delay_ms(50);
    uart_putbuff(GPS_TAU1201_UART, (uint8 *)close_zda, sizeof(close_zda));
    systick_delay_ms(50);
    uart_putbuff(GPS_TAU1201_UART, (uint8 *)close_gst, sizeof(close_gst));
    systick_delay_ms(50);
    uart_putbuff(GPS_TAU1201_UART, (uint8 *)close_txt, sizeof(close_txt));
    systick_delay_ms(50);
    uart_putbuff(GPS_TAU1201_UART, (uint8 *)close_txt_ant, sizeof(close_txt_ant));
    systick_delay_ms(50);

    uart_rx_irq(GPS_TAU1201_UART, 1);
	
	double temp[2];
	GeodeticToCartesian(home_lo,home_la,temp);
	home_x = temp[1];
	home_y = temp[0];
}
