#ifndef SCE_SDMACRO_H
#define SCE_SDMACRO_H

#define SD_INIT_COLD 0

#define SD_TRANS_MODE_WRITE 0
#define SD_TRANS_MODE_READ  1
#define SD_TRANS_MODE_STOP  2
#define SD_TRANS_MODE_WRITE_FROM 3

#define SD_TRANS_BY_DMA     (0x0<<3)
#define SD_TRANS_BY_IO      (0x1<<3)

#define SD_BLOCK_ONESHOT (0<<4)
#define SD_BLOCK_LOOP (1<<4)
#define SD_BLOCK_HANDLER (1<<7)

#define SD_P_MVOLL ((0x09<<8)+(0x01<<7))
#define SD_P_MVOLR ((0x0a<<8)+(0x01<<7))
#define SD_P_BVOLL ((0x0f<<8)+(0x01<<7))
#define SD_P_BVOLR ((0x10<<8)+(0x01<<7))

#endif // SCE_SDMACRO_H
