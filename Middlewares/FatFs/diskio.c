/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"     /* Obtains integer types */
#include "diskio.h" /* Declarations of disk functions */
#include "./SYSTEM/usart/usart.h"
#include "diskio.h"
#include "./BSP/SDMMC/sdmmc_sdcard.h"

/* Definitions of physical drive number for each drive */
#define SD_CARD  0 /* SD卡,卷标为0 */
#define EX_FLASH 1 /* 外部spi flash,卷标为1 */
#define EX_NAND  2 /* 外部NAND FLASH卷标为2 */

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(
    BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
    return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(
    BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
    uint8_t res = 0;

    switch (pdrv) {
        case SD_CARD:        /* SD卡 */
            res = sd_init(); /* SD卡初始化 */
            break;
        default:
            res = 1;
            break;
    }

    if (res) {
        return STA_NOINIT;
    } else {
        return 0; /* 初始化成功*/
    }
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(
    BYTE pdrv,    /* Physical drive nmuber to identify the drive */
    BYTE *buff,   /* Data buffer to store read data */
    LBA_t sector, /* Start sector in LBA */
    UINT count    /* Number of sectors to read */
)
{
    uint8_t res = 0;

    if (!count) return RES_PARERR; /* count不能等于0，否则返回参数错误 */

    switch (pdrv) {
        case SD_CARD: /* SD卡 */
            res = sd_read_disk(buff, sector, count);

            while (res) /* 读出错 */
            {
                // printf("sd rd error:%d\r\n", res);
                sd_init(); /* 重新初始化SD卡 */
                res = sd_read_disk(buff, sector, count);
            }
            break;
        default:
            res = 1;
    }
    /* 处理返回值，将返回值转成ff.c的返回值 */
    if (res == 0x00) {
        return RES_OK;
    } else {
        return RES_ERROR;
    }
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(
    BYTE pdrv,        /* Physical drive nmuber to identify the drive */
    const BYTE *buff, /* Data to be written */
    LBA_t sector,     /* Start sector in LBA */
    UINT count        /* Number of sectors to write */
)
{
    uint8_t res = 0;

    if (!count) return RES_PARERR; /* count不能等于0，否则返回参数错误 */

    switch (pdrv) {
        case SD_CARD: /* SD卡 */
            res = sd_write_disk((uint8_t *)buff, sector, count);

            while (res) /* 写出错 */
            {
                // printf("sd wr error:%d\r\n", res);
                sd_init(); /* 重新初始化SD卡 */
                res = sd_write_disk((uint8_t *)buff, sector, count);
            }
            break;
        default:
            res = 1;
    }

    /* 处理返回值，将返回值转成ff.c的返回值 */
    if (res == 0x00) {
        return RES_OK;
    } else {
        return RES_ERROR;
    }
}

#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(
    BYTE pdrv, /* Physical drive nmuber (0..) */
    BYTE cmd,  /* Control code */
    void *buff /* Buffer to send/receive control data */
)
{
    DRESULT res;

    if (pdrv == SD_CARD) /* SD卡 */
    {
        switch (cmd) {
            case CTRL_SYNC:
                res = RES_OK;
                break;

            case GET_SECTOR_SIZE:
                *(DWORD *)buff = 512;
                res            = RES_OK;
                break;

            case GET_BLOCK_SIZE:
                *(WORD *)buff = g_sd_card_info_handle.LogBlockSize;
                res           = RES_OK;
                break;

            case GET_SECTOR_COUNT:
                *(DWORD *)buff = g_sd_card_info_handle.LogBlockNbr;
                res            = RES_OK;
                break;

            default:
                res = RES_PARERR;
                break;
        }
    } else {
        res = RES_ERROR; /* 其他的不支持 */
    }

    return res;
}
