#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "gameover.h"

#include "os/eeiop/adpcm/ea_ctrl.h"
#include "os/eeiop/adpcm/ea_gameover.h"
#include "os/eeiop/adpcm/ea_scene.h"
#include "os/eeiop/eese.h"
#include "os/system.h"
#include "main/gamemain.h"
#include "main/glob.h"
#include "ingame/enemy/ene_ctl.h"
#include "ingame/entry/entry.h"
#include "ingame/menu/gameover.h"
#include "ingame/menu/ig_menu.h"
#include "ingame/menu/item.h"
#include "ingame/menu/sp_menu.h"
#include "graphics/graph2d/effect_scr.h"
#include "graphics/mov/movie.h"

typedef struct {
    float menu_alp;
    float logo_alp;
    u_char type;
    u_char csr;
    u_char mode;
    u_char fade;
} GO_WRK;

static void GameOverInit();
static void GameOverMenuDisp(u_char alp);

static GO_WRK go_wrk;
static FLSH_CORE flsh;
static int gov_load_id;

#define MAX_VOLUME 0x3fff

void StartGameOver()
{
    ingame_wrk.mode = INGAME_MODE_GAME_OVER;
    ingame_wrk.stts |= 0x80;

    GameOverInit();
    InitGameOver();
    SetGameOverON();

    if (ZeroGhostBattleJudge() || ap_wrk.zh_mode)
    {
        go_wrk.type = 1;
    }
    else
    {
        AdpcmPlayGameOver(AO003_GAMEOVER_STR, MAX_VOLUME, 0);
    }

    SeFadeFlameAll(60, 0);
}

void ReturnGameOver()
{
    ingame_wrk.mode = INGAME_MODE_GAME_OVER;

    GameOverInit();
    InitGameOver();
}

static void GameOverInit()
{
    go_wrk = (GO_WRK) {0};

    ingame_wrk.stts |= 0x20;
}

void GameOverMenuMain()
{
    u_char alp_add;

    switch (go_wrk.type)
    {
    case 1:
        ingame_wrk.stts |= 0x20;

        AdpcmMapNoUse();

        go_wrk.type = 2;
    break;
    case 2:
        MoviePlay(SCENE_NO_9_00_0);
        AdpcmSceneStop();

        go_wrk.type = 3;
    break;
    case 3:
        AdpcmPlayGameOver(AO003_GAMEOVER_STR, MAX_VOLUME, 0);
        InitGameOver();
        SetGameOverON();

        go_wrk.type = 4;
    break;
    case 4:
        SetSysBackColor(0, 0, 0);

        go_wrk.type = 0;
    break;
    }

    if (go_wrk.type != 0)
    {
        return;
    }

    if (go_wrk.mode != 0)
    {
        if (*key_now[6] == 1 || *key_now[5] == 1)
        {
            SeStartFix(1, 0, 0x1000, 0x1000, 0);

            go_wrk.fade = 3;
        }
        else if (
            *key_now[0] == 1 ||
            (*key_now[0] > 25 && (*key_now[0] % 5) == 1) ||
            Ana2PadDirCnt(0) == 1 ||
            (Ana2PadDirCnt(0) > 25 && (Ana2PadDirCnt(0) % 5) == 1)
        )
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 0);

            go_wrk.csr = 1 - go_wrk.csr;
        }
        else if (
            *key_now[1] == 1 ||
            (*key_now[1] > 25 && (*key_now[1] % 5) == 1) ||
            Ana2PadDirCnt(2) == 1 ||
            (Ana2PadDirCnt(2) > 25 && (Ana2PadDirCnt(2) % 5) == 1)
        )
        {
            SeStartFix(0, 0, 0x1000, 0x1000, 0);

            go_wrk.csr = 1 - go_wrk.csr;
        }
    }
    else
    {
        go_wrk.fade = 1;

        if (SetGameOver() == 0xff)
        {
            go_wrk.mode = 1;
            go_wrk.fade = 2;
        }
    }

    alp_add = 6;

    switch (go_wrk.fade)
    {
    case 1:
        if (go_wrk.logo_alp + alp_add < 128.0f)
        {
            go_wrk.logo_alp += alp_add;
        }
        else
        {
            go_wrk.logo_alp = 128.0f;
        }

        go_wrk.menu_alp = 0.0f;
    break;
    case 2:
        if (go_wrk.menu_alp + alp_add < 128.0f)
        {
            go_wrk.menu_alp += alp_add;
        }
        else
        {
            go_wrk.menu_alp = 128.0f;
        }
    break;
    case 3:
        go_wrk.logo_alp = 0.0f;

        if (go_wrk.logo_alp < go_wrk.menu_alp - alp_add)
        {
            go_wrk.menu_alp -= alp_add;
        }
        else
        {
            go_wrk.menu_alp = go_wrk.logo_alp;

            if (go_wrk.csr == 0)
            {
                AlbumModeInGameOverInit();
            }

            else if (go_wrk.csr == 1)
            {
                GameModeChange(1);

                ingame_wrk.stts &= ~0x80;

                AdpcmStopGameOver(0);
            }

        }
    break;
    }

    if (go_wrk.menu_alp != 0.0f)
    {
        GameOverMenuDisp(go_wrk.menu_alp);
    }
}

static void GameOverMenuDisp(u_char alp)
{
    int i;

    CmnWindow(21, 0, 0, alp, 0x80);

    PutSpriteYW(534, 535, 0.0f, 0.0f, 0.0f, 0x808080, alp, 1.0f, 1.0f, 0, 0xff, 1, 0, 0);
    FlashStarYW(&flsh, 96, 64, 90, 0);

    CmnCursol(169, go_wrk.csr * 29 + 202, 302, 30, flsh.alpha, go_wrk.menu_alp, 0x23000);

    for (i = 0; i < 2; i++)
    {
        PutStringYW(6, 11 + i, 320, 204 + i * 29, 0x808080, go_wrk.menu_alp, 0x1e000, 1);
    }
}
