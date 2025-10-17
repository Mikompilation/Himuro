#ifndef SCE_LIBGIFPK_H
#define SCE_LIBGIFPK_H

#include "typedefs.h"

typedef struct {
	u_int *pCurrent;
	u_long128 *pBase;
	u_long128 *pDmaTag;
	u_long *pGifTag;
} sceGifPacket;

u_long128 *sceGifPkTerminate(sceGifPacket *pPacket);
void sceGifPkCloseGifTag(sceGifPacket *pPacket);
void sceGifPkOpenGifTag(sceGifPacket *pPacket, u_long128 gifTag);
void sceGifPkAddGsAD(sceGifPacket *pPacket, u_int addr, u_long data);
void sceGifPkEnd(sceGifPacket *pPacket, u_int opt1, u_int opt2, u_int flag);
void sceGifPkReset(sceGifPacket *pPacket);
void sceGifPkInit(sceGifPacket *pPacket, u_long128 *pBase);
void sceGifPkReset(sceGifPacket *pPacket);
void sceGifPkCnt(sceGifPacket *pPacket, u_int opt1, u_int opt2, u_int flag);
u_int *sceGifPkReserve(sceGifPacket *pPacket, u_int count);
void sceGifPkRef(sceGifPacket *pPacket, u_long128 *pRef, u_int size, u_int opt1, u_int opt2, u_int flag);


#endif // SCE_LIBGIFPK_H
