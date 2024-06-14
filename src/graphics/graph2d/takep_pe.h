#ifndef GRAPHICS_GRAPH2D_TAKEP_PE_H
#define GRAPHICS_GRAPH2D_TAKEP_PE_H

#include "typedefs.h"

typedef enum
{
    ST_WAIT1 = 0,
    ST_GO_AROUND = 1,
    ST_WAIT2 = 2,
    ST_GO_CAMERA = 3,
    ST_FINISH_PE = 4
} ST_PEOBJ;

struct PARTICLE_EFF_OBJ
{ // 0x90
    /* 0x00 */ void *nextpe;
    /* 0x04 */ void *underpe;
    /* 0x08 */ ST_PEOBJ state;
    /* 0x10 */ sceVu0IVECTOR scrvec;
    /* 0x20 */ sceVu0FVECTOR pos;
    /* 0x30 */ sceVu0FVECTOR c2ov_step;
    /* 0x40 */ sceVu0FVECTOR c2ov_move;
    /* 0x50 */ sceVu0FVECTOR c2sv_unit;
    /* 0x60 */ float c2slen;
    /* 0x64 */ float c2olen;
    /* 0x68 */ float steplen;
    /* 0x6c */ int pe_width;
    /* 0x70 */ int m_time;
    /* 0x74 */ int time;
    /* 0x78 */ float nowz;
    /* 0x7c */ float ka;
    /* 0x80 */ float kb;
    /* 0x84 */ int alphabl;
};

#include "graphics/graph2d/tim2.h"

// extern sceVu0IVECTOR particle_eff_col;
// extern u_char eye_light_flg;
// extern sceVu0FVECTOR l_eye_pos;
// extern sceVu0FVECTOR r_eye_pos;
// extern PARTICLE_EFF_OBJ dummy_pe_obj;

void GetBetweenABVecUnitLen(float *out_a2bv, float *out_unit_a2bv, float *out_len, float *in_av, float *in_bv);
void SetPrePEImage(PARTICLE_EFF_OBJ *obj);
int InitPrePEImage(int pe_num);
int SetPEObjPos();
void MakeRandomVec(float *output, int m);
void InitPEObj(float *ov, float *sv, float *tempgv);
void SetPEObjSprSize();
void SetPEObjAlphaBl();
int CtrlPEObj();
void SortRegisterPEObj();
Q_WORDDATA* ParticleEffectInTakingPicture(Q_WORDDATA *qd, u_int b_num);
Q_WORDDATA* SetPESpritePacket(Q_WORDDATA *qd, int *col, int *centervec, int pe_width, int albl);
void SetCenterGravOfFVECTOR(float *input, sceVu0FVECTOR *vec, u_int num);
void GetA2BLoadVector(float *out_v, float *fr_v, float *to_v, float rate);
void CallParticleEffect(u_char ene_id);
void StopParticleEffect();
void SetParticleEffect();
void CallEyeLight(sceVu0FVECTOR *lpos, sceVu0FVECTOR *rpos);
void StopEyeLight();
void EyeLightCtrl();

#endif // GRAPHICS_GRAPH2D_TAKEP_PE_H
