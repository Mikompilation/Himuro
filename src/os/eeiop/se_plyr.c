#include "common.h"
#include "typedefs.h"
#include "addresses.h"
#include "enums.h"
#include "se_plyr.h"

#include "ingame/map/map_ctrl.h"
#include "main/glob.h"
#include "os/eeiop/eese.h"
#include "os/eeiop/sd_room.h"
#include "os/eeiop/se_foot.h"

#include "data/rand_ita_tbl.h" // SE_FOOT_RAND_ITA rand_ita_tbl[];

int SeReqFootStep(sceVu0FVECTOR pos)
{
    u_int *addr;
    int i;
    int rd0;
    int rd1;
    int foot_rand_per[3];
    int foot_rand;
    u_short px;
    u_short pz;
    u_short call_no;
    u_char room_no;
    u_char se_no;
    u_char dscv_flg;
    u_short frand_tbl[2][3] = {
        { 80, 15, 5 },
        { 90, 10, 0 },
    };

    px = pos[0];
    pz = pos[2];

    room_no = GetDataRoom(8, plyr_wrk.pr_info.room_no);
    call_no = 0;

    for (i = 0; i < room_wrk.se_num; i++)
    {
        if (PosInAreaJudge1(8, room_no, i, pz, px) != 0)
        {
            addr = (u_int *)(map_wrk.dat_adr + 32);
            addr = (u_int *)(addr[0] + LOAD_ADDRESS_00);
            addr = &addr[room_no] + 1;
            addr = (u_int *)(addr[0] + LOAD_ADDRESS_00);
            addr = &addr[i] + 1;
            addr = (u_int *)(addr[0] + LOAD_ADDRESS_00);

            call_no = ((u_short *)(addr[0] + LOAD_ADDRESS_00))[0];

            break;
        }
    }

    if (room_wrk.se_num <= i)
    {
        return -1;
    }

    rand();

    rd0 = ((115 - rand() % 30) * 0x1000) / 100;

    if (rand() % 3 == 0)
    {
        rd1 = ((110 - rand() % 25) * 0x1000) / 100;
    }
    else
    {
        rd1 = 0x1000;
    }

    if ((plyr_wrk.mvsta & 0x1) == 0)
    {
        rd0 *= 0.55f;
    }

    if (foot_se_index[call_no] == SAF008_ITANOMA1_BD)
    {
        dscv_flg = 0;

        for (i = 0; i < 8; i++)
        {
            if (se_ctrl.foot_no[i] == SAF005_ITA_K2_BD)
            {
                dscv_flg = 1;

                break;
            }
        }

        if (dscv_flg != 0)
        {
            foot_rand = rand() % (frand_tbl[0][0] + frand_tbl[0][1] + frand_tbl[0][2]);

            if (frand_tbl[0][0] > foot_rand)
            {
                SFootGetSeNo(call_no);
            }
            else if (frand_tbl[0][0] + frand_tbl[0][1] > foot_rand)
            {
                for (i = 0; i < 8; i++)
                {
                    if (0) break; // debug code?
                }
            }
            else
            {
                for (i = 0; i < 8; i++)
                {
                    if (0) break; // debug code?
                }
            }
        }
        else
        {
            foot_rand = rand() % (frand_tbl[1][0] + frand_tbl[1][1]);

            if (frand_tbl[0][0] > foot_rand)
            {
                SFootGetSeNo(call_no);
            }
            else
            {
                for (i = 0; i < 8; i++)
                {
                    if (0) break; // debug code?
                }
            }
        }
    }

    se_no = SFootGetSeNo(call_no);

    if (se_no == 0xff)
    {
        return 0xff;
    }

    return SeStartPut(se_no, pos, 0, rd0, rd1, 0xff);
}

u_char IsKarehaZone(sceVu0FVECTOR pos)
{
    u_int *addr;
    int i;
    u_short px;
    u_short pz;
    u_short call_no;
    u_char room_no;

    pz = pos[0];
    px = pos[2];

    room_no = GetDataRoom(8, plyr_wrk.pr_info.room_no);
    call_no = 0;

    for (i = 0; i < room_wrk.se_num; i++)
    {
        if (PosInAreaJudge1(8, room_no, i, px, pz) != 0)
        {
            addr = (u_int *)(map_wrk.dat_adr + 32);
            addr = (u_int *)(addr[0] + LOAD_ADDRESS_00);
            addr = &addr[room_no] + 1;
            addr = (u_int *)(addr[0] + LOAD_ADDRESS_00);
            addr = &addr[i] + 1;
            addr = (u_int *)(addr[0] + LOAD_ADDRESS_00);

            call_no = ((u_short *)(addr[0] + LOAD_ADDRESS_00))[0];

            break;
        }
    }

    return foot_se_index[call_no] == SAF016_OCHIBA_BD;
}

u_char IsTakenohaZone(sceVu0FVECTOR pos)
{
    u_int *addr;
    int i;
    u_short px;
    u_short pz;
    u_short call_no;
    u_char room_no;

    pz = pos[0];
    px = pos[2];

    room_no = GetDataRoom(8, plyr_wrk.pr_info.room_no);
    call_no = 0;

    for (i = 0; i < room_wrk.se_num; i++)
    {
        if (PosInAreaJudge1(8, room_no, i, px, pz))
        {
            addr = (u_int *)(map_wrk.dat_adr + 32);
            addr = (u_int *)(addr[0] + LOAD_ADDRESS_00);
            addr = &addr[room_no] + 1;
            addr = (u_int *)(addr[0] + LOAD_ADDRESS_00);
            addr = &addr[i] + 1;
            addr = (u_int *)(addr[0] + LOAD_ADDRESS_00);

            call_no = ((u_short *)(addr[0] + LOAD_ADDRESS_00))[0];

            break;
        }
    }

    return foot_se_index[call_no] == SAF022_DOTAKE_BD;
}

u_char IsHokoriZone()
{
    return plyr_wrk.pr_info.room_no == 9;
}
