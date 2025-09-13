#include "EEPROM.h"
#define num 60
#define EEPROM_PAGESIZE 	   ((uint16_t)64) //EEPROM一页的容量
#define  DATA_Size			1000
#define  EEP_Firstpage      10000

//模拟iic初始化
void EEPROM_INIT(void){
	simiic_init();
}

//EEPROM页写入
void I2C_EE_PageWrite(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite){
	simiic_write_regs(EEPROM_ADDR,WriteAddr,pBuffer,NumByteToWrite);
}

//EEPROM多字节写入
void I2C_EE_BufferWrite(uint8_t* pBuffer, uint16 WriteAddr, uint16_t NumByteToWrite)
{
  uint16_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;
  Addr = WriteAddr % EEPROM_PAGESIZE;
  count = EEPROM_PAGESIZE - Addr;
  NumOfPage =  NumByteToWrite / EEPROM_PAGESIZE;
  NumOfSingle = NumByteToWrite % EEPROM_PAGESIZE;
	
  /* If WriteAddr is I2C_PageSize aligned  */
  if(Addr == 0) 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage == 0) 
    {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
    /* If NumByteToWrite > I2C_PageSize */
    else  
    {
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, EEPROM_PAGESIZE); 
        WriteAddr +=  EEPROM_PAGESIZE;
        pBuffer += EEPROM_PAGESIZE;
      }

      if(NumOfSingle!=0)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
  /* If WriteAddr is not I2C_PageSize aligned  */
  else 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage== 0) 
    {
		if(Addr + NumOfSingle > EEPROM_PAGESIZE){
			I2C_EE_PageWrite(pBuffer, WriteAddr, count);
			pBuffer += count;
			WriteAddr += count;
			count = NumOfSingle - count;
			systick_delay_ms(EEPROM_DELAY_TIME);
			I2C_EE_PageWrite(pBuffer, WriteAddr, count);
		}
		else{
			I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
		}
    }
    /* If NumByteToWrite > I2C_PageSize */
    else
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / EEPROM_PAGESIZE;
      NumOfSingle = NumByteToWrite % EEPROM_PAGESIZE;	
      
      if(count != 0)
      {  
        I2C_EE_PageWrite(pBuffer, WriteAddr, count);
		systick_delay_ms(EEPROM_DELAY_TIME);
        WriteAddr += count;
        pBuffer += count;
      } 
      
      while(NumOfPage--)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, EEPROM_PAGESIZE);
		systick_delay_ms(EEPROM_DELAY_TIME);
        WriteAddr +=  EEPROM_PAGESIZE;
        pBuffer += EEPROM_PAGESIZE;  
      }
      if(NumOfSingle != 0)
      {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle); 
		systick_delay_ms(EEPROM_DELAY_TIME);
      }
    }
  }  
  
}

