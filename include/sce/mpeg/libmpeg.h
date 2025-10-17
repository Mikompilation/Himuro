#ifndef SCE_MPEG_LIBMPEG_H
#define SCE_MPEG_LIBMPEG_H

#include "typedefs.h"

#include "sce/mpeg/defs.h"

#define SCE_MPEG_BUFFER_SIZE(w, h)  ((w)*(h)*9/2 + 512 + 24*64 + 8192)

typedef enum {
    sceMpegCbError = 0,
    sceMpegCbNodata = 1,
    sceMpegCbStopDMA = 2,
    sceMpegCbRestartDMA = 3,
    sceMpegCbBackground = 4,
    sceMpegCbTimeStamp = 5,
    sceMpegCbStr = 6,
} sceMpegCbType;

typedef enum {
	sceMpegStrM2V = 0,
	sceMpegStrIPU = 1,
	sceMpegStrPCM = 2,
	sceMpegStrADPCM = 3,
	sceMpegStrDATA = 4,
} sceMpegStrType;

typedef struct {
    int width;
    int height;
    int frameCount;
    long int pts;
    long int dts;
    u_long flags;
    long int pts2nd;
    long int dts2nd;
    u_long flags2nd;
    void *sys;
} sceMpeg;

typedef struct {
    sceMpegCbType type;
    char *errMessage;
} sceMpegCbDataError;

typedef struct {
    sceMpegCbType type;
    long int pts;
    long int dts;
} sceMpegCbDataTimeStamp;

typedef struct {
    sceMpegCbType type;
    u_char *header;
    u_char *data;
    u_int len;
    long int pts;
    long int dts;
} sceMpegCbDataStr;

typedef union {
    sceMpegCbType type;
    sceMpegCbDataError error;
    sceMpegCbDataTimeStamp ts;
    sceMpegCbDataStr str;
} sceMpegCbData;

typedef struct {
	u_int pix[256];
} sceIpuRGB32;

typedef int (*sceMpegCallback)(sceMpeg *mp, sceMpegCbData *cbData, void *anyData);

int sceMpegReset(sceMpeg *mp);
int sceMpegCreate(sceMpeg *mp, u_char *work_area, int work_area_size);
int sceMpegDelete(sceMpeg *mp);
int sceMpegGetPicture(sceMpeg *mp, sceIpuRGB32 *rgb32, int mbcount);
int sceMpegIsEnd(sceMpeg *mp);
int sceMpegIsRefBuffEmpty(sceMpeg *mp);
void sceMpegSetDecodeMode(sceMpeg *mp, int ni, int np, int nb);
sceMpegCallback sceMpegAddCallback(sceMpeg *mp, sceMpegCbType type, sceMpegCallback callback, void *anyData);
sceMpegCallback sceMpegAddStrCallback(sceMpeg *mp, sceMpegStrType strType, int ch, sceMpegCallback callback, void *anyData);

#endif // SCE_MPEG_LIBMPEG_H
