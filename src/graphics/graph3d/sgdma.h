#ifndef GRAPHICS_GRAPH3D_SGDMA_H
#define GRAPHICS_GRAPH3D_SGDMA_H

#include "typedefs.h"

#include "ingame/map/furn_dat.h"

void SetBlackWhiteCLUT();
void ClearBlackWhiteCLUT();
void SendDmaON();
void SendDmaOFF();
void ClearDMATrans();
void CheckDMATrans();
void CheckDmaSync();
void FlushModel(int signal);
void AppendDmaTag(u_int addr, int size);
void AppendDmaBuffer(int size);
void AppendDmaTagCall(u_int next_tag_addr);
void AppendDmaTagNextRet(void *tag_addr);
void AppendDmaBufferFromEndAddress(qword *end_adr);
void ClearTextureCache();
void PushTextureCache();
void PopTextureCahche();
void SgSetPacketBuffer(void *packet_buffer, int packet_size, void *tag_buffer, int tag_num);
void SetDmaScratch();
void SetDmaBSS();
void InitialDmaBuffer();
void GsImageProcess(u_int *prim);
void SgTEXTransEnable();
void SgTEXTransDisenable();
void LoadTRI2Files(u_int *prim);
void RebuildTRI2Files(u_int *prim);
TextureAnimation* GetTextureAnimation(void *sgd_top);
void LoadTextureAnimation(u_int *prim);

#endif // GRAPHICS_GRAPH3D_SGDMA_H