//EEPROM多字节读取函数
void I2C_EE_BufferRead(uint8_t* pBuffer, uint16 ReadAddr, uint16_t NumByteToRead){
	uint16_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;

  Addr = ReadAddr % EEPROM_PAGESIZE;
  count = EEPROM_PAGESIZE - Addr;
  NumOfPage =  NumByteToRead / EEPROM_PAGESIZE;
  NumOfSingle = NumByteToRead % EEPROM_PAGESIZE;
 
  /* If WriteAddr is I2C_PageSize aligned  */
  if(Addr == 0) 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage == 0) 
    {
      simiic_read_regs(EEPROM_ADDR,ReadAddr,pBuffer,  NumOfSingle,SIMIIC);
	  systick_delay_ms(EEPROM_DELAY_TIME);
    }
    /* If NumByteToWrite > I2C_PageSize */
    else  
    {
      while(NumOfPage--)
      {
        simiic_read_regs(EEPROM_ADDR,ReadAddr,pBuffer,  EEPROM_PAGESIZE,SIMIIC); 
		systick_delay_ms(EEPROM_DELAY_TIME);
        ReadAddr +=  EEPROM_PAGESIZE;
        pBuffer += EEPROM_PAGESIZE;
      }

      if(NumOfSingle!=0)
      {
        simiic_read_regs(EEPROM_ADDR,ReadAddr,pBuffer,  NumOfSingle,SIMIIC);
		systick_delay_ms(EEPROM_DELAY_TIME);
      }
    }
  }
  /* If WriteAddr is not I2C_PageSize aligned  */
  else 
  {
    /* If NumByteToWrite < I2C_PageSize */
    if(NumOfPage== 0) 
    {
		if(Addr + NumOfSingle > EEPROM_PAGESIZE){
//			printf("fb");
			simiic_read_regs(EEPROM_ADDR,ReadAddr,pBuffer,  count,SIMIIC);
			systick_delay_ms(EEPROM_DELAY_TIME);
			pBuffer += count;
			ReadAddr += count;
			count = NumOfSingle - count;
			
			simiic_read_regs(EEPROM_ADDR,ReadAddr,pBuffer,  count,SIMIIC);
			systick_delay_ms(EEPROM_DELAY_TIME);
		}
		else{
			simiic_read_regs(EEPROM_ADDR,ReadAddr,pBuffer,  NumOfSingle,SIMIIC);
			systick_delay_ms(EEPROM_DELAY_TIME);
		}	
    }
    /* If NumByteToWrite > I2C_PageSize */
    else
    {
      NumByteToRead -= count;
      NumOfPage =  NumByteToRead / EEPROM_PAGESIZE;
      NumOfSingle = NumByteToRead % EEPROM_PAGESIZE;	
      
      if(count != 0)
      {  
        simiic_read_regs(EEPROM_ADDR,ReadAddr,pBuffer,  count,SIMIIC);
		systick_delay_ms(EEPROM_DELAY_TIME);
        ReadAddr += count;
        pBuffer += count;
      } 
      
      while(NumOfPage--)
      {
        simiic_read_regs(EEPROM_ADDR,ReadAddr,pBuffer,  EEPROM_PAGESIZE,SIMIIC);
		systick_delay_ms(EEPROM_DELAY_TIME);
        ReadAddr +=  EEPROM_PAGESIZE;
        pBuffer += EEPROM_PAGESIZE;  
      }
      if(NumOfSingle != 0)
      {
        simiic_read_regs(EEPROM_ADDR,ReadAddr,pBuffer, NumOfSingle,SIMIIC); 
		systick_delay_ms(EEPROM_DELAY_TIME);
      }
    }
  }  
}

//EEPROM读写测试函数
void EEPROM_TEST(void){
	POSI_UNION my_posi0;
	POSI_UNION my_posi1;
	int i = 0;
	while(i < DATA_Size){
		my_posi0.wgs84.longitude = 456.462546;
		my_posi0.wgs84.latitude = 2345.234;
		I2C_EE_BufferWrite( my_posi0.data, 356 + i*sizeof(WGS84_T), sizeof(WGS84_T));
		systick_delay_ms(1);
		i++;
	}
	i = 0;
	while(i < DATA_Size){
		I2C_EE_BufferRead( my_posi1.data, 356 + i*sizeof(WGS84_T), sizeof(WGS84_T));
		printf("%.20lf  %.20lf\r\n",my_posi1.wgs84.longitude,my_posi1.wgs84.latitude);
		systick_delay_ms(1);
		i++;
	}
}

//计算两点之间的距离，用于判断是否采集新点
float GET_DISTANCE(POSI_ST posi,POSI_ST posi_target){
	POSI_ST posi_offset;
	float distance;
	posi_offset.x = posi_target.x - posi.x;
	posi_offset.y = posi_target.y - posi.y;
	distance = pow((pow(posi_offset.x,2)+pow(posi_offset.y,2)),0.5);
	return distance;
}

