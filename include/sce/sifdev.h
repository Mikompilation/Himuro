#ifndef SCE_SIFDEV_H
#define SCE_SIFDEV_H

#define SCE_RDONLY      0x0001
#define SCE_WRONLY      0x0002
#define SCE_RDWR        0x0003
#define SCE_NBLOCK      0x0010
#define SCE_APPEND      0x0100
#define SCE_CREAT       0x0200
#define SCE_TRUNC       0x0400
#define SCE_EXCL        0x0800
#define SCE_NOBUF       0x4000
#define SCE_NOWAIT      0x8000

#define SCE_SEEK_SET    0
#define SCE_SEEK_CUR    1
#define SCE_SEEK_END    2

int sceOpen(const char *filename, int flag, ...);
int sceClose(int fd);
int sceRead(int fd, void *buf, int nbyte);
int sceWrite(int fd, const void *buf, int nbyte);
int sceLseek(int fd, int offset, int where);

int sceFsReset();

int sceSifInitIopHeap();
int sceSifFreeIopHeap(void *);
void *sceSifAllocIopHeap(unsigned int);
int sceSifRebootIop(const char *img);
int sceSifSyncIop();
int sceSifLoadModule(const char *filename, int args, const char *argp);

#endif // SCE_SIFDEV_H