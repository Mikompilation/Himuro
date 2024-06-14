#include "common.h"

void NewgameMenuMapInit(void) {
}

void LoadgameMenuMapInit(void) {
}

INCLUDE_ASM(const s32, "ingame/menu/ig_map", StartMapModeInit);

INCLUDE_ASM(const s32, "ingame/menu/ig_map", IngameMenuMap);

INCLUDE_ASM(const s32, "ingame/menu/ig_map", IngameMenuMapDisp);

void MapCntInit(void) {
}

INCLUDE_ASM(const s32, "ingame/menu/ig_map", MapCntRenew);

INCLUDE_ASM(const s32, "ingame/menu/ig_map", MapModeInOut);

INCLUDE_ASM(const s32, "ingame/menu/ig_map", MapModeInOut2);

INCLUDE_ASM(const s32, "ingame/menu/ig_map", DspPlyrInMap);

INCLUDE_ASM(const s32, "ingame/menu/ig_map", MapScoop);

INCLUDE_ASM(const s32, "ingame/menu/ig_map", MapMove);

INCLUDE_ASM(const s32, "ingame/menu/ig_map", MapInfo1);

INCLUDE_ASM(const s32, "ingame/menu/ig_map", MapInfo2);

INCLUDE_ASM(const s32, "ingame/menu/ig_map", MapPrint);

INCLUDE_ASM(const s32, "ingame/menu/ig_map", MapPlayer);

INCLUDE_ASM(const s32, "ingame/menu/ig_map", MapChrCtrl);

INCLUDE_ASM(const s32, "ingame/menu/ig_map", MapChrSet);

INCLUDE_ASM(const s32, "ingame/menu/ig_map", PutChrForMap);

INCLUDE_ASM(const s32, "ingame/menu/ig_map", PutIcnForMap);

INCLUDE_ASM(const s32, "ingame/menu/ig_map", PutIcnForMap2);

INCLUDE_ASM(const s32, "ingame/menu/ig_map", DspLayOut);

INCLUDE_ASM(const s32, "ingame/menu/ig_map", MapTri);

INCLUDE_ASM(const s32, "ingame/menu/ig_map", MapExstFlr);

INCLUDE_ASM(const s32, "ingame/menu/ig_map", MapExistCHK);

INCLUDE_ASM(const s32, "ingame/menu/ig_map", MapMovableCHK);

INCLUDE_ASM(const s32, "ingame/menu/ig_map", SetLineStrip);

INCLUDE_ASM(const s32, "ingame/menu/ig_map", SetCircle);

INCLUDE_ASM(const s32, "ingame/menu/ig_map", AngleForWin);
