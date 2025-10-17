#ifndef GRAPHICS_MOV_MOVIE_H
#define GRAPHICS_MOV_MOVIE_H

#include "typedefs.h"

#include "sce/mpeg/libmpeg.h"
#include "sce/mpeg/vibuf.h"
#include "sce/mpeg/vobuf.h"
#include "sce/mpeg/readbuf.h"
#include "sce/mpeg/videodec.h"
#include "sce/mpeg/audiodec.h"
#include "sce/libcdvd.h"

typedef struct {
	int play_event_no;
	int play_event_sta;
} MOVIE_WRK;

typedef union {
    u_long128 q;
    u_long l[2];
    u_int i[4];
    u_short s[8];
    u_char c[16];
} QWORD;

typedef struct {
    int isOnCD;
    int size;
    sceCdlFILE fp;
    u_char *iopBuf;
    int fd;
} StrFile;

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
extern MOVIE_WRK movie_wrk;
extern int thread_id;
extern u_int controller_val;
extern int videoDecTh;
// extern int defaultTh;
// extern StrFile infile;
// extern VideoDec videoDec;
// extern AudioDec audioDec;
extern int frd;
// extern sceGsDBuff db;
extern int play_mov_no;

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
int audioDecDelete(AudioDec *ad);
void audioDecPause(AudioDec *ad);
void audioDecResume(AudioDec *ad);
void audioDecStart(AudioDec *ad);
void audioDecReset(AudioDec *ad);
void audioDecBeginPut(AudioDec *ad, u_char **ptr0, int *len0, u_char **ptr1, int *len1);
void audioDecEndPut(AudioDec *ad, int size);
int audioDecIsPreset(AudioDec *ad);
int audioDecSendToIOP(AudioDec *ad);
void clearGsMem(int r, int g, int b, int disp_width, int disp_height);
void setImageTag(u_int *tags, void *image, int index, int image_w, int image_h);
int vblankHandlerM(int val);
int handler_endimage(int val);
void startDisplay(int waitEven);
void endDisplay();
int videoCallback(sceMpeg *mp, sceMpegCbDataStr *str, void *data);
int pcmCallback(sceMpeg *mp, sceMpegCbDataStr *str, void *data);
void readBufCreate(ReadBuf *b);
void readBufDelete(ReadBuf *b);
int readBufBeginPut(ReadBuf *b, u_char **ptr);
int readBufEndPut(ReadBuf *b, int size);
int readBufBeginGet(ReadBuf *b, u_char **ptr);
int readBufEndGet(ReadBuf *b, int size);
int strFileOpen(StrFile *file, char *filename);
int strFileClose(StrFile *file);
int strFileRead(StrFile *file, void *buff, int size);
int getFIFOindex(ViBuf *f, void *addr);
void setD3_CHCR(u_int val);
void setD4_CHCR(u_int val);
void scTag2(QWORD *q, void *addr, u_int id, u_int qwc);
int viBufCreate(ViBuf *f, u_long128 *data, u_long128 *tag, int size, TimeStamp *ts, int n_ts);
int viBufReset(ViBuf *f);
void viBufBeginPut(ViBuf *f, u_char **ptr0, int *len0, u_char **ptr1, int *len1);
void viBufEndPut(ViBuf *f, int size);
int viBufAddDMA(ViBuf *f);
int viBufStopDMA(ViBuf *f);
int viBufRestartDMA(ViBuf *f);
int viBufDelete(ViBuf *f);
int viBufIsActive(ViBuf *f);
int viBufCount(ViBuf *f);
void viBufFlush(ViBuf *f);
int viBufModifyPts(ViBuf *f, TimeStamp *new_ts);
int viBufPutTs(ViBuf *f, TimeStamp *ts);
int viBufGetTs(ViBuf *f, TimeStamp *ts);
int videoDecCreate(VideoDec *vd, u_char *mpegWork, int mpegWorkSize, u_long128 *data, u_long128 *tag, int tagSize, TimeStamp *pts, int n_pts);
void videoDecSetDecodeMode(VideoDec *vd, int ni, int np, int nb);
int videoDecSetStream(VideoDec *vd, int strType, int ch, sceMpegCallback cb, void *data);
void videoDecBeginPut(VideoDec *vd, u_char **ptr0, int *len0, u_char **ptr1, int *len1);
void videoDecEndPut(VideoDec *vd, int size);
void videoDecReset(VideoDec *vd);
int videoDecDelete(VideoDec *vd);
void videoDecAbort(VideoDec *vd);
u_int videoDecGetState(VideoDec *vd);
u_int videoDecSetState(VideoDec *vd, u_int state);
int videoDecPutTs(VideoDec *vd, long int pts_val, long int dts_val, u_char *start, int len);
int videoDecInputCount(VideoDec *vd);
int videoDecInputSpaceCount(VideoDec *vd);
int videoDecFlush(VideoDec *vd);
int videoDecIsFlushed(VideoDec *vd);
void videoDecMain(VideoDec *vd);
int decBs0(VideoDec *vd);
int mpegError(sceMpeg *mp, sceMpegCbDataError *cberror, void *anyData);
int mpegNodata(sceMpeg *mp, sceMpegCbData *cbdata, void *anyData);
int mpegStopDMA(sceMpeg *mp, sceMpegCbData *cbdata, void *anyData);
int mpegRestartDMA(sceMpeg *mp, sceMpegCbData *cbdata, void *anyData);
int mpegTS(sceMpeg *mp, sceMpegCbDataTimeStamp *cbts, void *anyData);
void voBufCreate(VoBuf *f, VoData *data, VoTag *tag, int size);
void voBufDelete(VoBuf *f);
void voBufReset(VoBuf *f);
int voBufIsFull(VoBuf *f);
void voBufIncCount(VoBuf *f);
VoData* voBufGetData(VoBuf *f);
int voBufIsEmpty(VoBuf *f);
VoTag* voBufGetTag(VoBuf *f);
void voBufDecCount(VoBuf *f);

#endif // GRAPHICS_MOV_MOVIE_H
