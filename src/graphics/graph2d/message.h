#ifndef GRAPHICS_GRAPH2D_MESSAGE_H
#define GRAPHICS_GRAPH2D_MESSAGE_H

#include "typedefs.h"

typedef struct {
	u_char *str;
	int pos_x;
	int pos_y;
	int type;
	u_int r;
	u_int g;
	u_int b;
	int alpha;
	int pri;
	int x_wide;
	int y_wide;
	int brnch_num;
	int csr;
	int st;
} DISP_STR;

typedef struct {
	u_char *str;
	int pos_x;
	int pos_y;
	int type;
	u_int r;
	u_int g;
	u_int b;
	int alpha;
	int pri;
} STR_DAT;

#include "outgame/mode_slct.h"

// extern u_char ascii_font_tbl[0];
// extern u_char ascii_font_tbl2[0];
// extern int wbyte_font_tbl0[0];
// extern int wbyte_font_tbl1[0];
// extern int wbyte_font_tbl2[0];
// extern int wbyte_font_tbl3[0];
// extern int wbyte_font_tbl4[0];
// extern int wbyte_font_tbl5[0];
// extern SPRT_DAT fntdat[0];
// extern SPRT_DAT mesbox[0];
// extern SPRT_DAT btntex[0];
// extern u_char *save_mes_addr;

void InitMessage();
void InitMessageEF();
void SetMessageDirect(int addr, int pri, int fn, int x, int y, u_char r, u_char g, u_char b);
void SetMessageAddr(int addr);
int SetMessage();
void MessageWaitOff();
void SetMesFrame(int pri, int type, float bx, float by, float sw, float sh);
void SetInteger(float x, float y, int num);
void SetInteger2(int pri, float x, float y, int type, u_char r, u_char g, u_char b, int num);
void SetInteger3(int pri, float x, float y, int type, u_char r, u_char g, u_char b, u_char a, int num);
void SetASCIIString(float x, float y, char *str);
void SetASCIIString2(int pri, float x, float y, int type, u_char r, u_char g, u_char b, char *str);
void SetASCIIString3(int pri, float x, float y, int type, u_char r, u_char g, u_char b, u_char a, char *str);
void SetWString2(int pri, float x, float y, u_char r, u_char g, u_char b, char *str);
void SetString(float x, float y, char *fmt, ...);
void SetString2(int pri, float x, float y, int type, u_char r, u_char g, u_char b, char *fmt, ...);
void CopyStrDToStr(DISP_STR *s, STR_DAT *d);
int SetMessageV2_2(DISP_STR *s);
int SetMessageV2(DISP_STR *s);
int SubMessageV3(u_char *s, int pri, int delflg);
int SetMessageV3(u_char *s, int pri);
int SetMessageV3_2(u_char *s, int pri);
void MesPassCheck();
int MesStatusCheck();
void DrawButtonTex(u_int pri, int type, float x, float y, u_char alp);
void DrawButtonTexNZ(u_int pri, int type, float x, float y, u_char alp);
void DrawMessageBox(u_int pri, float x, float y, float sizew, float sizeh, u_char alp);
int GetStrLength(u_char *str);

#ifdef BUILD_EU_VERSION
int GetStrWidthMain(u_char *str, int type);
#endif

int GetStrWidth(u_char *str);

#ifdef BUILD_EU_VERSION
int GetStrWidthST(u_char *str);
#endif

void FontDispSample();

#ifdef BUILD_EU_VERSION
int SetMessageMov(int path, int type);
#endif

#endif // GRAPHICS_GRAPH2D_MESSAGE_H
