#ifndef GRAPHICS_GRAPH2D_TIM2_NEW_H
#define GRAPHICS_GRAPH2D_TIM2_NEW_H

#include "typedefs.h"
#include "graphics/graph2d/tim2.h"

// extern u_int *tm2_end_pkt;

u_int* MakeTim2Direct2(u_int *pkt_addr, u_int *tim2_addr, int tbp);
u_int* MakeClutDirect2(u_int *pkt_addr, u_int *tim2_addr, int cbp);
u_int* MakeTim2ClutDirect2(u_int *pkt_addr, u_int *tim2, int tbp, int cbp);
Q_WORDDATA* SetImageTransParam2(int tbp, int tbw, int psm, u_short w, u_short h);
u_int* ChainPK2Direct(u_int *pkt_addr, u_int *tm2_addr);
void MakeFontTexSendPacket();
void CallFontTexSendPacket();
void CallFontTexAndMesPacket();
void MakeTim2Direct3(u_int *tim2_addr, int tbp, int offset);
void MakeClutDirect3(u_int *tim2_addr, int cbp, int offset);
void MakeTim2ClutDirect3(u_int tm2_addr, int tbp, int cbp, int offset);
void MakeTim2ClutDirect4(u_int tm2_addr, int num, int tbp, int cbp, int offset);
void MakeTim2SendPacket(u_int tm2_addr, int offset);
void MakeTim2SendPacket_3Dpkt(u_int tm2_addr, int offset);
void Reserve2DPacket(u_int pri);
void Reserve2DPacket_Load();
void SetG2DTopPkt(u_int addr);
Q_WORDDATA* Get2DPacketBufferAddress();
void Set2DPacketBufferAddress(Q_WORDDATA *addr);
void DrawAll2DMes_P2();

#ifdef BUILD_EU_VERSION
void SendFontTex();
#endif

#endif // GRAPHICS_GRAPH2D_TIM2_NEW_H
