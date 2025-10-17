#ifndef SCE_MPEG_VIDEODEC_H
#define SCE_MPEG_VIDEODEC_H

#include "sce/mpeg/libmpeg.h"
#include "sce/mpeg/vibuf.h"

#define VD_STATE_NORMAL 0
#define VD_STATE_ABORT  1
#define VD_STATE_FLUSH  2
#define VD_STATE_END    3

typedef struct { // 0xb8
	/* 0x00 */ sceMpeg mpeg;
	/* 0x48 */ ViBuf vibuf;
	/* 0xa8 */ u_int state;
	/* 0xac */ int sema;
	/* 0xb0 */ int hid_endimage;
	/* 0xb4 */ int hid_vblank;
} VideoDec;

#endif // SCE_MPEG_VIDEODEC_H