#ifndef GRAPHICS_GRAPH2D_G2D_DEBUG_H
#define GRAPHICS_GRAPH2D_G2D_DEBUG_H

#include "typedefs.h"

typedef struct
{ // 0xc
    /* 0x0 */ char *name;
    /* 0x4 */ int subnum;
    /* 0x8 */ int nmax;
} DEBUG_SUB_MENU;

typedef struct {
	int parent;
	int off_num;
	int mnum;
	int kai;
	int max;
	int pos;
	char *title;
	DEBUG_SUB_MENU submenu[10];
} DEBUG_MENU;

// extern int dither_alp;
// extern int dither_col;
extern int hint_test_sw;
extern int hint_test_posx;

void gra2dInitDbgMenu();
void gra2dDrawDbgMenuSub(DEBUG_MENU *wlp);
void gra2dDrawDbgMenu();
void SetMaxNumber(int num1, int num2, int nmax);
void InitShibataSet();
void SetShibataSet();
void InitPerformanceCounter();
void StartPerformanceCounter();
void StopPerformanceCounter();
void DrawPerformanceCounter();
void WaitFrameTop(int type);
void CheckHintTex();

#endif // GRAPHICS_GRAPH2D_G2D_DEBUG_H
