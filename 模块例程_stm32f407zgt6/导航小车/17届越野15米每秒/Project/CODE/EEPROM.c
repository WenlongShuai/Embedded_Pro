#include "EEPROM.h"
#define num 60
#define EEPROM_PAGESIZE 	   ((uint16_t)64) //EEPROMһҳ������
#define  DATA_Size			1000
#define  EEP_Firstpage      10000

//ģ��iic��ʼ��
void EEPROM_INIT(void){
	simiic_init();
}

//EEPROMҳд��
void I2C_EE_PageWrite(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite){
	simiic_write_regs(EEPROM_ADDR,WriteAddr,pBuffer,NumByteToWrite);
}

//EEPROM���ֽ�д��
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

//EEPROM���ֽڶ�ȡ����
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

//EEPROM��д���Ժ���
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

//��������֮��ľ��룬�����ж��Ƿ�ɼ��µ�
float GET_DISTANCE(POSI_ST posi,POSI_ST posi_target){
	POSI_ST posi_offset;
	float distance;
	posi_offset.x = posi_target.x - posi.x;
	posi_offset.y = posi_target.y - posi.y;
	distance = pow((pow(posi_offset.x,2)+pow(posi_offset.y,2)),0.5);
	return distance;
}

//�켣�ɼ����ɼ�ʱ���ų���һ����Ҫ���ٵĹ켣��������Զ���һ�ξ��뽫�������EEPROM
void EEPROM_COLLECT(void){
	int mode;
	static int step = 0;
	static uint16 addr = 100;//EEPROM�洢��ַ
	static uint16 NUM = 0,NUM2 = 0;
	static POSI_ST posi_current;//��ǰ�ɼ����ĵ�
	static POSI_ST posi_last;//��һ�βɼ����ĵ�
	static POSI_ST posi_first;//�ɼ��ĵ�һ����
	static POSI_ST posi_sh;//����Ļ����ʾ�����ĵ�
	static POSI_UNION posi;//����union��ʵ�ֽṹ����ֽڵ��໥ת��
	static POSI_UNION posi_show;//������ʾ
	char str[10][20];
	mode = get_mode();//�������ǹر�
	if(mode == 1){//�����ɼ�
		GET_MY_POSI(&posi_current);//��ȡ��ǰλ��
        
		switch(step){//��һ�βɵ�û����һ���㣬��Ϊ�˼�¼��һ���㣬�ֳ����ν���
			case 0:{
				posi.wgs84.latitude = gps_tau1201.latitude;//����γ������union��
				posi.wgs84.longitude = gps_tau1201.longitude;
                
				I2C_EE_BufferWrite(posi.data, addr, sizeof(WGS84_T));//д��EEPROM���ظ�д����
				systick_delay_ms(10);
				I2C_EE_BufferRead(posi_show.data,addr,sizeof(WGS84_T));
				systick_delay_ms(10);
                
				GET_POSI(posi_show.wgs84.longitude,posi_show.wgs84.latitude,&posi_sh);//תΪ�ѿ������������ʾ
				
				addr += sizeof(WGS84_T);//��ַ˳��
				NUM++;//����+1
                
				POSI_COPY(&posi_last,posi_current);//��ǰ���Ϊ��һ����
				POSI_COPY(&posi_first,posi_current);//��¼��һ����
				step = 1;//������һ�׶�
			}break;
			case 1:{
				if(GET_DISTANCE(posi_current,posi_last) >= 5 && GET_DISTANCE(posi_current,posi_last) < 20 ){//��ǰ�������һ����ﵽ5�򽫵�ǰ����Ϊ�µ㣬<20��������Ϊ�˷�ֹ�������ݱ��ɼ�
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
                
                //��ǰ�����һ�������С��5��ʱ��Ϊ�ɼ���ɣ���ʱ�����������ٳ�������������EEPROM�ĵ�ַ0x00��������ȡ��
				if(NUM > 60 && GET_DISTANCE(posi_current,posi_first) <= 5){
					I2C_EE_BufferWrite((uint8*)&NUM, 0, 1);
					step = 2;//����switch���˴��и�bug����������²ɼ������븴λ����������ϵ�
				}
				
			}break;
		}
//		����Ļ����ʾ��Ϣ
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

//����������������ȫ�������������ڷ�����PC������matlab������ƹ켣
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

//��ȡEEPROM�еĵ�i������
void EEPROM_GET_POSI(POSI_ST* posi,uint16 i){
	POSI_UNION posi_now;
	I2C_EE_BufferRead(posi_now.data,100 + i*sizeof(WGS84_T),sizeof(WGS84_T));//100Ϊ��ʼ���ĵ�ַ
	systick_delay_ms(10);
	GET_POSI(posi_now.wgs84.longitude,posi_now.wgs84.latitude,posi);//��γ��תΪ�ѿ�������
}

//��ȡ�ɼ��ĵ���
void GET_NUM(uint8* n){
	I2C_EE_BufferRead(n,0,1);//��������0x00��ַ��
}