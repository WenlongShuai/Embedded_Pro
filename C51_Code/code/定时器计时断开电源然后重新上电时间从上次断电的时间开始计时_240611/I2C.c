#include "I2C.h"

/*******************************************************************************
* �� �� ��       : iiciicStartCondition
* ��������		 		 : ����IIC��ʼ�ź�
* ��    ��       : ��
* ��    ��    	 : ��
*******************************************************************************/
void iicStartCondition()
{
	EEPROM_SDA = 1;
	delay_us(2);
	EEPROM_SCL = 1;
	delay_us(2);
	EEPROM_SDA = 0;
	delay_us(2);
	EEPROM_SCL = 0;
	delay_us(2);
}

/*******************************************************************************
* �� �� ��       : iiciicStopCondition
* ��������		 		 : ����IICֹͣ�ź�
* ��    ��       : ��
* ��    ��    	 : ��
*******************************************************************************/
void iicStopCondition()
{
	EEPROM_SDA = 0;
	delay_us(2);
	EEPROM_SCL = 0;
	delay_us(2);
	EEPROM_SCL = 1;
	delay_us(2);
	EEPROM_SDA = 1;
	delay_us(2);
}

/*******************************************************************************
* �� �� ��       : iicAck
* ��������		 		 : ����������ACKӦ���ź�
* ��    ��       : ��
* ��    ��    	 : ��
*******************************************************************************/
void iicAck()
{
	EEPROM_SDA = 0;
	delay_us(2);
	EEPROM_SCL = 1;
	delay_us(2);
	EEPROM_SCL = 0;
	delay_us(2);
}


/*******************************************************************************
* �� �� ��       : iicNoAck
* ��������		 		 : ����������NoACK ��Ӧ���ź�
* ��    ��       : ��
* ��    ��    	 : ��
*******************************************************************************/
void iicNoAck()
{
	EEPROM_SDA = 1;
	delay_us(2);
	EEPROM_SCL = 1;
	delay_us(2);
	EEPROM_SCL = 0;
	delay_us(2);
}

/*******************************************************************************
* �� �� ��       : iicWaitACK
* ��������		 		 : �ȴ��ӻ�����Ӧ���źţ����SDA��Ϊ0ʱ��˵���ӻ��豸����Ӧ�����200us��SDA��ûΪ0��˵���ӻ��豸������Ӧ��ֱ�ӷ���ֹͣ�źš�
* ��    ��       : ��
* ��    ��    	 : ��
*******************************************************************************/
void iicWaitACK()
{
	uint i = 0;
	EEPROM_SCL = 0;
	delay_us(2);
	EEPROM_SCL = 1;
	delay_us(2);

	while(EEPROM_SDA)
	{
		i++;
		if(i > 200)
		{
			iicStopCondition();
			return;
		}
	}
	EEPROM_SCL = 0;
	delay_us(2);
}

/*******************************************************************************
* �� �� ��       : iicWriteByte
* ��������		 		 : I2C д��һ���ֽ�
* ��    ��       : д�������
* ��    ��    	 : ��
*******************************************************************************/
void iicWriteByte(uchar byteData)
{
	char i = 0;  //����������з������ͣ����д���޷���������ô--��-1��ʱ�򣬱������ͻ��-1���255����һֱ����forѭ����
	uchar temp = byteData;
	for(i = 7; i >=0;i--)
	{
		temp <<= 1;
		EEPROM_SCL = 0;
		delay_us(2);
		EEPROM_SDA = (byteData >> i) & 1;
		//EEPROM_SDA = CY;  //CY�������־������֮�����λ�����PSW(״̬�Ĵ���)��CYλ
		delay_us(2);
		EEPROM_SCL = 1;
		delay_us(2);
	}
}


/*******************************************************************************
* �� �� ��       : eepromWriteByte
* ��������		 		 : ��EEPROM��д��һ���ֽڴ�С������
* ��    ��       : deviceWriteAddr:�ӻ��豸��ַ+д����
										wordAddr:д��EEPROM�ĵ�ַ
										dat:д��EEPROM������
* ��    ��    	 : ��
*******************************************************************************/
void eepromWriteByte(uchar deviceWriteAddr, uchar wordAddr, uchar dat)
{
	iicStartCondition();
	iicWriteByte(deviceWriteAddr);
	iicWaitACK();
	iicWriteByte(wordAddr);
	iicWaitACK();
	iicWriteByte(dat);
	iicWaitACK();
	iicStopCondition();
}


/*******************************************************************************
* �� �� ��       : eepromWritePage
* ��������		 		 : ��EEPROM��д��һҳ��8�ֽڣ���С������
* ��    ��       : deviceWriteAddr:�ӻ��豸��ַ+д����
										wordAddr:д��EEPROM�ĵ�ַ
										writeDat:д��EEPROMһҳ��ȫ�����ݣ����飩
										len:д��EEPROMһҳ���ݵĳ��ȣ�8��
* ��    ��    	 : ��
*******************************************************************************/
void eepromWritePage(uchar deviceWriteAddr, uchar wordAddr,uchar *writeDat, uchar len)
{
	uint i = 0;
	iicStartCondition();
	iicWriteByte(deviceWriteAddr);
	iicWaitACK();
	iicWriteByte(wordAddr);
	iicWaitACK();
	for(i = 0;i < len;i++)
	{
		iicWriteByte(writeDat[i]);
		iicWaitACK();
	}
	iicStopCondition();
}


