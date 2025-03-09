#include "common.h"
#include "typedefs.h"
#include "leditor.h"

#include "sce/sifdev.h"

#include "main/glob.h"
#include "graphics/graph3d/gra3d.h"
#include "graphics/graph3d/sglib.h"
#include "graphics/graph3d/sgsgd.h"
#include "graphics/graph3d/load3d.h"
#include "graphics/graph3d/sglight.h"

#include "data/room_name.h" // static char *room_name[];
static u_int *ldata_top;
static u_int *pdata_top;
static u_int *sdata_top;

static sceVu0FVECTOR le_ambient;
static SgLIGHT le_lights[3];
static SgLIGHT le_plights[16];
static SgLIGHT le_slights[16];

static int fog_mode;

#define PI 3.1415925f

void ReadLights(ROOM_LIGHT *rdata, void *buf)
{
    int num;
    int i;
    u_int *prim;
    sceVu0FVECTOR *pvec;

    prim = (u_int *)((int *)buf)[6]; // structure ??

    rdata->sdata = NULL;
    rdata->pdata = NULL;
    rdata->ldata = NULL;

    if (prim[0] == 0)
    {
        return;
    }

    while (prim[0] != 0)
    {
        if (prim[1] == 11)
        {
            num = prim[3];

            switch (prim[2])
            {
            case 0:
                pvec = (sceVu0FVECTOR *)&prim[4];
                rdata->ldata = prim;
                num = rdata->lnum < num ? rdata->lnum : num;

                if (rdata->lights != NULL)
                {
                    for (i = 0; i < num; i++)
                    {
                        Vu0CopyVector(rdata->lights[i].diffuse, pvec[0]);
                        Vu0CopyVector(rdata->lights[i].specular, pvec[0]);
                        Vu0CopyVector(rdata->lights[i].ambient, pvec[0]);
                        Vu0CopyVector(rdata->lights[i].direction, pvec[1]);
                        
                        pvec += 2;
                    }
                }
            break;
            case 1:
                rdata->pdata = prim;
                num = rdata->pnum < num ? rdata->pnum : num;

                if (rdata->plights != NULL)
                {
                    for (i = 0; i < num; i++)
                    {
                        // debug code ? optimized out?
                    }
                }
            break;
            case 2:
                pvec = (sceVu0FVECTOR *)&prim[4];
                // debug code ? optimized out?
            break;
            }
        }
    }
}

static void LoadLightData(DEBUG_MENU **dbgmenu_tbl, int disp_room)
{
    int i;
    int sub_menu_num;
    int nnum;
    DEBUG_SUB_MENU *psubmenu;
    
    SgReadLights(NULL, room_addr_tbl[disp_room].lit_data, le_ambient, le_lights, 3, le_plights, 16, le_slights, 16);
    
    sub_menu_num = 1;
    
    if (0 < le_lights[0].num)
    {
        psubmenu = &dbgmenu_tbl[18]->submenu[sub_menu_num];
            
        psubmenu->subnum = 4116;
        psubmenu->nmax = 0;
        psubmenu->name = "Infinite";
        
        for (i = 0; i < le_lights[0].num; i++)
        {
            psubmenu = &dbgmenu_tbl[20]->submenu[i];
            
            sprintf(psubmenu->name, "Infinite %d", i);
            
            psubmenu->nmax = 0;
            psubmenu->subnum = 4121;
        } 
        
        sub_menu_num = 2;
        
        psubmenu = &dbgmenu_tbl[20]->submenu[le_lights[0].num];
        
        memcpy(psubmenu->name, "_end_", 6); // ??
        psubmenu->nmax = 0;
        psubmenu->subnum = 0;
    }
    
    if (0 < le_plights[0].num)
    {
        nnum = (le_plights[0].num / 9) + 1;
        
        for (i = 0; i < nnum; i++)
        {
            psubmenu = &dbgmenu_tbl[18]->submenu[sub_menu_num];
            
            sprintf(psubmenu->name, "Point%d", i);
            
            psubmenu->nmax = i;
            psubmenu->subnum = 4117;

            sub_menu_num++;
        } 
    }
    
    if (0 < le_slights[0].num)
    {
        nnum = (le_slights[0].num / 9) + 1;
        
        for (i = 0; i < nnum; i++)
        {
            psubmenu = &dbgmenu_tbl[18]->submenu[sub_menu_num];
            
            sprintf(psubmenu->name, "Spot%d", i);
            
            psubmenu->nmax = 0;
            psubmenu->subnum = 4118;

            sub_menu_num++;
        }
    }
    
    psubmenu = &dbgmenu_tbl[18]->submenu[sub_menu_num];
    
    psubmenu->name = "Ambient";
    psubmenu->subnum = 4145;
    psubmenu->nmax = 0; 

    sub_menu_num++;
    
    psubmenu = &dbgmenu_tbl[18]->submenu[sub_menu_num];
    
    psubmenu->name = "_end_";
    psubmenu->subnum = 0;
    psubmenu->nmax = 0;
    
    for (i = sub_menu_num + 1; i < 10; i++)
    {
        psubmenu = &dbgmenu_tbl[18]->submenu[i];
        
        psubmenu->name = "";
        psubmenu->subnum = 0;
        psubmenu->nmax = 0;
    }
}

