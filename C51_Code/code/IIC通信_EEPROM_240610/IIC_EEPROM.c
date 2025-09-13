#include <reg51.h>

//I2C �� EEPROM ����ͨ��

sbit EEPROM_SCL = P2^1;
sbit EEPROM_SDA = P2^0;

typedef unsigned char uchar;  //����������
typedef unsigned int uint;

void eepromWriteByte(uchar deviceWriteAddr, uchar wordAddr, uchar dat);
void eepromWritePage(uchar deviceWriteAddr, uchar wordAddr, uchar *writeDat,uchar len);
uchar eepromReadCurrentAddr(uchar deviceReadAddr);
uchar eepromReadRandomAddr(uchar deviceReadAddr, uchar wordAddr);
void eepromReadSequentialAddr(uchar deviceReadAddr, uchar wordAddr,uchar *readDat,uchar readDataLen);
void delay_us(uchar us);
void delayMs(uint ms);

int main()
{
	uchar i = 0;
	uchar readArr[8] = {0};   //��EEPROM�н���˳�����ҳ����������
	uchar writeArr[8] = {0x00,0xfc,0xf8,0xf0,0xe0,0xc0,0x80,0x00};  //��EEPROM��ҳд�������
	uchar readData = 0;  //��EEPROM�������������
	//eepromWriteByte(0xa0, 1, 0x55);  //��EEPROM��0x01��λ��д��һ��0x55������
	eepromWritePage(0xa0, 0, writeArr, 8);  //��EEPROM��0x00λ�ð�ҳд�����ݣ�ͬʱд��8���ֽڵ����ݣ�8���ֽ�Ϊһҳ��
	
	delayMs(1000);  //�ȴ�д�����
	
	//readData = eepromReadCurrentAddr(0xa1);  //��EEPROM�ĵ�ǰλ�ö����ڲ���ַ�������������ϴη���ʱ���һ����ַ+1��ֵ��
	//readData = eepromReadRandomAddr(0xa1, 1);  //��EEPROM��0x01λ���϶�ȡһ���ֽڵ�����
	eepromReadSequentialAddr(0xa1, 0, readArr, 3);  //��EEPROM��0x00λ����˳���ȡ3���ֽڴ�С������
	
	//����LED
	for(i = 0;i < 8;i++)
	{
		P2 = readArr[i];
		delayMs(1000);
	}
	
	while(1)
	{
		//P2 = readData;
	}

	return 0;
}

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
