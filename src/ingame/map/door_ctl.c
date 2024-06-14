#include "common.h"

INCLUDE_ASM(const s32, "ingame/map/door_ctl", IsUseDoor);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", SearchDoorWrk);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", SearchAcDoor);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", SearchNcDoor);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", ClearDoorAttrFlg);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", InitDoorAttrFlg);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorPassRoom);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorRDoorSetRotOK);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorPassSetSttsMap);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorPassSetHide);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorPassSetTouch);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorAcInit);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorCtrlInit);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", InitDoorOpenCtrl);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", InitAreaReadWrk);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorDataInit);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", SetDoorSttsData);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", SetDoorSttsEve);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", SetDoorLockEve);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorSetAllLocked);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", IsInDoorSpuare);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", GetRoomIdBeyondDoor2);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", GetRoomIdBeyondDoor);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", IsAllConnectDoorClose);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", LockAllDoorInNowRoom);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", UnlockAllDoorInNowRoom);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", IsDoorLoad);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", GetRoomDoorID);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", SetDoorWrk);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorSetFurnWrk);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorCtrlMain);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", IsDoorOpenCtrlMode);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorCtrlOpen);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", SetPlyrPosAfterDOpen);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", SetNcDwrk);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", SetAcDwrk);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorCtrlAutoClose);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", AllCloseAcDoor);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", AllCloseConnectDoor);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorReOpenCheck);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorCtrlNoClose);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorOpenCtrlRot);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorOpenCtrlSlide);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorOpenCheckMain);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorCheckOn);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorCheckOff);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", SetDoorOpenPlyrPos);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorOpenJudge);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", SetDoorOpenCameraNo);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", AreaReadCheck);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorOpenCheck);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", GetDoorBaseRot);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorOpenCheckSquareArea);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorOpenSttsPermissionCheck);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorInitDSM);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorSetDSM);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorGetOpenSquareNo);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorKeyLock);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorKeyLockOnly);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorKeyUnlock);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorKeyUnlockOnly);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorLockChange);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorSttsChange);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorHitCheck);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorHitCheck2);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorCoverCheck);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", GetFusumaHitVertex);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", GetFusumaHitVertex2);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", GetDoorHitVertex);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", GetDoorHitVertex2);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", ChkRotBetween);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorDataRenewNow);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorFreeFurnWrk);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorJoint2Next);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorDataRenewNext);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", GetNextRIdFromRNoDId);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", NextRoomRenew);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", ChkNextRoomIsInsite);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", GetDoorRoomNumP);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", GetDoorRoomNumPFloor);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", GetDoorDoorNumP);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", GetDoorRoomConectDataP);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", GetDoorDataTopP);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", GetDoorDataHeader);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", ChkDoorIsInsite);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", ChkDoorIsInsite2);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", TmpMapHit);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", ChkDoorIsOpen);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", GetDoorIdFromDoorNo);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", GetDoorStts);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", GetDoorSttsForMap);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorChangeModel);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorDispOnOff);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorOpenShiftForce);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", TargetDoorKindChk);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", PlyrPosAdj);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", DoorMoveStart);

INCLUDE_ASM(const s32, "ingame/map/door_ctl", OneRoomLoadReq);

void PrintDoorID(void) {
}
