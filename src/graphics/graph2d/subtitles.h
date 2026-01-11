#ifdef BUILD_EU_VERSION

#ifndef GRAPHICS_GRAPH2D_SUBTITLES_H
#define GRAPHICS_GRAPH2D_SUBTITLES_H

#include "typedefs.h"

typedef struct {
	u_int type;
	u_int no;
	u_int cnt;
	u_char alp;
	u_char flg;
	u_char run;
	u_char dummy;
} SUBTITLES_SYS;

extern SUBTITLES_SYS subtitles_sys;

void ContSubtitlesTime(int path, u_int frame);
void DispNowFrame(int path, u_int frame);
int GetIndexNums(int *tbl, int no);
void MakeMovMes();
void DrawMovMes(Q_WORDDATA *packet_buf);
void InitSubtitlesSys();
void FinishSubtitlesSys();
void SetSubtitlesNCnt(int type, int no);
void SetSubtitlesNCntOne(int type, int no);
void CallSubtitles();
void SetSubtitles(int type, int no, u_int mframe);
void CheckSubtitlesFlag();

#endif // GRAPHICS_GRAPH2D_SUBTITLES_H

#endif // BUILD_EU_VERSION