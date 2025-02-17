#include "common.h"

#ifdef MATCHING_DECOMP
#define INCLUDING_FROM_CAMERA_C
#include "mdlwork.h"
#undef INCLUDING_FROM_CAMERA_C
#else
#include "camera.h"
#endif

INCLUDE_ASM(const s32, "ingame/camera/camera", CameraMain);

INCLUDE_ASM(const s32, "ingame/camera/camera", KonwakuCamCtrl);

INCLUDE_ASM(const s32, "ingame/camera/camera", FinderInCameraCtrl);

INCLUDE_ASM(const s32, "ingame/camera/camera", NormalCameraCtrl);

INCLUDE_ASM(const s32, "ingame/camera/camera", GetCameraInfo);

INCLUDE_ASM(const s32, "ingame/camera/camera", GetCameraData);

INCLUDE_ASM(const s32, "ingame/camera/camera", SetCamPos0);

INCLUDE_ASM(const s32, "ingame/camera/camera", SetCamPos1);

INCLUDE_ASM(const s32, "ingame/camera/camera", SetCamPos2);

INCLUDE_ASM(const s32, "ingame/camera/camera", SetCamPos3);

INCLUDE_ASM(const s32, "ingame/camera/camera", SetCamPos4);

INCLUDE_ASM(const s32, "ingame/camera/camera", SetCamPos5);

INCLUDE_ASM(const s32, "ingame/camera/camera", GetMCLocalPosPer);

INCLUDE_ASM(const s32, "ingame/camera/camera", CompleCameraPos);

INCLUDE_ASM(const s32, "ingame/camera/camera", CompleReqChk);

INCLUDE_ASM(const s32, "ingame/camera/camera", FinderModeCameraCtrl);

INCLUDE_ASM(const s32, "ingame/camera/camera", PlyrCamCondChk);

INCLUDE_ASM(const s32, "ingame/camera/camera", PconMahiCameraCtrl);

INCLUDE_ASM(const s32, "ingame/camera/camera", PconTebureCameraCtrl);

INCLUDE_ASM(const s32, "ingame/camera/camera", FinderModePadChk);

INCLUDE_ASM(const s32, "ingame/camera/camera", SetFinderRot);

INCLUDE_ASM(const s32, "ingame/camera/camera", CameraIdMoveCtrl);

INCLUDE_ASM(const s32, "ingame/camera/camera", SetMapCamDat0);

INCLUDE_ASM(const s32, "ingame/camera/camera", SetMapCamDat1);

INCLUDE_ASM(const s32, "ingame/camera/camera", SetMapCamDat2);

INCLUDE_ASM(const s32, "ingame/camera/camera", SetMapCamDat3);

INCLUDE_ASM(const s32, "ingame/camera/camera", SetMapCamDat4);

INCLUDE_ASM(const s32, "ingame/camera/camera", SetMapCamDat5);

INCLUDE_ASM(const s32, "ingame/camera/camera", SaveCamDat);

INCLUDE_ASM(const s32, "ingame/camera/camera", ReqFinderInOverRap);

INCLUDE_ASM(const s32, "ingame/camera/camera", FinderInOverRapCtrl);

INCLUDE_ASM(const s32, "ingame/camera/camera", PlyrDmgCameraCtrl);

INCLUDE_ASM(const s32, "ingame/camera/camera", ReqDramaCamera);

INCLUDE_ASM(const s32, "ingame/camera/camera", DramaCameraReqCtrl);

INCLUDE_ASM(const s32, "ingame/camera/camera", ClearDramaCamReq);
