#ifndef GRAPHICS_GRAPH2D_MESSAGE_H
#define GRAPHICS_GRAPH2D_MESSAGE_H

#include "typedefs.h"

#include "graphics/graph2d/str_types.h"
#include "outgame/mode_slct.h"

typedef struct {
	int pri;
	int bx;
	int by;
	u_char r;
	u_char g;
	u_char b;
	u_char alp;
	u_char *str;
	u_char *stp;
	int sta;
	int flg;
	int pass;
	int csr;
	int selnum;
	int seltype;
	int decide;
	int mes_is_end;
	int cnt;
	int retst;
	int disptype;
	int fntmcnt;
	int fntcnt;
	int fntwait;
	u_char usrgb[4];
	u_char vib;
#if defined(BUILD_EU_VERSION)
	u_char bx_pass;
	u_char bx_pass_old;
	u_char bx_pass_st;
#endif
} MES_DAT;

typedef struct {
	u_short dummy;
	u_char pass;
	u_char pass_flg;
	u_char *old_adr;
	int cnt;
	u_char mes_alps[16][48];
} MESV22;

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
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
void InitMessageEF();
#endif
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
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
int SetMessageV2_2(DISP_STR *s);
#endif
int SetMessageV2(DISP_STR *s);
int SubMessageV3(u_char *s, int pri, int delflg);
int SetMessageV3(u_char *s, int pri);
int SetMessageV3_2(u_char *s, int pri);
void MesPassCheck();
int MesStatusCheck();
void DrawButtonTex(u_int pri, int type, float x, float y, u_char alp);
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
void DrawButtonTexNZ(u_int pri, int type, float x, float y, u_char alp);
#endif
void DrawMessageBox(u_int pri, float x, float y, float sizew, float sizeh, u_char alp);
int GetStrLength(u_char *str);
#if defined(BUILD_EU_VERSION)
int GetStrWidthMain(u_char *str, int type);
#endif
#if defined(BUILD_US_VERSION) || defined(BUILD_EU_VERSION)
int GetStrWidth(u_char *str);
#endif
#if defined(BUILD_EU_VERSION)
int GetStrWidthST(u_char *str);
#endif
void FontDispSample();
#if defined(BUILD_EU_VERSION)
int SetMessageMov(int path, int type, DISP_STR *s);
#endif

#endif // GRAPHICS_GRAPH2D_MESSAGE_H