static void GetSGDLightDataPointer(int room_no, u_int **amb_p, u_int **infinite_p, u_int **point_p, u_int **spot_p)
{
    u_int *prim;
    
    if (room_addr_tbl[room_no].lit_data != NULL)
    {
        *spot_p = NULL;
        *point_p = NULL;
        *infinite_p = NULL;
        *amb_p = NULL;
        
        prim = (u_int *)room_addr_tbl[room_no].lit_data[6];
        
        while (*prim != NULL)
        {
            if (prim[1] == 11)
            {
                switch (prim[2])
                {
                case 1:
                    *point_p = prim;
                break;
                case 0:
                    *infinite_p = prim;
                break;
                case 2:
                    *spot_p = prim;
                break;
                case 3:
                    *amb_p = prim;
                break;
                }
            }
            
            prim = (u_int*)*prim;
        }
    }
}

static void WriteSGDLightDataPointer(int room_no, u_int **amb_p, u_int **infinite_p, u_int **point_p, u_int **spot_p)
{
    int i;
    sceVu0FVECTOR *pvec;
    
    if (*amb_p != NULL)
    {
        pvec = (sceVu0FVECTOR *)&(*amb_p)[4];
        
        Vu0CopyVector(pvec[0], le_ambient); 
    }
    
    if (*infinite_p != NULL)
    {
        if (le_lights[0].num != 0)
        {
            pvec = (sceVu0FVECTOR *)&(*infinite_p)[4];

            for (i = 0; i < le_lights[0].num; i++)
            {
                Vu0CopyVector(pvec[0], le_lights[i].diffuse);
                Vu0CopyVector(pvec[1], le_lights[i].direction);
                
                pvec += 2; 
            }
        }
    }
    
    if (*point_p != NULL)
    {
        if (le_plights[0].num != 0)
        {
            pvec = (sceVu0FVECTOR *)&(*point_p)[4];

            for (i = 0; i < le_plights[0].num; i++) 
            {
                Vu0CopyVector(pvec[0], le_plights[i].diffuse);

                (*pvec)[3] = (le_plights[i].intens / le_plights[i].ambient[0]);
                pvec += 2;
            }
        }
    }
    
    if (*spot_p != NULL) 
    {
        if (le_slights[0].num != 0)
        {
            pvec = (sceVu0FVECTOR *)&(*spot_p)[4];

            for (i = 0; i < le_slights[0].num; i++) 
            {
                Vu0CopyVector(pvec[0], le_slights[i].diffuse);

                (*pvec + 4)[3] = le_slights[i].ambient[1];
                pvec += 3;
            }
        }
    }
}

