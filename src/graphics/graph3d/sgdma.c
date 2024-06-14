#include "common.h"

INCLUDE_ASM(const s32, "graphics/graph3d/sgdma", SetBlackWhiteCLUT);

INCLUDE_ASM(const s32, "graphics/graph3d/sgdma", ClearBlackWhiteCLUT);

INCLUDE_ASM(const s32, "graphics/graph3d/sgdma", SendDmaON);

INCLUDE_ASM(const s32, "graphics/graph3d/sgdma", SendDmaOFF);

INCLUDE_ASM(const s32, "graphics/graph3d/sgdma", ClearDMATrans);

INCLUDE_ASM(const s32, "graphics/graph3d/sgdma", CheckDMATrans);

INCLUDE_ASM(const s32, "graphics/graph3d/sgdma", CheckDmaSync);

INCLUDE_ASM(const s32, "graphics/graph3d/sgdma", FlushBuffer);

INCLUDE_ASM(const s32, "graphics/graph3d/sgdma", FlushModel);

INCLUDE_ASM(const s32, "graphics/graph3d/sgdma", AppendDmaTag);

INCLUDE_ASM(const s32, "graphics/graph3d/sgdma", AppendDmaBuffer);

INCLUDE_ASM(const s32, "graphics/graph3d/sgdma", AppendDmaTagCall);

INCLUDE_ASM(const s32, "graphics/graph3d/sgdma", AppendDmaTagNextRet);

INCLUDE_ASM(const s32, "graphics/graph3d/sgdma", AppendDmaBufferFromEndAddress);

INCLUDE_ASM(const s32, "graphics/graph3d/sgdma", ClearTextureCache);

INCLUDE_ASM(const s32, "graphics/graph3d/sgdma", PushTextureCache);

INCLUDE_ASM(const s32, "graphics/graph3d/sgdma", PopTextureCahche);

INCLUDE_ASM(const s32, "graphics/graph3d/sgdma", SgSetPacketBuffer);

void SetDmaScratch(void) {
}

void SetDmaBSS(void) {
}

INCLUDE_ASM(const s32, "graphics/graph3d/sgdma", InitialDmaBuffer);

INCLUDE_ASM(const s32, "graphics/graph3d/sgdma", GsImageProcess);

INCLUDE_ASM(const s32, "graphics/graph3d/sgdma", SgTEXTransEnable);

INCLUDE_ASM(const s32, "graphics/graph3d/sgdma", SgTEXTransDisenable);

INCLUDE_ASM(const s32, "graphics/graph3d/sgdma", LoadTRI2Files);

INCLUDE_ASM(const s32, "graphics/graph3d/sgdma", RebuildTRI2Files);

INCLUDE_ASM(const s32, "graphics/graph3d/sgdma", GetTextureAnimation);

INCLUDE_ASM(const s32, "graphics/graph3d/sgdma", LoadTextureAnimation);
