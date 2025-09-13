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
#include "bsp_spi_flash.h" /* Declarations of SPI_FLASH functions */
#include "bsp_usart.h"

#define DEV_RAM_MMC_USB 0

/* Definitions of physical drive number for each drive */
#if DEV_RAM_MMC_USB
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

#else

#define SPI_FLASH 0
#define SD_CARD   1
#endif


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	DWORD spi_flash_id = 0;
	#if DEV_RAM_MMC_USB
	int result;
	#endif

	switch (pdrv) 
	{
	#if DEV_RAM_MMC_USB
		case DEV_RAM :
			result = RAM_disk_status();
			// translate the reslut code here
			return stat;
		case DEV_MMC :
			result = MMC_disk_status();
			// translate the reslut code here
			return stat;
		case DEV_USB :
			result = USB_disk_status();
			// translate the reslut code here
			return stat;
	#else
		case SD_CARD:
			
			break;
		case SPI_FLASH:
			spi_flash_id = SPI_Flash_Read_JEDECId();
			if(spi_flash_id == FLASH_ID)
				stat = STA_NORMAL;
			else
				stat = STA_NODISK;
			return stat;
	#endif
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	DSTATUS flag = 0;
	#if DEV_RAM_MMC_USB
	int result;
	#endif

	switch (pdrv) {
	#if DEV_RAM_MMC_USB
	case DEV_RAM :
		result = RAM_disk_initialize();
		// translate the reslut code here
		return stat;
	case DEV_MMC :
		result = MMC_disk_initialize();
		// translate the reslut code here
		return stat;
	case DEV_USB :
		result = USB_disk_initialize();
		// translate the reslut code here
		return stat;
	#else
	case SD_CARD:
		break;
	case SPI_FLASH:
		SPI_GPIO_Init();
		SPI_Flash_WaitForBUSY();
		SPI_Flash_ReleasePowerDown();  //唤醒SPI_FLASH
		flag = SPI_Flash_ReadStatusReg();
		if(flag & 0x02 == 0x00)  //SPI_FLASH Write disable
			stat = STA_PROTECT;
		else
			stat = disk_status(SPI_FLASH);
		return stat;
	#endif
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
	DSTATUS flag;
	#if DEV_RAM_MMC_USB
	int result;
	#endif

	switch (pdrv) {
	#if DEV_RAM_MMC_USB
	case DEV_RAM :
		// translate the arguments here
		result = RAM_disk_read(buff, sector, count);
		// translate the reslut code here
		return res;
	case DEV_MMC :
		// translate the arguments here
		result = MMC_disk_read(buff, sector, count);
		// translate the reslut code here
		return res;
	case DEV_USB :
		// translate the arguments here
		result = USB_disk_read(buff, sector, count);
		// translate the reslut code here
		return res;
	#else
	case SD_CARD:
		break;
	case SPI_FLASH:
		SPI_Flash_WaitForBUSY();
		flag = SPI_Flash_ReadStatusReg();
		if(flag & 0x02 == 0x00)  //SPI_FLASH Write disable
			res =  RES_WRPRT;
		else if(flag & 0x01 == 0x01)
			res = RES_NOTRDY;
		else
		{
			sector += 512;  //起始扇区向后偏移2M
			SPI_Flash_SequenceReadData(sector<<12, buff, count<<12);  //<<12表示*4096
			res = RES_OK;
		}
		return res;
	#endif
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
	DSTATUS flag;
	uint32_t eraseAddr = 0;
	#if DEV_RAM_MMC_USB
	int result;
	#endif

	switch (pdrv) {
	#if DEV_RAM_MMC_USB
	case DEV_RAM :
		// translate the arguments here
		result = RAM_disk_write(buff, sector, count);
		// translate the reslut code here
		return res;
	case DEV_MMC :
		// translate the arguments here
		result = MMC_disk_write(buff, sector, count);
		// translate the reslut code here
		return res;
	case DEV_USB :
		// translate the arguments here
		result = USB_disk_write(buff, sector, count);
		// translate the reslut code here
		return res;
	#else
	case SD_CARD:
		break;
	case SPI_FLASH:
		SPI_Flash_WaitForBUSY();
		flag = SPI_Flash_ReadStatusReg();
		if(flag & 0x02 == 0x00)  //SPI_FLASH Write disable
			res =  RES_WRPRT;
		else if(flag & 0x01 == 0x01)
			res = RES_NOTRDY;
		else
		{
			sector += 512;  //起始扇区向后偏移2M
			eraseAddr = sector << 12;
			SPI_Flash_SectorErase(eraseAddr);
			SPI_Flash_SequenceWriteData(sector<<12, (uint8_t*)buff, count<<12);  //<<12表示*4096
			res = RES_OK;
		}
		return res;
	
	#endif
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
	DSTATUS flag = 0;
	#if DEV_RAM_MMC_USB
	int result;
	#endif

	switch (pdrv) {
	#if DEV_RAM_MMC_USB
	case DEV_RAM :
		// Process of the command for the RAM drive
		return res;
	case DEV_MMC :
		// Process of the command for the MMC/SD card
		return res;
	case DEV_USB :
		// Process of the command the USB drive
		return res;
	#else 
	case SD_CARD:
		break;
	case SPI_FLASH:
		SPI_Flash_WaitForBUSY();
		flag = SPI_Flash_ReadStatusReg();
		if(flag & 0x02 == 0x00)  //SPI_FLASH Write disable
			res =  RES_WRPRT;
		else if(flag & 0x01 == 0x01)
			res = RES_NOTRDY;
		else
		{
			res = RES_OK;
		}
		//2048-512 = 1536，因为Flash前2M用于存放其他文件，所以后面6M用于文件系统
		if(cmd == GET_SECTOR_COUNT)
			*(DWORD*)buff = 1536;
		else if(cmd == GET_SECTOR_SIZE)
			*(WORD*)buff = 4096;
		else if(cmd == GET_BLOCK_SIZE)
			*(DWORD*)buff = 1;
		
		return res;
	
	#endif
	}

	return RES_PARERR;
}

