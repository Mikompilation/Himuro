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

typedef struct {
    void *nextpe;
    void *underpe;
    ST_PEOBJ state;
    sceVu0IVECTOR scrvec;
    sceVu0FVECTOR pos;
    sceVu0FVECTOR c2ov_step;
    sceVu0FVECTOR c2ov_move;
    sceVu0FVECTOR c2sv_unit;
    float c2slen;
    float c2olen;
    float steplen;
    int pe_width;
    int m_time;
    int time;
    float nowz;
    float ka;
    float kb;
    int alphabl;
} PARTICLE_EFF_OBJ;

// extern sceVu0IVECTOR particle_eff_col;
// extern u_char eye_light_flg;
// extern sceVu0FVECTOR l_eye_pos;
// extern sceVu0FVECTOR r_eye_pos;
// extern PARTICLE_EFF_OBJ dummy_pe_obj;

void GetBetweenABVecUnitLen(sceVu0FVECTOR out_a2bv, sceVu0FVECTOR out_unit_a2bv, float *out_len, sceVu0FVECTOR in_av, sceVu0FVECTOR in_bv);
void SetPrePEImage(PARTICLE_EFF_OBJ *obj);
int InitPrePEImage(PARTICLE_EFF_OBJ *obj, int pe_num);
int SetPEObjPos(PARTICLE_EFF_OBJ *obj);
void MakeRandomVec(float *output, int m);
void InitPEObj(PARTICLE_EFF_OBJ *obj, sceVu0FVECTOR ov, sceVu0FVECTOR sv, sceVu0FVECTOR tempgv);
void SetPEObjSprSize(PARTICLE_EFF_OBJ *obj);
void SetPEObjAlphaBl(PARTICLE_EFF_OBJ *obj);
int CtrlPEObj(PARTICLE_EFF_OBJ *obj);
void SortRegisterPEObj(PARTICLE_EFF_OBJ *obj);
Q_WORDDATA* ParticleEffectInTakingPicture(Q_WORDDATA *qd, u_int b_num);
Q_WORDDATA* SetPESpritePacket(Q_WORDDATA *qd, sceVu0IVECTOR col, sceVu0IVECTOR centervec, int pe_width, int albl);
void SetCenterGravOfFVECTOR(float *input, sceVu0FVECTOR *vec, u_int num);
void GetA2BLoadVector(sceVu0FVECTOR out_v, sceVu0FVECTOR fr_v, sceVu0FVECTOR to_v, float rate);
void CallParticleEffect(u_char ene_id);
void StopParticleEffect();
void SetParticleEffect();
void CallEyeLight(sceVu0FVECTOR *lpos, sceVu0FVECTOR *rpos);
void StopEyeLight();
void EyeLightCtrl();

#endif // GRAPHICS_GRAPH2D_TAKEP_PE_H
