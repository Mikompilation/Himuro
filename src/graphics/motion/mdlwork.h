#ifndef GRAPHICS_MOTION_MDLWORK_H
#define GRAPHICS_MOTION_MDLWORK_H

#include "typedefs.h"

#include "graphics/graph3d/sg_dat.h"
#include "graphics/motion/ani_dat.h"
#include "graphics/motion/mdldat.h"
#include "graphics/motion/motion.h"
#include "graphics/motion/mime_dat.h"

typedef struct {
	u_short pk2;
	u_short mpk;
	u_short acs;
	u_short anm;
	u_short bwc;
	u_short clt;
} PLYR_FILE_ID;

extern u_int *plyr_clut_addr;
extern u_int *plyr_bwc_addr;

void ManmdlSetAlpha(void *sgd_top, u_char alpha);
char motCheckTrRateMdl(u_int mdl_no);
u_int* MpkMapUnit(u_int *mpk_p);
void DrawGirlSubObj(u_int *mpk_p, u_char alpha);
void DrawEneSubObj(u_int *mpk_p, u_char alpha1, u_char alpha2);
void SortUnitRefCoordKind(void *sgd_top, SgCOORDUNIT *coordp, int num);
void SortUnitRefCoordP(void *sgd_top, SgCOORDUNIT *coordp, int pnum);
void SortUnitRefCoord(void *sgd_top, SgCOORDUNIT *coordp, int pnum);
void InitEneVramCtrl();
void InitEneVramCtrlSub(ENE_VRAM_CTRL *ev_ctrl);
void SetEneTexture(u_int work_id);
void SetEneVram(u_int *mdl_p, int offset);
void ReleaseEneTexture(u_int work_id);
void SendManMdlTex();
void SetTextureToScene();
void SetTextureAfterScene();
void MpkAddTexOffset(u_int *mpk_p, int offset);
void SetManmdlTm2(u_int *pak_addr, int offset, char mode);
void BackupEneVramCtrl();
void SetupEneVramCtrl();
void motPrintVramCtrl();
void ManTexflush();
void SgdAddTexOffset(void *sgd_top, int offset);
u_int* GetOkuraBuffer(u_char work_id);
void ReqMsnInitPlyr(u_char msn_no);
char MsnInitPlyr();
void motInitMsn();

#ifdef MATCHING_DECOMP
#ifdef INCLUDING_FROM_MDLWORK_C
// fix for undeclared motInitAniMdlBuf in header
void motInitAniMdlBuf();
#endif
#else
void motInitAniMdlBuf();
#endif

void motInitAniMdlBufSub(ANI_MDL_CTRL *am_ctrl);
u_char motSetAniMdlBuf(u_int anm_no, u_int *anm_p);
void motReleaseOneAniMdlBuf(u_int anm_no, u_int *anm_p);
void motReleaseAniMdlBuf(u_int anm_no, u_int *anm_p);
ANI_CTRL* motGetAniMdl(u_int work_no);
void motInitPacketCtrl();
void motSetAnmPacket(u_char no);
void motReleaseAnmPacket(u_char no);
void motEneTexAnm(ANI_CTRL *ani_ctrl, u_int work_id);
void motInitEneKuttuki();
void motSetEnemyKuttuki(u_char work_id, u_char bone_id, float radius, float ry);
void motReleaseEnemyKuttuki(u_char work_id);
void SetPlyrClut(int bw_flg);

#endif // GRAPHICS_MOTION_MDLWORK_H