/*******************************************************************************
* �� �� ��       : iicReadByte
* ��������		 		 : ��I2C �϶�ȡһ���ֽڴ�С������
* ��    ��       : ��
* ��    ��    	 : ��I2C �϶�ȡ������
*******************************************************************************/
uchar iicReadByte()
{
	uchar readByte = 0;
	uint i = 0;
	EEPROM_SDA = 1;  //�����ڶ�֮ǰ�Ȱ�SDA�����ߣ������ߴ��ڿ���״̬
	delay_us(2);
	
	for(i = 0;i < 8;i++)
	{
		readByte <<= 1;
		EEPROM_SCL = 0;
		delay_us(2);
		EEPROM_SCL = 1;
		delay_us(2);
		readByte |= EEPROM_SDA;
		delay_us(2);
	}
	
	EEPROM_SCL = 0;  //I2C����һ�ֽ�����֮��Ӧ�ð�SCL�����ͣ��Ա�SDA���Ըı�����
	delay_us(2);
	return readByte;
}

/*******************************************************************************
* �� �� ��       : eepromReadCurrentAddr
* ��������		 		 : ��EEPROM�ĵ�ǰ��ַ�����ݣ��ڲ���ַ�������������ϴη���ʱ���һ����ַ+1��ֵ��
* ��    ��       : deviceReadAddr:�ӻ�����ַ+������
* ��    ��    	 : ��EEPROM�ж�ȡ������
*******************************************************************************/
uchar eepromReadCurrentAddr(uchar deviceReadAddr)
{
	uchar ReadCurrentAddr = 0;
	iicStartCondition();
	iicWriteByte(deviceReadAddr);
	iicWaitACK();
	ReadCurrentAddr = iicReadByte();
	iicStopCondition();
	
	return ReadCurrentAddr;
}

/*******************************************************************************
* �� �� ��       : eepromReadRandomAddr
* ��������		 		 : ��EEPROM�������ַ������
* ��    ��       : deviceReadAddr:�ӻ�����ַ+������
										wordAddr:��EEPROM�е��ĸ���ַ������
* ��    ��    	 : ��EEPROM��wordAddr��ַ�ж�ȡ������
*******************************************************************************/
uchar eepromReadRandomAddr(uchar deviceReadAddr, uchar wordAddr)
{
	uchar ReadRandomAddr = 0;
	iicStartCondition();
	iicWriteByte(0xa0);
	iicWaitACK();
	iicWriteByte(wordAddr);
	iicWaitACK();
	
	iicStartCondition();
	iicWriteByte(deviceReadAddr);
	iicWaitACK();
	
	ReadRandomAddr = iicReadByte();

	iicStopCondition();
	
	return ReadRandomAddr;
}

/*******************************************************************************
* �� �� ��       : eepromReadSequentialAddr
* ��������		 		 : ��EEPROM��˳�������
* ��    ��       : deviceReadAddr:�ӻ�����ַ+������
										wordAddr:��EEPROM�е��ĸ���ַ��ʼ������
										readDat:˳��������ݷ����ĸ��ط����������������������
										readDataLen:˳���ȡ���ֽڳ���
* ��    ��    	 : ��EEPROM��wordAddr��ַ��˳���ȡreadDataLen������
*******************************************************************************/
void eepromReadSequentialAddr(uchar deviceReadAddr, uchar wordAddr,uchar *readDat, uchar readDataLen)
{
	uint i = 0;
	iicStartCondition();
	iicWriteByte(0xa0);
	iicWaitACK();
	iicWriteByte(wordAddr);
	iicWaitACK();
	
	iicStartCondition();
	iicWriteByte(deviceReadAddr);
	iicWaitACK();
	
	for(i = 0;i < readDataLen;i++)
	{
		readDat[i] = iicReadByte();
		iicAck();
	}
	iicNoAck();
	iicStopCondition();
}

/*******************************************************************************
* �� �� ��       : delay_us
* ��������		 		 : ��ʱ������us=1,��ʱ3us
* ��    ��       : ��ʱ��ʱ��
* ��    ��    	 : ��
*******************************************************************************/
void delay_us(uchar us)
{
	while(us--);
}

/*******************************************************************************
* �� �� ��       : delayMs
* ��������		 		 : ��ʱ������ms=1,��ʱ1ms
* ��    ��       : ��ʱ��ʱ��
* ��    ��    	 : ��
*******************************************************************************/
void delayMs(uint ms)
{
	uint i = 0;
	uint j = 0;
	for(i = 0;i < ms;i++)
	{
		for(j = 0;j < 130;j++);
	}
}
