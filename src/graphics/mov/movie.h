#ifndef GRAPHICS_MOV_MOVIE_H
#define GRAPHICS_MOV_MOVIE_H

#include "typedefs.h"

typedef struct {
	u_int lsn;
	u_int size;
	char name[16];
	u_char date[8];
	u_int flag;
} sceCdlFILE;

typedef struct {
	int play_event_no;
	int play_event_sta;
} MOVIE_WRK;

typedef union
{ // 0x10
    /* 0x0 */ u_long128 q;
    /* 0x0 */ u_long l[2];
    /* 0x0 */ u_int i[4];
    /* 0x0 */ u_short s[8];
    /* 0x0 */ u_char c[16];
} QWORD;

typedef struct
{ // 0x20
    /* 0x00 */ char id[4];
    /* 0x04 */ int size;
    /* 0x08 */ int type;
    /* 0x0c */ int rate;
    /* 0x10 */ int ch;
    /* 0x14 */ int interSize;
    /* 0x18 */ int loopStart;
    /* 0x1c */ int loopEnd;
} SpuStreamHeader;

typedef struct
{ // 0x8
    /* 0x0 */ char id[4];
    /* 0x4 */ int size;
} SpuStreamBody;

typedef struct
{ // 0x5c
    /* 0x00 */ int state;
    /* 0x04 */ SpuStreamHeader sshd;
    /* 0x24 */ SpuStreamBody ssbd;
    /* 0x2c */ int hdrCount;
    /* 0x30 */ u_char *data;
    /* 0x34 */ int put;
    /* 0x38 */ int count;
    /* 0x3c */ int size;
    /* 0x40 */ int totalBytes;
    /* 0x44 */ int iopBuff;
    /* 0x48 */ int iopBuffSize;
    /* 0x4c */ int iopLastPos;
    /* 0x50 */ int iopPausePos;
    /* 0x54 */ int totalBytesSent;
    /* 0x58 */ int iopZero;
} AudioDec;

typedef struct
{ // 0x34
    /* 0x00 */ int isOnCD;
    /* 0x04 */ int size;
    /* 0x08 */ sceCdlFILE fp;
    /* 0x2c */ u_char *iopBuf;
    /* 0x30 */ int fd;
} StrFile;

typedef struct
{ // 0x48
    /* 0x00 */ int width;
    /* 0x04 */ int height;
    /* 0x08 */ int frameCount;
    /* 0x10 */ long int pts;
    /* 0x18 */ long int dts;
    /* 0x20 */ u_long flags;
    /* 0x28 */ long int pts2nd;
    /* 0x30 */ long int dts2nd;
    /* 0x38 */ u_long flags2nd;
    /* 0x40 */ void *sys;
} sceMpeg;

typedef enum {
    sceMpegCbError      = 0,
    sceMpegCbNodata     = 1,
    sceMpegCbStopDMA    = 2,
    sceMpegCbRestartDMA = 3,
    sceMpegCbBackground = 4,
    sceMpegCbTimeStamp  = 5,
    sceMpegCbStr        = 6
} sceMpegCbType;

typedef struct
{ // 0x20
    /* 0x00 */ sceMpegCbType type;
    /* 0x04 */ u_char *header;
    /* 0x08 */ u_char *data;
    /* 0x0c */ u_int len;
    /* 0x10 */ long int pts;
    /* 0x18 */ long int dts;
} sceMpegCbDataStr;

typedef struct
{ // 0x5000c
    /* 0x00000 */ u_char data[327680];
    /* 0x50000 */ int put;
    /* 0x50004 */ int count;
    /* 0x50008 */ int size;
} ReadBuf;

typedef struct
{ // 0x18
    /* 0x00 */ long int pts;
    /* 0x08 */ long int dts;
    /* 0x10 */ int pos;
    /* 0x14 */ int len;
} TimeStamp;

typedef struct
{ // 0x24
    /* 0x00 */ u_int d4madr;
    /* 0x04 */ u_int d4tadr;
    /* 0x08 */ u_int d4qwc;
    /* 0x0c */ u_int d4chcr;
    /* 0x10 */ u_int d3madr;
    /* 0x14 */ u_int d3qwc;
    /* 0x18 */ u_int d3chcr;
    /* 0x1c */ u_int ipubp;
    /* 0x20 */ u_int ipuctrl;
} sceIpuDmaEnv;

typedef struct
{ // 0x60
    /* 0x00 */ u_long128 *data;
    /* 0x04 */ u_long128 *tag;
    /* 0x08 */ int n;
    /* 0x0c */ int dmaStart;
    /* 0x10 */ int dmaN;
    /* 0x14 */ int readBytes;
    /* 0x18 */ int buffSize;
    /* 0x1c */ sceIpuDmaEnv env;
    /* 0x40 */ int sema;
    /* 0x44 */ int isActive;
    /* 0x48 */ long int totalBytes;
    /* 0x50 */ TimeStamp *ts;
    /* 0x54 */ int n_ts;
    /* 0x58 */ int count_ts;
    /* 0x5c */ int wt_ts;
} ViBuf;

typedef struct
{ // 0x8
    /* 0x0 */ sceMpegCbType type;
    /* 0x4 */ char *errMessage;
} sceMpegCbDataError;

typedef struct
{ // 0x18
    /* 0x00 */ sceMpegCbType type;
    /* 0x08 */ long int pts;
    /* 0x10 */ long int dts;
} sceMpegCbDataTimeStamp;

