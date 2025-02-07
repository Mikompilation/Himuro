#ifndef GRAPHICS_MOTION_MIME_H
#define GRAPHICS_MOTION_MIME_H

#include "typedefs.h"

#include "ingame/map/furn_dat.h"
#include "graphics/motion/mime_dat.h"
#include "graphics/motion/acs_dat.h"
#include "graphics/motion/ani_dat.h"

u_int* mimInitMimeCtrl(MIME_CTRL *m_ctrl, MIME_DAT *mdat, u_int *mim_p, u_int *mdl_p, u_int *tmp_p);
void mimInitAcsCtrl(ANI_CTRL *ani_ctrl, u_short mdl_no);
void mimInitLoop(ANI_CTRL *ani_ctrl);
void mimRequest(MIME_CTRL *m_ctrl, u_char rev);
void mimRequestLastFrame(MIME_CTRL *m_ctrl, u_char rev);
void mimMepatiReq(u_char mode, u_char rev);
void mimLNigiriReq(u_char mode, u_char rev);
void mimRNigiriReq(u_char mode, u_char rev);
void mimStop(MIME_CTRL *m_ctrl);
void mimSetReso(MIME_CTRL *m_ctrl, u_char reso);
void mimClearVertex(MIME_CTRL *m_ctrl);
void mimCalcVertex(MIME_CTRL *m_ctrl, WMIM_CTRL *wmim, u_char clear_vtx_flg);
void mimSetMimeCtrl(MIME_CTRL *m_ctrl, MIME_DAT *mdat, u_int furn_id, u_int parts_id);
u_int* mimSetMimeDat(MIME_DAT *mdat, u_int *mim_p, u_int *tmp_buf, u_int *mdl_p);
void mimSetOriVertex(MIME_DAT *mdat);
void mimSetVertex(ANI_CTRL *ani_ctrl);
void mimClearToScene();
void mimClearAllVertex(ANI_CTRL *ani_ctrl);
u_char mimAddFrame(MIME_CTRL *m_ctrl, u_int *dat);
void SceneMimSetVertex(ANI_CTRL *ani_ctrl, u_int frame);
void mimChodoInitWork();
void mimChodoSetWork(u_int furn_id, u_char room_no);
void mimChodoReleaseWork(u_int furn_id, u_char room_no);
void mimInitChodo(u_int *mim_p, u_short *furn_id, u_char room_id, u_short room_no, u_int num);
void mimDispChodo(MIME_CTRL *m_ctrl, u_int *mdl_p);
u_char mimChodoChkExec(u_int furn_id);
void mimChodoRequest(FURN_ACT_WRK *fawp, u_char mode, u_char rev);
void mimChodoStop(u_int furn_id);
void mimChodoSetReso(u_int furn_id, u_char reso);
u_int mimGetFrameNum(u_int *mim_top);
u_int mimGetKeymdlNum(u_int *mim_top);
u_int mimGetPartsNo(u_int *mim_top);
u_int mimGetFlg(u_int *mim_top);
u_char* mimGetWeightName(u_int *mim_top);
float mimGetWavdata(u_int *mim_top, u_int key_no, u_int frame);
sceVu0FVECTOR* mimGetKeymdlTop(u_int *mim_top, u_int key_no);
void mimAddressMapping(u_int *top_addr);
void mimPlyrMepatiCtrl();
void mimInitWeight(ANI_CTRL *ani_ctrl);
void mimWeightCtrl(ANI_CTRL *ani_ctrl, u_int weight_id, float scale);

#endif // GRAPHICS_MOTION_MIME_H
