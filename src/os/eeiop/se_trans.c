#include "common.h"

INCLUDE_ASM(const s32, "os/eeiop/se_trans", FGTransInit);

INCLUDE_ASM(const s32, "os/eeiop/se_trans", IsEndFgTrans);

INCLUDE_ASM(const s32, "os/eeiop/se_trans", ChkLoadReqQueueFree);

INCLUDE_ASM(const s32, "os/eeiop/se_trans", ChkFGhostAlreadySet);

INCLUDE_ASM(const s32, "os/eeiop/se_trans", ChkNeedsFGhostSe);

INCLUDE_ASM(const s32, "os/eeiop/se_trans", FloatGhostSENotEmpty);

INCLUDE_ASM(const s32, "os/eeiop/se_trans", GetSeTransSize);

INCLUDE_ASM(const s32, "os/eeiop/se_trans", DmaTransReq);

INCLUDE_ASM(const s32, "os/eeiop/se_trans", ChkDmaTransEndSe);

INCLUDE_ASM(const s32, "os/eeiop/se_trans", TransReqIopCmd);

INCLUDE_ASM(const s32, "os/eeiop/se_trans", TransWaitIopCmd);

INCLUDE_ASM(const s32, "os/eeiop/se_trans", UseOkFGhost);

INCLUDE_ASM(const s32, "os/eeiop/se_trans", SeFGhostTransCtrl);
