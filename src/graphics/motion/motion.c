#include "common.h"

INCLUDE_ASM(const s32, "graphics/motion/motion", motInitPlayerAnm);

INCLUDE_ASM(const s32, "graphics/motion/motion", motInitEnemyAnm);

INCLUDE_ASM(const s32, "graphics/motion/motion", motInitOneEnemyAnm);

INCLUDE_ASM(const s32, "graphics/motion/motion", motInitMultiEnemyAnm);

INCLUDE_ASM(const s32, "graphics/motion/motion", motInitEnemyMdl);

INCLUDE_ASM(const s32, "graphics/motion/motion", motInitOneEnemyMdl);

INCLUDE_ASM(const s32, "graphics/motion/motion", motInitMultiEnemyMdl);

INCLUDE_ASM(const s32, "graphics/motion/motion", motInitFlyMdl);

INCLUDE_ASM(const s32, "graphics/motion/motion", motInitMultiFlyMdl);

INCLUDE_ASM(const s32, "graphics/motion/motion", motInitOneFlyMdl);

INCLUDE_ASM(const s32, "graphics/motion/motion", motInitClearAniCtrl);

INCLUDE_ASM(const s32, "graphics/motion/motion", motInitAniCtrl);

INCLUDE_ASM(const s32, "graphics/motion/motion", motInitMotCtrl);

INCLUDE_ASM(const s32, "graphics/motion/motion", motSetCoordCamera);

INCLUDE_ASM(const s32, "graphics/motion/motion", motSetCoord);

INCLUDE_ASM(const s32, "graphics/motion/motion", motInitNewMotion);

INCLUDE_ASM(const s32, "graphics/motion/motion", motAddFrame);

INCLUDE_ASM(const s32, "graphics/motion/motion", motGetNowFrame);

INCLUDE_ASM(const s32, "graphics/motion/motion", ReqPlayerAnime);

INCLUDE_ASM(const s32, "graphics/motion/motion", ReqEnemyAnime);

INCLUDE_ASM(const s32, "graphics/motion/motion", motSetAnime);

INCLUDE_ASM(const s32, "graphics/motion/motion", GetFileInPak);

INCLUDE_ASM(const s32, "graphics/motion/motion", GetPakTaleAddr);

INCLUDE_ASM(const s32, "graphics/motion/motion", GetMdlNeckPos);

INCLUDE_ASM(const s32, "graphics/motion/motion", GetMdlBonePos);

INCLUDE_ASM(const s32, "graphics/motion/motion", GetMdlWaistPos);

INCLUDE_ASM(const s32, "graphics/motion/motion", GetMdlLegPos);

INCLUDE_ASM(const s32, "graphics/motion/motion", GetMdlShldPos);

INCLUDE_ASM(const s32, "graphics/motion/motion", GetPlyrFootPos);

INCLUDE_ASM(const s32, "graphics/motion/motion", GetPlyrAcsLightPos);

INCLUDE_ASM(const s32, "graphics/motion/motion", GetToushuKatanaPos);

INCLUDE_ASM(const s32, "graphics/motion/motion", GetPlyrFtype);

INCLUDE_ASM(const s32, "graphics/motion/motion", motInitInterpAnime);

INCLUDE_ASM(const s32, "graphics/motion/motion", motSetInterpMatrix);

INCLUDE_ASM(const s32, "graphics/motion/motion", motInterpAnm);

INCLUDE_ASM(const s32, "graphics/motion/motion", motInterpMatrix);

INCLUDE_ASM(const s32, "graphics/motion/motion", motMatrix2Quaternion);

INCLUDE_ASM(const s32, "graphics/motion/motion", motQuaternion2Matrix);

INCLUDE_ASM(const s32, "graphics/motion/motion", motQuaternionSlerp);

INCLUDE_ASM(const s32, "graphics/motion/motion", LocalRotMatrixX);

INCLUDE_ASM(const s32, "graphics/motion/motion", LocalRotMatrixY);

INCLUDE_ASM(const s32, "graphics/motion/motion", LocalRotMatrixZ);

INCLUDE_ASM(const s32, "graphics/motion/motion", motInversKinematics);

INCLUDE_ASM(const s32, "graphics/motion/motion", movGetFrameNum);

INCLUDE_ASM(const s32, "graphics/motion/motion", movGetMaxval);

INCLUDE_ASM(const s32, "graphics/motion/motion", movGetMoveval);

INCLUDE_ASM(const s32, "graphics/motion/motion", motGetBoneNum);

INCLUDE_ASM(const s32, "graphics/motion/motion", motGetTransBoneNum);

INCLUDE_ASM(const s32, "graphics/motion/motion", motGetFrameNum);

INCLUDE_ASM(const s32, "graphics/motion/motion", motGetSIFrameNum);

INCLUDE_ASM(const s32, "graphics/motion/motion", motGetInterpFrameNum);

INCLUDE_ASM(const s32, "graphics/motion/motion", motGetRstBuffer);

INCLUDE_ASM(const s32, "graphics/motion/motion", motCheckRstFull);

INCLUDE_ASM(const s32, "graphics/motion/motion", motCheckTransCut);

INCLUDE_ASM(const s32, "graphics/motion/motion", motCheckIncludeRstPacket);

INCLUDE_ASM(const s32, "graphics/motion/motion", motAddressMapping);

INCLUDE_ASM(const s32, "graphics/motion/motion", motGetFrameDataAddr);

INCLUDE_ASM(const s32, "graphics/motion/motion", motGetFrameData);

INCLUDE_ASM(const s32, "graphics/motion/motion", motGetFrameDataRST);

INCLUDE_ASM(const s32, "graphics/motion/motion", motGetFrameDataRT);

INCLUDE_ASM(const s32, "graphics/motion/motion", motGetTransId);

INCLUDE_ASM(const s32, "graphics/motion/motion", motGetParentId);

INCLUDE_ASM(const s32, "graphics/motion/motion", motSetHierarchy);

INCLUDE_ASM(const s32, "graphics/motion/motion", SceneInitAnime);

INCLUDE_ASM(const s32, "graphics/motion/motion", SceneInitOtherAnime);

INCLUDE_ASM(const s32, "graphics/motion/motion", motSetCoordFrame);

INCLUDE_ASM(const s32, "graphics/motion/motion", SceneSetCoordFrame);

INCLUDE_ASM(const s32, "graphics/motion/motion", motSetInvMatrix);

INCLUDE_ASM(const s32, "graphics/motion/motion", motAlign128);

void motPrintVector(void) {
}

INCLUDE_ASM(const s32, "graphics/motion/motion", sceRotMatrixXYZ);
