/*-----------------------------------------------------------------------/
/  Low level disk interface modlue include file   (C)ChaN, 2019          /
/-----------------------------------------------------------------------*/

#ifndef _DISKIO_DEFINED
#define _DISKIO_DEFINED

#include <ff.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /* Status of Disk Functions */
    typedef BYTE DSTATUS;

    /* Results of Disk Functions */
    typedef enum
    {
        RES_OK = 0, /* 0: Successful */
        RES_ERROR,  /* 1: R/W Error */
        RES_WRPRT,  /* 2: Write Protected */
        RES_NOTRDY, /* 3: Not Ready */
        RES_PARERR  /* 4: Invalid Parameter */
    } DRESULT;

    /*---------------------------------------*/
    /* Prototypes for disk control functions */

    /// @brief 初始化驱动器。
    /// @param pdrv 驱动器号
    /// @return 返回 0 表示成功。
    DSTATUS disk_initialize(BYTE pdrv);

    /// @brief 获取驱动器的状态，指示驱动器是否准备好。
    /// @param pdrv 驱动器号。
    /// @return 返回 0 表示驱动器准备好。
    DSTATUS disk_status(BYTE pdrv);

    /// @brief 读驱动器。
    /// @param pdrv 驱动器号。
    /// @param buff 读取的数据放到此缓冲区。
    /// @param sector 起始扇区号。
    /// @param count 要读取的扇区数量。
    /// @return
    DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count);

    /// @brief 写驱动器。
    /// @param pdrv 驱动器号。
    /// @param buff 要被写入到驱动器的数据缓冲区。
    /// @param sector 起始扇区号。
    /// @param count 要写入的扇区数量。
    /// @return
    DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count);

    /// @brief 驱动器控制。
    /// @param pdrv 驱动器号。
    /// @param cmd 命令。具体有哪些命令见下方的宏定义。
    /// @param buff 命令参数或者命令输出结果。
    /// @return
    DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff);

    /* Disk Status Bits (DSTATUS) */

#define STA_NOINIT 0x01  /* Drive not initialized */
#define STA_NODISK 0x02  /* No medium in the drive */
#define STA_PROTECT 0x04 /* Write protected */

/* Command code for disk_ioctrl fucntion */

/* Generic command (Used by FatFs) */
#define CTRL_SYNC 0        /* Complete pending write process (needed at FF_FS_READONLY == 0) */
#define GET_SECTOR_COUNT 1 /* Get media size (needed at FF_USE_MKFS == 1) */
#define GET_SECTOR_SIZE 2  /* Get sector size (needed at FF_MAX_SS != FF_MIN_SS) */
#define GET_BLOCK_SIZE 3   /* Get erase block size (needed at FF_USE_MKFS == 1) */
#define CTRL_TRIM 4        /* Inform device that the data on the block of sectors is no longer used (needed at FF_USE_TRIM == 1) */

/* Generic command (Not used by FatFs) */
#define CTRL_POWER 5  /* Get/Set power status */
#define CTRL_LOCK 6   /* Lock/Unlock media removal */
#define CTRL_EJECT 7  /* Eject media */
#define CTRL_FORMAT 8 /* Create physical format on the media */

/* MMC/SDC specific ioctl command */
#define MMC_GET_TYPE 10   /* Get card type */
#define MMC_GET_CSD 11    /* Get CSD */
#define MMC_GET_CID 12    /* Get CID */
#define MMC_GET_OCR 13    /* Get OCR */
#define MMC_GET_SDSTAT 14 /* Get SD status */
#define ISDIO_READ 55     /* Read data form SD iSDIO register */
#define ISDIO_WRITE 56    /* Write data to SD iSDIO register */
#define ISDIO_MRITE 57    /* Masked write data to SD iSDIO register */

/* ATA/CF specific ioctl command */
#define ATA_GET_REV 20   /* Get F/W revision */
#define ATA_GET_MODEL 21 /* Get model name */
#define ATA_GET_SN 22    /* Get serial number */

#ifdef __cplusplus
}
#endif

#endif
