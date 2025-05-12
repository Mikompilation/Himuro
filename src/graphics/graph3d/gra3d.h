#ifndef GRAPHICS_GRAPH3D_GRA3D_H
#define GRAPHICS_GRAPH3D_GRA3D_H

#include "typedefs.h"

#include "ingame/camera/camera.h"
#include "ingame/map/furn_spe/furn_spe.h"
#include "ingame/map/furn_spe/fsla_main.h"
#include "graphics/graph3d/load3d.h"
#include "graphics/graph3d/light_dat.h"
#include "graphics/graph3d/sg_dat.h"
#include "graphics/graph3d/sglight.h"
#include "graphics/graph3d/shadow.h"

extern u_int fly_display[3];
extern sceVu0FVECTOR fog_param[64];
extern sceVu0IVECTOR fog_rgb[64];
extern sceVu0FVECTOR fog_param_finder[64];
extern sceVu0IVECTOR fog_rgb_finder[64];
extern int disp_frame_counter;
extern u_char mimchodo_num[];
extern u_int *pmanmodel[70];
extern u_int *pmanmpk[70];
extern u_int *pmanpk2[70];
extern u_int *pgirlbase;
extern u_int *pgirlshadow;
extern u_int *pgirlacs[]; // [2] wrong declaration
extern sceVu0FMATRIX runit_mtx;
extern LIGHT_PACK girls_light;
extern LIGHT_PACK enemy_light;
extern u_int *ssearch_models[15];
extern u_int search_num;
extern u_int *ssearch_models2[15];
extern u_int search_num2;
extern sceVu0FVECTOR room_ambient_light;
extern SgLIGHT room_pararell_light[12];
extern SgLIGHT room_point_light[16];
extern SgLIGHT room_spot_light[16];

u_int* LoadDataFromDVD(u_char *fname);
u_int* LoadDataFromDVD2(u_char *fname, u_int *addr);
void LoadPackedTextureFromMemory(u_int *pointer);
void LoadPackedTexture(u_char *fname);
void CalcRoomCoord(void *sgd_top, sceVu0FVECTOR pos);
void SetUpRoomCoordinate(int disp_room, sceVu0FVECTOR pos);
void ChoudoPreRender(FURN_WRK *furn);
void ChoudoPreRenderDual(FURN_WRK *furn);
void SetPreRender(void *buf, void *light_buf);
void ScenePrerender();
void RequestBlackWhiteMode();
void CancelBlackWhiteMode();
void SetEnvironment();
void SetEnvironment2();
void ClearFrame();
void SetLWS(SgCOORDUNIT *cp, SgCAMERA *camera);
void CalcCoordinate(SgCOORDUNIT *cp, int blocks);
void CopyCoordinate(SgCOORDUNIT *dcp, SgCOORDUNIT *scp, int blocks);
char* appendchar(char *dest, char *source, char *append);
void SetUnitMatrix(u_int *pmodel);
void InitializeRoom(RMDL_ADDR *room_tbl);
void gra3dInitFirst();
u_int PlayerModelInit();
u_int PlayerAccessoryInit(int addr);
u_int SGDLoadInit(u_int *addr, int size);
void gra3dInit();
void Init3D();
void SetDefaultLightPower(float pow);
// void RequestSpirit(int no, int swch);
void InitRequestSpirit();
void RequestFly(int no, int swch);
void InitRequestFly();
void SetSgSpotLight(SPOT_WRK *spot, SgLIGHT *plight);
void SetMyLight(LIGHT_PACK *light_pack, sceVu0FVECTOR eyevec);
void TransMyLight(LIGHT_PACK *dest_pack, LIGHT_PACK *light_pack, sceVu0FVECTOR pos);
void CalcReflectLight();
void AppendReflectLight(LIGHT_PACK *light_pack);
void DeleteReflectLight(LIGHT_PACK *light_pack);
void SetMyLightObj(LIGHT_PACK *trans_pack, LIGHT_PACK *light_pack, sceVu0FVECTOR cam_zd, sceVu0FVECTOR pos);
void SetMyLightRoom(LIGHT_PACK *light_pack, sceVu0FVECTOR eyevec);
void SceneSortUnit();
void Kagu027Control(void *sgd_top);
void MakeDebugValue();
void AppendSearchModel(void *sgd_top, int room_no);
void DrawOneRoom(int no);
void DrawRoom(int disp_no);
int CalcShadowDirecion(ShadowHandle *shandle);
u_int* SearchBoundingBoxPacket(u_int *prim);
void DrawRoomShadow();
void DrawFurniture(int disp_room);
void SetWScissorBox(int disp_room);
void ReSetWScissorBox();
void InitFogSelection();
void FogSelection(int disp_room);
void gra3dDraw();
int CheckModelBoundingBox(sceVu0FMATRIX lwmtx, sceVu0FVECTOR *bbox);
void CalcGirlCoord();
void DrawGirl(int in_mirror);
int DrawEnemy(int no);
int DrawFlyMove(int work_no);

#endif // GRAPHICS_GRAPH3D_GRA3D_H