static void SaveLightData(int room_no)
{
    u_int *amb_p;
    u_int *infinite_p;
    u_int *point_p;
    u_int *spot_p;
    int len;
    int rfd;
    static char LoadDir[31] = "host0:../bin/data/3ddata/room/";
    char filename[100];
    char filename2[100];
    u_int **sgd_top;
    
    if (room_addr_tbl[room_no].lit_data != NULL)
    {
        sgd_top = &room_addr_tbl[room_no].lit_data;
        
        GetSGDLightDataPointer(room_no, &amb_p, &infinite_p, &point_p, &spot_p);

        sprintf(filename2, "%s", room_name[room_no]);

        rfd = sceOpen(appendchar(filename, LoadDir, filename2), SCE_RDONLY);

        if (rfd > -1)
        {
            len = sceLseek(rfd, 0, SCE_SEEK_END);
            sceLseek(rfd, 0, SCE_SEEK_SET);
            sceRead(rfd, *sgd_top, len);
            sceClose(rfd);

            WriteSGDLightDataPointer(room_no, &amb_p, &infinite_p, &point_p, &spot_p);

            rfd = sceOpen(appendchar(filename, LoadDir, filename2), SCE_WRONLY);

            if (rfd < 0)
            {
                SgMapUnit(*sgd_top);
            }
            else
            {
                sceLseek(rfd, 0, SCE_SEEK_SET);
                sceWrite(rfd, *sgd_top, len);
                sceClose(rfd);

                SgMapUnit(*sgd_top);
            }
        }
    }
}

void ApplyLight(int room_no)
{
    void *mdl_addr;
    SgCOORDUNIT *cp; 
    u_int *amb_p;
    u_int *infinite_p;
    u_int *point_p;
    u_int *spot_p;
    
    PushLight();

    SgSetAmbient(le_ambient);
    SgSetInfiniteLights(camera.zd, le_lights, le_lights[0].num);
    SgSetPointLights(le_plights, le_plights[0].num);
    SgSetSpotLights(le_slights, le_slights[0].num);
    
    mdl_addr = room_addr_tbl[room_no].near_sgd;

    if (mdl_addr != NULL)
    {
        HeaderSection *h = mdl_addr;

        cp = h->coordp;

        sceVu0UnitMatrix(cp->matrix);
        CalcCoordinate(cp, h->blocks - 1);
        SgClearPreRender(mdl_addr, -1);
        SgPreRender(mdl_addr, -1);

        cp->flg = 0;
    }
    
    mdl_addr = room_addr_tbl[room_no].ss_sgd;
    
    if (mdl_addr != NULL)
    {
        HeaderSection *h = mdl_addr;

        cp = h->coordp;
        
        sceVu0UnitMatrix(cp->matrix);
        CalcCoordinate(cp, h->blocks - 1);
        SgClearPreRender(mdl_addr, -1);
        SgPreRender(mdl_addr, -1);
    }

    PopLight();

    GetSGDLightDataPointer(room_no, &amb_p, &infinite_p, &point_p, &spot_p);
    WriteSGDLightDataPointer(room_no, &amb_p, &infinite_p, &point_p, &spot_p);
}

static int ReadNMax(DEBUG_MENU **dbgmenu_tbl, int now_tree)
{
    DEBUG_MENU *parent;
    DEBUG_SUB_MENU *select;

    parent = dbgmenu_tbl[dbgmenu_tbl[now_tree]->parent];
    select = &parent->submenu[parent->pos];

    return select->nmax;
}

