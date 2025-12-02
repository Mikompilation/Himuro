#ifndef OUTGAME_SCN_TEST_H
#define OUTGAME_SCN_TEST_H

#include "typedefs.h"

#include "graphics/graph3d/sglight.h"
#include "graphics/scene/scene.h"
#include "graphics/scene/fod.h"

typedef struct { // 0x20
	/* 0x00 */ FOD_EFF_DATA eff_data;
	/* 0x10 */ u_int eff_flg;
	/* 0x14 */ int eff_no;
	/* 0x18 */ int param_num;
	/* 0x1c */ char *eff_name;
} SCN_EFF_CTRL;

typedef struct {
	SgLIGHT light_tmp[36];
	SCENE_FOG fog_tmp;
	sceVu0FVECTOR amb_tmp[6];
	u_int mode;
	u_int init;
	u_int load_status;
	int scene_no;
	int main_menu_csr;
	int now_menu;
	int menu_csr;
	int l_menu_csr;
	int l_sel_csr;
	int effect_csr;
	int db_flg;
	int light_no;
	int effect_id;
	int loop_flg;
	int ene_mdl_no;
} SCN_TEST_WRK;

extern SCN_TEST_WRK scn_test_wrk;

void SceneTestCtrl();
int SceneTestInit();
void SceneTestMain();
void SceneTestSelect();
void SceneTestPlay();
int SceneTestLoad();
void ScnTestMdlLoadReq(u_int *addr);
int ScnTestMdlLoadWait();
void SceneTestMenu();
void SceneTestMainMenu();
void SceneTestLightSelect();
void SceneTestLightData();
void ScnDispPrintVector(char *ttl, sceVu0FVECTOR v, float x, float y);
void SceneTestAmbient();
void RotVectorY(sceVu0FVECTOR normal, sceVu0FVECTOR vector, float delta);
void SceneTestPadCamera();
void SceneTestCameraMode();
void SceneTestFogData();
void SceneTestEffect();
void SceneTestEffectData();
void SceneTestEneEffect();
void SceneTestEneEffFire();
void SceneTestEneEffPDeform();
void SceneTestEneEffOthers();
void SceneTestEffectTest();
void SceneTestSetDefEffect();
void SceneTestVibrate();
int SceneTestDraw(int cnt_flg);
void SceneTestLoopManage(char flg);
int ScnLRCtrl(int *csr, int max, int min);
int ScnUDCtrl(int *csr, int max, int min);
int ScnValueCtrl(float *val, float max, float min, float inc, float mul);
int ScnValueCtrlI(int *val, int max, int min, int inc, int mul);
int ScnValueCtrlC(u_char *val, u_char max, u_char min, int inc, int mul);
int SceneFileSaveBin();
int SceneFileSaveText();

#endif // OUTGAME_SCN_TEST_H
