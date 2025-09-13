#include <reg51.h>

//I2C 与 EEPROM 串行通信

sbit EEPROM_SCL = P2^1;
sbit EEPROM_SDA = P2^0;

typedef unsigned char uchar;  //类型重命名
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
	uchar readArr[8] = {0};   //从EEPROM中接收顺序读（页读）的数据
	uchar writeArr[8] = {0x00,0xfc,0xf8,0xf0,0xe0,0xc0,0x80,0x00};  //向EEPROM按页写入的数据
	uchar readData = 0;  //从EEPROM中随机读的数据
	//eepromWriteByte(0xa0, 1, 0x55);  //向EEPROM的0x01的位置写入一个0x55的数据
	eepromWritePage(0xa0, 0, writeArr, 8);  //向EEPROM的0x00位置按页写入数据（同时写入8个字节的数据，8个字节为一页）
	
	delayMs(1000);  //等待写入完成
	
	//readData = eepromReadCurrentAddr(0xa1);  //从EEPROM的当前位置读（内部地址计数器保存着上次访问时最后一个地址+1的值）
	//readData = eepromReadRandomAddr(0xa1, 1);  //从EEPROM的0x01位置上读取一个字节的数据
	eepromReadSequentialAddr(0xa1, 0, readArr, 3);  //从EEPROM的0x00位置上顺序读取3个字节大小的数据
	
	//点亮LED
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
* 函 数 名       : iiciicStartCondition
* 函数功能		 		 : 产生IIC起始信号
* 输    入       : 无
* 输    出    	 : 无
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
* 函 数 名       : iiciicStopCondition
* 函数功能		 		 : 产生IIC停止信号
* 输    入       : 无
* 输    出    	 : 无
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
* 函 数 名       : iicAck
* 函数功能		 		 : 产生主机的ACK应答信号
* 输    入       : 无
* 输    出    	 : 无
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
* 函 数 名       : iicNoAck
* 函数功能		 		 : 产生主机的NoACK 非应答信号
* 输    入       : 无
* 输    出    	 : 无
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
* 函 数 名       : iicWaitACK
* 函数功能		 		 : 等待从机产生应答信号，如果SDA线为0时，说明从机设备产生应答，如果200us后SDA还没为0，说明从机设备产生非应答，直接发送停止信号。
* 输    入       : 无
* 输    出    	 : 无
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
* 函 数 名       : iicWriteByte
* 函数功能		 		 : I2C 写入一个字节
* 输    入       : 写入的数据
* 输    出    	 : 无
*******************************************************************************/
void iicWriteByte(uchar byteData)
{
	char i = 0;  //这里必须是有符号类型，如果写成无符号类型那么--到-1的时候，编译器就会把-1编程255，会一直卡在for循环中
	uchar temp = byteData;
	for(i = 7; i >=0;i--)
	{
		temp <<= 1;
		EEPROM_SCL = 0;
		delay_us(2);
		EEPROM_SDA = (byteData >> i) & 1;
		//EEPROM_SDA = CY;  //CY是溢出标志，左移之后，最高位会放在PSW(状态寄存器)的CY位
		delay_us(2);
		EEPROM_SCL = 1;
		delay_us(2);
	}
}


/*******************************************************************************
* 函 数 名       : eepromWriteByte
* 函数功能		 		 : 向EEPROM中写入一个字节大小的数据
* 输    入       : deviceWriteAddr:从机设备地址+写操作
										wordAddr:写入EEPROM的地址
										dat:写入EEPROM的数据
* 输    出    	 : 无
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
* 函 数 名       : eepromWritePage
* 函数功能		 		 : 向EEPROM中写入一页（8字节）大小的数据
* 输    入       : deviceWriteAddr:从机设备地址+写操作
										wordAddr:写入EEPROM的地址
										writeDat:写入EEPROM一页的全部数据（数组）
										len:写入EEPROM一页数据的长度（8）
* 输    出    	 : 无
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
* 函 数 名       : iicReadByte
* 函数功能		 		 : 从I2C 上读取一个字节大小的数据
* 输    入       : 无
* 输    出    	 : 从I2C 上读取的数据
*******************************************************************************/
uchar iicReadByte()
{
	uchar readByte = 0;
	uint i = 0;
	EEPROM_SDA = 1;  //主机在读之前先把SDA线拉高，让总线处于空闲状态
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
	
	EEPROM_SCL = 0;  //I2C读完一字节数据之后，应该把SCL线拉低，以便SDA可以改变数据
	delay_us(2);
	return readByte;
}

/*******************************************************************************
* 函 数 名       : eepromReadCurrentAddr
* 函数功能		 		 : 从EEPROM的当前地址读数据（内部地址计数器保存着上次访问时最后一个地址+1的值）
* 输    入       : deviceReadAddr:从机设别地址+读操作
* 输    出    	 : 从EEPROM中读取的数据
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
* 函 数 名       : eepromReadRandomAddr
* 函数功能		 		 : 从EEPROM的随机地址读数据
* 输    入       : deviceReadAddr:从机设别地址+读操作
										wordAddr:从EEPROM中的哪个地址读数据
* 输    出    	 : 从EEPROM中wordAddr地址中读取的数据
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
* 函 数 名       : eepromReadSequentialAddr
* 函数功能		 		 : 从EEPROM的顺序读数据
* 输    入       : deviceReadAddr:从机设别地址+读操作
										wordAddr:从EEPROM中的哪个地址开始读数据
										readDat:顺序读的数据放在哪个地方，这里用数组来存放数据
										readDataLen:顺序读取的字节长度
* 输    出    	 : 从EEPROM中wordAddr地址中顺序读取readDataLen个数据
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
* 函 数 名       : delay_us
* 函数功能		 		 : 延时函数，us=1,延时3us
* 输    入       : 延时的时间
* 输    出    	 : 无
*******************************************************************************/
void delay_us(uchar us)
{
	while(us--);
}

/*******************************************************************************
* 函 数 名       : delayMs
* 函数功能		 		 : 延时函数，ms=1,延时1ms
* 输    入       : 延时的时间
* 输    出    	 : 无
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