void MakeLightEditorData(DEBUG_MENU **dbgmenu_tbl, int now_tree)
{
    int i;
    int j;
    int disp_room;
    int menu_end;
    int menu_start;
    DEBUG_SUB_MENU *psubmenu;
    SgLIGHT *plight;
    static int old_disp = -1;
    static int loff = 0;
    float cc;

    switch (now_tree)
    {
    case 18:
        for (j = 0; j < 2; j++)
        {
            disp_room = room_wrk.disp_no[0];

            if ((disp_room != 255) && (room_addr_tbl[disp_room].near_sgd != NULL))
            {
                if (old_disp == disp_room)
                {
                    return;
                }

                old_disp = disp_room;  

                dbg_wrk.light_infinite = 0;
                dbg_wrk.light_point = 0;
                dbg_wrk.light_spot = 0; 

                LoadLightData(dbgmenu_tbl, disp_room);

                ApplyLight(old_disp);

                dbgmenu_tbl[19]->title = room_name[disp_room];
                break; 
            }
        } 

    break;
    case 20:
        loff = 0;
    break;
    case 21:
        menu_end = ReadNMax(dbgmenu_tbl, now_tree) * 9;

        menu_start = menu_end + 9;

        if (le_plights[0].num < (menu_end + 9))
        {
            menu_start = le_plights[0].num;
        }

        for (i = menu_end; i < menu_start; i++)
        {
            psubmenu = &dbgmenu_tbl[21]->submenu[i - menu_end];

            sprintf(psubmenu->name, "Point %d", i);

            psubmenu->nmax = NULL;
            psubmenu->subnum = 4122;
        }

        menu_start = menu_start - menu_end;

        psubmenu = &dbgmenu_tbl[21]->submenu[menu_start]; 

        memcpy(psubmenu->name, "_end_", 6);

        psubmenu->nmax = 0;
        psubmenu->subnum = 0;

        loff = menu_end;
    break;
    case 22:
        menu_end = ReadNMax(dbgmenu_tbl, now_tree) * 9;

        menu_start = menu_end + 9;

        if (le_slights[0].num < (menu_end + 9))
        {
            menu_start = le_slights[0].num;
        }

        for (i = menu_end; i < menu_start; i++)
        {
            psubmenu = &dbgmenu_tbl[22]->submenu[i - menu_end];

            sprintf(psubmenu->name, "Spot %d", i);

            psubmenu->nmax = 0;
            psubmenu->subnum = 0x101B;
        }

        menu_start = menu_start - menu_end;

        psubmenu = &dbgmenu_tbl[22]->submenu[menu_start];

        memcpy(psubmenu->name, "_end_", 6);

        psubmenu->nmax = 0; 
        psubmenu->subnum = 0;

        loff = menu_end;
    break;
    case 23:
        SaveLightData(old_disp);
    break;
    case 24:
        LoadLightData(dbgmenu_tbl, old_disp);
        ApplyLight(old_disp);
    break;
    case 25:
        plight = &le_lights[dbg_wrk.light_infinite + loff];

        dbg_wrk.le_R = (plight->diffuse[0] * 100.0f) + 0.0001f;
        dbg_wrk.le_G = (plight->diffuse[1] * 100.0f) + 0.0001f;
        dbg_wrk.le_B = (plight->diffuse[2] * 100.0f) + 0.0001f; 

        sprintf(dbgmenu_tbl[25]->title, "Infinite %d", dbg_wrk.light_infinite + loff);
    break;
    case 26:
        plight = &le_plights[dbg_wrk.light_point + loff];

        dbg_wrk.le_R = (plight->diffuse[0] * 100.0f) + 0.0001f;
        dbg_wrk.le_G = (plight->diffuse[1] * 100.0f) + 0.0001f;
        dbg_wrk.le_B = (plight->diffuse[2] * 100.0f) + 0.0001f;  

        dbg_wrk.le_power = (plight->power / plight->ambient[0]) + 0.0001f;  
        dbg_wrk.le_plen = ((plight->intens * 10.0f) / plight->ambient[0]) + 0.0001f; 

        sprintf(dbgmenu_tbl[26]->title, "Point %d", dbg_wrk.light_point + loff);
    break;
    case 27:
        plight = &le_slights[dbg_wrk.light_spot + loff];

        dbg_wrk.le_R = (plight->diffuse[0] * 100.0f) + 0.0001f;
        dbg_wrk.le_G = (plight->diffuse[1] * 100.0f) + 0.0001f;
        dbg_wrk.le_B = (plight->diffuse[2] * 100.0f) + 0.0001f;

        dbg_wrk.le_power = (plight->power / plight->ambient[0]) + 0.0001f;
        dbg_wrk.le_cone = plight->ambient[1] + 0.0001f;

        sprintf(dbgmenu_tbl[27]->title, "Spot %d", dbg_wrk.light_spot + loff);  
    break;
    case 49:
        dbg_wrk.le_AR = (le_ambient[0] * 100.0f) + 0.0001f;
        dbg_wrk.le_AG = (le_ambient[1] * 100.0f) + 0.0001f;
        dbg_wrk.le_AB = (le_ambient[2] * 100.0f) + 0.0001f;
    break;
    case 31:
        plight = &le_lights[dbg_wrk.light_infinite + loff];

        plight->diffuse[0] = dbg_wrk.le_R * 0.01f;
        plight->diffuse[1] = dbg_wrk.le_G * 0.01f;
        plight->diffuse[2] = dbg_wrk.le_B * 0.01f;

        Vu0CopyVector(plight->specular, plight->diffuse);

        ApplyLight(old_disp);
    break;
    case 32:
        plight = &le_plights[dbg_wrk.light_point + loff];

        plight->diffuse[0] = dbg_wrk.le_R * 0.01f;
        plight->diffuse[1] = dbg_wrk.le_G * 0.01f;
        plight->diffuse[2] = dbg_wrk.le_B * 0.01f;

        Vu0CopyVector(plight->specular, plight->diffuse);

        plight->power = dbg_wrk.le_power * plight->ambient[0];

        plight->intens = dbg_wrk.le_plen * plight->ambient[0] * 0.1f;

        ApplyLight(old_disp); 
    break;
    case 33:
        plight = &le_slights[dbg_wrk.light_spot + loff];

        plight->diffuse[0] = dbg_wrk.le_R * 0.01f;
        plight->diffuse[1] = dbg_wrk.le_G * 0.01f;
        plight->diffuse[2] = dbg_wrk.le_B * 0.01f;

        Vu0CopyVector(plight->specular, plight->diffuse);

        plight->ambient[1] = dbg_wrk.le_cone;

        plight->power = dbg_wrk.le_power * plight->ambient[0];

        cc = SgCosf(dbg_wrk.le_cone * PI / 180.0f);

        plight->intens = cc * cc;

        ApplyLight(old_disp);
    break;
    case 50:
        le_ambient[0] = dbg_wrk.le_AR * 0.01f;
        le_ambient[1] = dbg_wrk.le_AG * 0.01f;
        le_ambient[2] = dbg_wrk.le_AB * 0.01f;

        ApplyLight(old_disp);
    break;
    }
}

