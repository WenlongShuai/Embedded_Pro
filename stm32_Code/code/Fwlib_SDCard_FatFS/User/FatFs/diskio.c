/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

/* Definitions of physical drive number for each drive */
//#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
//#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
//#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

#define SPI_FLASH		0	/* Example: Map spi_flash to physical drive 0 */
#define SD_CARD			1	/* Example: Map sd card to physical drive 1 */

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	SDCardState cardstate =  SD_CARD_TRANSFER; //SD card 初始化完成后处于传输模式

	switch (pdrv) {
	case SPI_FLASH :
		return stat;

	case SD_CARD :
		cardstate = SD_GetState();
		if(cardstate == SD_CARD_TRANSFER)
		{
			stat = STA_NOINIT & (~0x01);
		}
		else
		{
			stat = STA_NOINIT;
		}
		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	SD_Error errorstatus = SD_OK;

	switch (pdrv) {
	case SPI_FLASH :
		return stat;

	case SD_CARD :
		SD_Init();
		errorstatus = SD_EnterCardIdentificationMode();
		if(errorstatus == SD_OK)
		{
			stat = STA_NOINIT & (~0x01);
		}
		else
		{
			stat = STA_NOINIT;
		}
		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
	SD_Error errorstatus = SD_OK;

	switch (pdrv) {
	case SPI_FLASH :
		return res;

	case SD_CARD :
		//字节对齐
		if((DWORD)buff&3)
		{
			DRESULT res = RES_OK;
			DWORD scratch[512 / 4];

			while (count--) 
			{
				res = disk_read(SD_CARD,(void *)scratch, sector++, 1);

				if (res != RES_OK) 
				{
					break;
				}
				memcpy(buff, scratch, 512);
				buff += 512;
			}
			return res;
		}
		
		errorstatus = SD_ReadMultiBlocks((uint8_t*)buff, sector*512, 512, count);
		if(errorstatus == SD_OK)
		{
			errorstatus = SD_WaitReadOperation();
			while(SD_GetStatus() != SD_TRANSFER_OK);
		}
		
		if(errorstatus == SD_OK)
		{
			res = RES_OK;
		}
		else
		{
			res = RES_ERROR;
		}
		return res;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	SD_Error errorstatus = SD_OK;

	switch (pdrv) {
	case SPI_FLASH :
		return res;

	case SD_CARD:
		//字节对齐
		if((DWORD)buff&3)
		{
			DRESULT res = RES_OK;
			DWORD scratch[512 / 4];

			while (count--) 
			{
				memcpy( scratch,buff,512);
				res = disk_write(SD_CARD,(void *)scratch, sector++, 1);
				if (res != RES_OK) 
				{
					break;
				}					
				buff += 512;
			}
			return res;
		}		
		errorstatus = SD_WriteMultiBlocks((uint8_t*)buff, sector*512, 512, count);
		if(errorstatus == SD_OK)
		{
			errorstatus = SD_WaitWriteOperation();
			while(SD_GetStatus() != SD_TRANSFER_OK);
		}
		
		if(errorstatus == SD_OK)
		{
			res = RES_OK;
		}
		else
		{
			res = RES_ERROR;
		}
		return res;
	}

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;

	switch (pdrv) {
	case SPI_FLASH :
		return res;

	case SD_CARD :
		switch(cmd)
		{
			case GET_SECTOR_COUNT:
				*(DWORD*)buff = SDCardInfo.CardCapacity/SDCardInfo.CardBlockSize;
				break;
			case GET_SECTOR_SIZE:
				*(WORD*)buff = 512;
				break;
			case GET_BLOCK_SIZE:
				*(DWORD*)buff = SDCardInfo.CardBlockSize;
				break;
		}
		return RES_OK;
	}

	return RES_PARERR;
}

