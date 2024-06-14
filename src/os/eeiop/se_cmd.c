#include "common.h"

void SeCtrlInit(void) {
}

INCLUDE_ASM(const s32, "os/eeiop/se_cmd", SeCtrlMain);

INCLUDE_ASM(const s32, "os/eeiop/se_cmd", SeCmdGetPan);

INCLUDE_ASM(const s32, "os/eeiop/se_cmd", SeCmdGetPhase);

INCLUDE_ASM(const s32, "os/eeiop/se_cmd", SeCmdGetPitch);

INCLUDE_ASM(const s32, "os/eeiop/se_cmd", SeCmdGetVol);

void SeCmdSetVolForStts(void) {
}

void SeCmdAjstVolChng(void) {
}

void SeCmdFdIn(void) {
}

INCLUDE_ASM(const s32, "os/eeiop/se_cmd", SeCmdFdOut);

void SeSetVolReg(void) {
}

INCLUDE_ASM(const s32, "os/eeiop/se_cmd", SeCmdGetAngle);

void SeCmdGetRotRatio(void) {
}

INCLUDE_ASM(const s32, "os/eeiop/se_cmd", SeCmdSetPitch);

void SeCmdGetDataPitch(void) {
}