typedef union
{ // 0x20
    /* 0x00 */ sceMpegCbType type;
    /* 0x00 */ sceMpegCbDataError error;
    /* 0x00 */ sceMpegCbDataTimeStamp ts;
    /* 0x00 */ sceMpegCbDataStr str;
} sceMpegCbData;

typedef int (*sceMpegCallback)(sceMpeg *mp, sceMpegCbData *cbData, void *anyData);

typedef struct
{ // 0x12c000
    /* 0x000000 */ u_char v[1228800];
} VoData;

typedef struct
{ // 0x39240
    /* 0x00000 */ int status;
    /* 0x00004 */ int dummy[15];
    /* 0x00040 */ u_int v[2][29248];
} VoTag;

typedef struct
{ // 0x14
    /* 0x00 */ VoData *data;
    /* 0x04 */ VoTag *tag;
    /* 0x08 */ int write;
    /* 0x0c */ int count;
    /* 0x10 */ int size;
} VoBuf;

// extern ReadBuf *readBuf;
extern u_int scene_bg_color;
// extern int isWithAudio;
// extern char *commandname;
// extern char *VERSION;
// extern char *mpegName[0];
// extern u_char mpeg_vol_rate[0];
// extern int isCountVblank;
// extern int vblankCount;
// extern int isFrameEnd;
// extern int oddeven;
// extern int handler_error;
// extern int isStrFileInit;
// extern VoBuf voBuf;
// extern MOVIE_WRK movie_wrk;
// extern int thread_id;
// extern u_int controller_val;
// extern int videoDecTh;
// extern int defaultTh;
// extern StrFile infile;
// extern VideoDec videoDec;
// extern AudioDec audioDec;
// extern int frd;
// extern sceGsDBuff db;
// extern int play_mov_no;

void MovieInitWrk();
void ReqMpegEvent(int no);
int PlayMpegEvent();
u_int movie(char *name);
void switchThread();
void initMov(char *bsfilename);
void ErrMessage(char *message);
void proceedAudio();
int MoviePlay(int scene_no);
void MovieTest(int scene_no);
int PadSyncCallback2();
void movVblankPad();
void ReqLogoMovie();
int audioDecCreate(AudioDec *ad, u_char *buff, int buffSize, int iopBuffSize);
int audioDecDelete();
void audioDecPause();
void audioDecResume();
void audioDecStart();
void audioDecReset();
void audioDecBeginPut(u_char **ptr0, int *len0, u_char **ptr1, int *len1);
void audioDecEndPut(int size);
int audioDecIsPreset();
int audioDecSendToIOP();
void clearGsMem(int r, int g, int b, int disp_width, int disp_height);
void setImageTag(u_int *tags, void *image, int index, int image_w, int image_h);
int vblankHandlerM(int val);
int handler_endimage(int val);
void startDisplay(int waitEven);
void endDisplay();
int videoCallback(sceMpeg *mp, sceMpegCbDataStr *str, void *data);
int pcmCallback(void *data);
void readBufCreate(ReadBuf *b);
void readBufDelete(ReadBuf *b);
int readBufBeginPut(ReadBuf *b, u_char **ptr);
int readBufEndPut(ReadBuf *b, int size);
int readBufBeginGet(ReadBuf *b, u_char **ptr);
int readBufEndGet(ReadBuf *b, int size);
int strFileOpen(char *filename);
int strFileClose();
int strFileRead(void *buff, int size);
int getFIFOindex(ViBuf *f, void *addr);
void setD3_CHCR(u_int val);
void setD4_CHCR(u_int val);
void scTag2(QWORD *q, void *addr, u_int id, u_int qwc);
int viBufCreate(u_long128 *data, u_long128 *tag, int size, TimeStamp *ts, int n_ts);
int viBufReset();
void viBufBeginPut(int *len0, u_char **ptr1, int *len1);
void viBufEndPut(int size);
int viBufAddDMA();
int viBufStopDMA();
int viBufRestartDMA();
int viBufDelete();
int viBufIsActive();
int viBufCount();
void viBufFlush();
int viBufModifyPts(TimeStamp *new_ts);
int viBufPutTs(TimeStamp *ts);
int viBufGetTs(TimeStamp *ts);
int videoDecCreate(u_char *mpegWork, int mpegWorkSize, u_long128 *data, u_long128 *tag, int tagSize, TimeStamp *pts, int n_pts);
void videoDecSetDecodeMode(int ni, int np, int nb);
int videoDecSetStream(int strType, int ch, sceMpegCallback cb, void *data);
void videoDecBeginPut(int *len0, u_char **ptr1, int *len1);
void videoDecEndPut(int size);
void videoDecReset();
int videoDecDelete();
void videoDecAbort();
u_int videoDecGetState();
u_int videoDecSetState(u_int state);
int videoDecPutTs(long int pts_val, long int dts_val, u_char *start, int len);
int videoDecInputCount();
int videoDecInputSpaceCount();
int videoDecFlush();
int videoDecIsFlushed();
void videoDecMain();
int decBs0();
int mpegError(sceMpegCbDataError *cberror, void *anyData);
int mpegNodata(sceMpegCbData *cbdata, void *anyData);
int mpegStopDMA(void *anyData);
int mpegRestartDMA(void *anyData);
int mpegTS(sceMpegCbDataTimeStamp *cbts, void *anyData);
void voBufCreate(VoBuf *f, VoData *data, VoTag *tag, int size);
void voBufDelete();
void voBufReset();
int voBufIsFull();
void voBufIncCount();
VoData* voBufGetData();
int voBufIsEmpty();
VoTag* voBufGetTag();
void voBufDecCount();

#endif // GRAPHICS_MOV_MOVIE_H