//轨迹采集，采集时抱着车走一遍需要跟踪的轨迹，程序会自动隔一段距离将坐标存入EEPROM
void EEPROM_COLLECT(void){
	int mode;
	static int step = 0;
	static uint16 addr = 100;//EEPROM存储地址
	static uint16 NUM = 0,NUM2 = 0;
	static POSI_ST posi_current;//当前采集到的点
	static POSI_ST posi_last;//上一次采集到的点
	static POSI_ST posi_first;//采集的第一个点
	static POSI_ST posi_sh;//在屏幕上显示出来的点
	static POSI_UNION posi;//采用union将实现结构体和字节的相互转换
	static POSI_UNION posi_show;//用于显示
	char str[10][20];
	mode = get_mode();//启动还是关闭
	if(mode == 1){//启动采集
		GET_MY_POSI(&posi_current);//获取当前位置
        
		switch(step){//第一次采点没有上一个点，且为了记录第一个点，分成两段进行
			case 0:{
				posi.wgs84.latitude = gps_tau1201.latitude;//将经纬度填入union中
				posi.wgs84.longitude = gps_tau1201.longitude;
                
				I2C_EE_BufferWrite(posi.data, addr, sizeof(WGS84_T));//写入EEPROM，重复写两次
				systick_delay_ms(10);
				I2C_EE_BufferRead(posi_show.data,addr,sizeof(WGS84_T));
				systick_delay_ms(10);
                
				GET_POSI(posi_show.wgs84.longitude,posi_show.wgs84.latitude,&posi_sh);//转为笛卡尔坐标进行显示
				
				addr += sizeof(WGS84_T);//地址顺延
				NUM++;//点数+1
                
				POSI_COPY(&posi_last,posi_current);//当前点成为上一个点
				POSI_COPY(&posi_first,posi_current);//记录第一个点
				step = 1;//进入下一阶段
			}break;
			case 1:{
				if(GET_DISTANCE(posi_current,posi_last) >= 5 && GET_DISTANCE(posi_current,posi_last) < 20 ){//当前点距离上一个点达到5则将当前点作为新点，<20的条件是为了防止错误数据被采集
					posi.wgs84.latitude = gps_tau1201.latitude;
					posi.wgs84.longitude = gps_tau1201.longitude;
					I2C_EE_BufferWrite(posi.data, addr, sizeof(WGS84_T));
					systick_delay_ms(10);
					I2C_EE_BufferRead(posi_show.data,addr,sizeof(WGS84_T));
					systick_delay_ms(10);
					GET_POSI(posi_show.wgs84.longitude,posi_show.wgs84.latitude,&posi_sh);
					
					addr += sizeof(WGS84_T);
					NUM++;
					POSI_COPY(&posi_last,posi_current);
				}
                
                //当前点与第一个点距离小于5米时认为采集完成，此时走完了整个操场，将点数存入EEPROM的地址0x00处，方便取用
				if(NUM > 60 && GET_DISTANCE(posi_current,posi_first) <= 5){
					I2C_EE_BufferWrite((uint8*)&NUM, 0, 1);
					step = 2;//跳出switch，此处有个bug，如果想重新采集，必须复位程序或重新上电
				}
				
			}break;
		}
//		在屏幕上显示信息
		sprintf(str[0],"%d",NUM);
		sprintf(str[1],"%f",posi_show.wgs84.longitude);
		sprintf(str[2],"%f",posi_show.wgs84.latitude);
		lcd_showstr(5,0,(char*)str[0]);
		lcd_showstr(5,1,(char*)str[1]);
		lcd_showstr(5,2,(char*)str[2]);	
		Remote_Ctrl();
	}
	else if(mode == 0){
//		Remote_Ctrl();
	}
	sprintf(str[3],"%f",gps_tau1201.latitude);
	sprintf(str[4],"%f",gps_tau1201.longitude);
	lcd_showstr(5,3,(char*)str[3]);
	lcd_showstr(5,4,(char*)str[4]);	
	
}

//将点数和所有坐标全部读出，经串口发送至PC，可用matlab程序绘制轨迹
void EEPROM_READ(void){
	static uint16 ADDR = 100;
	static POSI_UNION posi_show;
	static POSI_ST posi_sh;
	uint8 num0 = 0;
	uint8 i = 1;
	I2C_EE_BufferRead(&num0,0,1);
	printf("#%d#\r\n",num0);
	while(i <= num0){
		I2C_EE_BufferRead(posi_show.data,ADDR,sizeof(WGS84_T));
		systick_delay_ms(10);
		GET_POSI(posi_show.wgs84.longitude,posi_show.wgs84.latitude,&posi_sh);
		printf("%.2f %.2f\r\n",posi_sh.x,posi_sh.y);
		i++;
		ADDR += sizeof(WGS84_T);
	}
}

//获取EEPROM中的第i个坐标
void EEPROM_GET_POSI(POSI_ST* posi,uint16 i){
	POSI_UNION posi_now;
	I2C_EE_BufferRead(posi_now.data,100 + i*sizeof(WGS84_T),sizeof(WGS84_T));//100为开始存点的地址
	systick_delay_ms(10);
	GET_POSI(posi_now.wgs84.longitude,posi_now.wgs84.latitude,posi);//经纬度转为笛卡尔坐标
}

//获取采集的点数
void GET_NUM(uint8* n){
	I2C_EE_BufferRead(n,0,1);//点数存在0x00地址处
}