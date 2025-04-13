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

typedef struct { // 0x58
	/* 0x00 */ int pri;
	/* 0x04 */ int bx;
	/* 0x08 */ int by;
	/* 0x0c */ u_char r;
	/* 0x0d */ u_char g;
	/* 0x0e */ u_char b;
	/* 0x0f */ u_char alp;
	/* 0x10 */ u_char *str;
	/* 0x14 */ u_char *stp;
	/* 0x18 */ int sta;
	/* 0x1c */ int flg;
	/* 0x20 */ int pass;
	/* 0x24 */ int csr;
	/* 0x28 */ int selnum;
	/* 0x2c */ int seltype;
	/* 0x30 */ int decide;
	/* 0x34 */ int mes_is_end;
	/* 0x38 */ int cnt;
	/* 0x3c */ int retst;
	/* 0x40 */ int disptype;
	/* 0x44 */ int fntmcnt;
	/* 0x48 */ int fntcnt;
	/* 0x4c */ int fntwait;
	/* 0x50 */ u_char usrgb[4];
	/* 0x54 */ u_char vib;
#ifdef BUILD_EU_VERSION
	/* 0x55 */ u_char bx_pass;
	/* 0x56 */ u_char bx_pass_old;
	/* 0x57 */ u_char bx_pass_st;
#endif
} MES_DAT;

typedef struct { // 0x30c
	/* 0x000 */ u_short dummy;
	/* 0x002 */ u_char pass;
	/* 0x003 */ u_char pass_flg;
	/* 0x004 */ u_char *old_adr;
	/* 0x008 */ int cnt;
	/* 0x00c */ u_char mes_alps[16][48];
} MESV22;

#include "outgame/mode_slct.h"

extern u_char ascii_font_tbl[];
extern u_char ascii_font_tbl2[];
extern int wbyte_font_tbl0[];
extern int wbyte_font_tbl1[];
extern int wbyte_font_tbl2[];
extern int wbyte_font_tbl3[];
extern int wbyte_font_tbl4[];
extern int wbyte_font_tbl5[];
extern SPRT_DAT fntdat[];
extern SPRT_DAT mesbox[];
extern SPRT_DAT btntex[];
extern u_char *save_mes_addr;

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
int SetMessageMov(int path, int type, DISP_STR *s);
#endif

#endif // GRAPHICS_GRAPH2D_MESSAGE_H
