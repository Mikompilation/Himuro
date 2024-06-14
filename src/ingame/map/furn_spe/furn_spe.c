#include "common.h"

void NewgameFActInit(void) {
}

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", InitFurnDatSave);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", PreGameInitFActWrk);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", InitFActWrk);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", FActWaitExecJob);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", FActWrkMain);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", SearchRegisterFW2FAW);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", ExchangeFAWFWID);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", FSpeFinishFlgOff);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", FurnActOffEve);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", FurnActOnEve);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", SearchFActWrkIDFromFurnID);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", AddRotVector);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", FActCheckExec);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", FreeFActFwrk);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", SearchFurnWrkIDFromFurnID);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", FreeFActFwrkNo);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", ForwardActionPointer);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", ForwardConditionPointer);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", SetFurnActData);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", RegisterFW2FAW);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", GetOpenedFAWID);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", SearchFActWrkIDFromFWID);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", FActStopAction);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", FActExecOccur);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", FActWait);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", CompareFloatVal);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", ChkANDCondition);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", FSpeCheckOutOfRoom);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", FACheckCondition);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", FACheckDirect);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", RotCheck);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", FACheckDistNear);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", CheckSquareAreaOne);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", FACheckCircleArea);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", FACheckSquareArea);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", FACheckDistFar);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", FACheckStatus);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", SpeedLessOrVecF);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", SpeedLessOrVecVec);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", FACheckSpeedLess);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", FSpeTexAnmCtrl);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", FSpeTexAnmStop);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", FSpeTexAnmExe);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", FSpeDelPointLight);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", FSpeSetPointLight);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", GetRandVal);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", StopTrembleH);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", GetVacantTrembleHWrk);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", CallTrembleH);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", TrembleHCtrl);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", FSpeTrembleCtrl);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", CallThunderLight);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", SetThunderLightEach);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", SetThunderLight);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", InitThunderLight);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", ThunderLight);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", CallLampTremble);

INCLUDE_ASM(const s32, "ingame/map/furn_spe/furn_spe", StopLampTremble);
