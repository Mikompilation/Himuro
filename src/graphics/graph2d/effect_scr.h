#ifndef GRAPHICS_GRAPH2D_EFFECT_SCR_H
#define GRAPHICS_GRAPH2D_EFFECT_SCR_H

#include "typedefs.h"
#include "structs.h"

// extern EFF_BLUR eff_blur;
// extern EFF_FOCUS eff_focus;
// extern EFF_DEFORM eff_deform;
// extern short int overlap_passflg[2];
// extern float blur_x_offset;
// extern float blur_y_offset;
// extern int gameover_flow;
// extern int ef_exfade1_flow1;
// extern int ef_exfade1_flow2;
// extern int ef_exfade1_time1;
// extern int ef_exfade2_flow1;
// extern int ef_exfade2_time1;
// extern int ef_exfade2_time2;
// extern int ef_exfade2_time3;

#include "graphics/graph2d/effect.h"

void InitEffectScr();
void InitEffectScrEF();
void SetWhiteOut();
void SetWhiteIn();
void SetBlackOut();
void SetBlackIn();
void SetWhiteOut2(int time);
void SetWhiteIn2(int time);
void SetBlackOut2(int time);
void SetBlackIn2(int time);
void SetFlash();
void SetBlackFilter(EFFECT_CONT *ec);
void SubBlur(int type, u_char alpha, float scale, float rot, float cx, float cy);
void SetBlur();
void RunBlur();
void CallBlur(int type, int wait, u_char alpha, float scale, float rot);
void CallBlur2(int in, int keep, int out, u_char alpha, float scale, float rot);
void CallBlur3(int in, int keep, int out, u_char alpha, float scale, float rot, float cx, float cy);
void SubFocus(int ef);
void SetFocus();
void RunFocus();
void CallFocus(int type, int wait, int gap);
void CallFocus2(int in, int keep, int out, int max);
void SubDeform(int type, float rate, u_char alp);
void SetDeform();
void RunDeform();
void CallDeform2(int in, int keep, int out, int type, int max);
void MakeScrDeformPacket(int pnumw, int pnumh, u_long tex0, SCRDEF *scrdef[33], int alp);
void SetDeform0(int type, float rate, u_char alp);
void SetDeform2(int type, float rate, u_char alp);
void SetDeform3(int type, float rate, u_char alp);
void SetDeform4(int type, float rate, u_char alp);
void SetDeform5(int type, float rate, u_char alp);
void SetDeform6(int type, float rate, u_char alp);
void SubContrast2(u_char col, u_char alp);
void SetContrast2();
void SubContrast3(u_char col, u_char alp);
void SetContrast3();
void SubNega(u_char r, u_char g, u_char b, u_char alp, u_char alp2);
void SetNega();
void* CallNega2(int in, int keep, int out);
void* CallNega(int time);
void SetOverRap();
void SetForcusDepth();
void SetForcusDepth2();
void MakeRDither3();
void SubDither3(int type, float alp, float spd, u_char alpmx, u_char colmx);
void SetDither3();
void SubDither4(float alp, float spd);
void SetDither4();
void SubFadeFrame(u_char alp, u_int pri);
void SetFadeFrame();
void ChangeMAGATOKI2(int sw);
void SetMAGATOKI2();
void SetMAGATOKI();
u_char SubNowLoading(int fl, int num, float sclx, float scly, float bx, float by, float spd, float rate, float trate);
void InitNowLoading();
void SetNowLoadingON();
void SetNowLoadingOFF();
int SetNowLoading();
void SubGameOver(u_char alp, float rate);
void InitGameOver();
void SetGameOverON();
int SetGameOver();
void ChangeMonochrome(int sw);
void InitExFade1(int fl, int time1);
int SetExFade1();
void InitExFade2(int fl, int time1, int time2, int time3);
int SetExFade2();
void InitScreenSaver();
void SetScreenSaver();
void SetScreenEffect();

#endif // GRAPHICS_GRAPH2D_EFFECT_SCR_H