void MakeFogData(DEBUG_SUB_MENU **dbgmenu_tbl, int pos)
{
    int disp_room;

    disp_room = (u_int)room_wrk.disp_no[0];
    
    if (pos == 3)
    {
        dbg_wrk.fog_min = fog_param[disp_room][0];
        dbg_wrk.fog_max = fog_param[disp_room][1];
        dbg_wrk.fog_near = (fog_param[disp_room][2] / 100.0f);
        dbg_wrk.fog_far = (fog_param[disp_room][3] / 100.0f);
        fog_mode = 0;
        dbg_wrk.fog_r = fog_rgb[disp_room][0];
        dbg_wrk.fog_g = fog_rgb[disp_room][1];
        dbg_wrk.fog_b = fog_rgb[disp_room][2];
    }
    else 
    {
        dbg_wrk.fog_min = fog_param_finder[disp_room][0]; 
        dbg_wrk.fog_max = fog_param_finder[disp_room][1];
        dbg_wrk.fog_near = (fog_param_finder[disp_room][2] / 100.0f);
        dbg_wrk.fog_far = (fog_param_finder[disp_room][3] / 100.0f);
        fog_mode = 1;
        dbg_wrk.fog_r = fog_rgb_finder[disp_room][0]; 
        dbg_wrk.fog_g = fog_rgb_finder[disp_room][1]; 
        dbg_wrk.fog_b = fog_rgb_finder[disp_room][2]; 
    }
}

void ApplyFogData()
{
    int disp_room;

    disp_room = (u_int)room_wrk.disp_no[0];
    
    if (fog_mode == 0)
    {
        fog_param[disp_room][0] = dbg_wrk.fog_min;
        fog_param[disp_room][1] = dbg_wrk.fog_max;
        fog_param[disp_room][2] = dbg_wrk.fog_near * 100.0f;
        fog_param[disp_room][3] = dbg_wrk.fog_far * 100.0f;
        fog_rgb[disp_room][0] = dbg_wrk.fog_r;
        fog_rgb[disp_room][1] = dbg_wrk.fog_g;
        fog_rgb[disp_room][2] = dbg_wrk.fog_b;
    }
    else
    {
        fog_param_finder[disp_room][0] = dbg_wrk.fog_min;
        fog_param_finder[disp_room][1] = dbg_wrk.fog_max;
        fog_param_finder[disp_room][2] = dbg_wrk.fog_near * 100.0f;
        fog_param_finder[disp_room][3] = dbg_wrk.fog_far * 100.0f;
        fog_rgb_finder[disp_room][0] = dbg_wrk.fog_r;
        fog_rgb_finder[disp_room][1] = dbg_wrk.fog_g;
        fog_rgb_finder[disp_room][2] = dbg_wrk.fog_b;
    }
}
