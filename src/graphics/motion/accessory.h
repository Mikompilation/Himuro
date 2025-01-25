#ifndef GRAPHICS_MOTION_ACCESSORY_H
#define GRAPHICS_MOTION_ACCESSORY_H

#include "typedefs.h"

#include "graphics/graph3d/sg_dat.h"
#include "graphics/motion/acs_dat.h"

#include "ingame/map/furn_ctl.h"
#include "graphics/motion/mdlwork.h"

// extern SPRING rope_spring[20][9];
// extern C_PARTICLE rope_particle[20][10];

void InitPlyrAcsAlpha();
void DispPlyrAcs(u_int *base_p, u_int *mdl_p, ACS_ALPHA *acs_ctrl, u_int bone_id);
void PlyrAcsAlphaCtrl();
void acsInitRopeWork();
void acsRopeSetWork(u_int furn_id);
void acsRopeReleaseWork(u_int furn_id);
sceVu0FVECTOR* acsGetRopePos(u_int furn_id);
void acsInitRopeSub(u_int work_id, u_int furn_id, u_int type);
void acsRopeMoveRequest(u_int furn_id, u_char move_mode, float pow);
void acsRopeMoveStop(u_int furn_id);
u_char acsCheckRopeMoveExec(u_int furn_id);
void acsCalcCoordinate(SgCOORDUNIT *cp, u_int block_num, FURN_WRK *f_wrk);
void acsSetMoveDir(float *dir);
void acsRopeMoveCtrl();
void acsRopeMoveWind(char dir_cng);
void acsRopeMoveVib();
void acsRopeMoveFall();
void acsMoveRope(SgCOORDUNIT *cp, COLLISION_DAT *collision, float *l2w_inscale[4]);
u_int* acsInitCloth(CLOTH_CTRL *cloth_top, u_int *mpk_p, u_int *top_addr, u_int mdl_no);
void acsClothCtrl(ANI_CTRL *ani_ctrl, u_int *mpk_p, u_int mdl_no, u_char scene_flg);
void acsMoveCloth(sceVu0FVECTOR *vtx, CLOTH_CTRL *cloth, SgCOORDUNIT *cp, COLLISION_DAT *collision, float scale, float collision_scale, u_int mdl_no);
u_char acsCheckCollisionSphere(SPHERE *sphere, float *current, float *relative_v, float Ke);
u_char acsCheckCollisionTube(TUBE *tube, float *current, float *relative_v, float Ke);
void SetLWS2(SgCOORDUNIT *cp, SgCAMERA *camera);
void acsInitChodoWork();
void acsChodoSetWork(u_int furn_id);
void acsChodoReleaseWork(u_int furn_id);
void acsChodoMoveRequest(u_int furn_id);
void acsChodoMoveStop(u_int furn_id);
void acsChodoMove(CMOVE_CTRL *mv, u_int *sgd_top);
void acsChodoMoveKage(SgCOORDUNIT *cp, float *p);
char acsChodoMoveBird(SgCOORDUNIT *cp, float *p);
void acsChodoMoveNingyo(SgCOORDUNIT *cp, float *p);
char acsChodoMoveKaidan(SgCOORDUNIT *cp, float *p1);
void SetSpeFurnLight(FURN_WRK *furn);

#endif // GRAPHICS_MOTION_ACCESSORY_H
