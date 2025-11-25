#ifndef IOPSE_H_
#define IOPSE_H_

#include "iopmain.h"

enum GHOST_SE_ID {
    SG000_HENREI = 0,
    SG001_JYOREI = 1,
    SG002_SAKKA = 2,
    SG003_DUMMY = 3,
    SG004_MAYOIGO1 = 4,
    SG005_DUMMY = 5,
    SG006_DUMMY = 6,
    SG007_DUMMY = 7,
    SG008_DUMMY = 8,
    SG009_KOMUSO = 9,
    SG010_DUMMY = 10,
    SG011_DUMMY = 11,
    SG012_DUMMY = 12,
    SG013_DUMMY = 13,
    SG014_DUMMY = 14,
    SG015_DUMMY = 15,
    SG016_DUMMY = 16,
    SG017_DUMMY = 17,
    SG018_TENAGA = 18,
    SG019_DUMMY = 19,
    SG020_KAMIONNA = 20,
    SG021_KUBI = 21,
    SG022_DUMMY = 22,
    SG023_DUMMY = 23,
    SG024_DUMMY = 24,
    SG025_DUMMY = 25,
    SG026_DUMMY = 26,
    SE_GHOST_ID_MAX = 27
};

enum {
    SE_ADDRNO_STATIC = 0,
    SE_ADDRNO_BTLHIT = 1,
    SE_ADDRNO_VOICE = 2,
    SE_ADDRNO_DOOR0 = 3,
    SE_ADDRNO_DOOR1 = 4,
    SE_ADDRNO_DOOR2 = 5,
    SE_ADDRNO_FOOT0 = 6,
    SE_ADDRNO_FOOT1 = 7,
    SE_ADDRNO_FOOT2 = 8,
    SE_ADDRNO_FOOT3 = 9,
    SE_ADDRNO_FOOT4 = 10,
    SE_ADDRNO_FOOT5 = 11,
    SE_ADDRNO_FOOT6 = 12,
    SE_ADDRNO_FOOT7 = 13,
    SE_ADDRNO_SRUND0 = 14,
    SE_ADDRNO_SRUND1 = 15,
    SE_ADDRNO_GHOST0 = 16,
    SE_ADDRNO_GHOST1 = 17,
    SE_ADDRNO_GHOST2 = 18,
    SE_ADDRNO_EVENT0 = 19,
    SE_ADDRNO_EVENT1 = 20,
    SE_ADDRNO_WIDE = 21,
    SE_ADDRNO_JIDOU0 = 22,
    SE_ADDRNO_JIDOU1 = 23,
    SE_ADDRNO_JIDOU2 = 24,
    SE_ADDRNO_JIDOU3 = 25,
    SE_ADDRNO_SB0 = 26,
    SE_ADDRNO_ADPCM0 = 27,
    SE_ADDRNO_SB1 = 28,
    SE_ADDRNO_ADPCM1 = 29,
    SE_CSR0 = 0,
    SE_CLIC = 1,
    SE_CLIC_NO = 2,
    SE_CANCEL = 3,
    SE_CSR1 = 4,
    SE_SEALING = 5,
    SE_USEITEM = 6,
    SE_SOUL_APR = 7,
    SE_SOUL_LOOP = 8,
    SE_CAMSUB_00 = 9,
    SE_CAMSUB_01 = 12,
    SE_CAMSUB_02 = 13,
    SE_CAMSUB_03 = 24,
    SE_COME_OUT = 10,
    SE_GETITEM = 11,
    SE_STAMINA_UP = 14,
    SE_TERROR = 15,
    SE_PHOTO = 16,
    SE_DRAIN = 17,
    SE_CHARGE = 18,
    SE_FLASH_CHARGE = 255,
    SE_PUNT = 19,
    SE_SHUTTER = 20,
    SE_SPOUT_LOW = 21,
    SE_SPOUT_MDL = 22,
    SE_SPOUT_HI = 23,
    SE_RARE0 = 27,
    SE_RARE1 = 28,
    SE_ENE_PURIFY = 29,
    SE_ENE0_GRASP = 30,
    SE_ENE0_WARP = 31,
    SE_ENE0_HIT = 32,
    SE_TOUSHU_KIRI = 32,
    SE_TOUSHU_KAZEKIRI = 31,
    SE_PLYR_DMG0 = 33,
    SE_PLYR_DMG1 = 34,
    SE_PLYR_DMG2 = 35,
    SE_PLYR_DEADLY = 36,
    SE_PLYR_DEADLY2 = 255,
    SE_PUZZLE0 = 33,
    SE_PUZZLE1 = 34,
    SE_PUZZLE2 = 35,
    SE_PUZZLE3 = 36,
    SE_PUZZLE4 = 37,
    SE_MN0 = 33,
    SE_MN1 = 34,
    SE_MN2 = 35,
    SE_MN3 = 36,
    SE_MN4 = 37,
    SE_DOOR_OPN_00 = 38,
    SE_DOOR_CLS_00 = 39,
    SE_DOOR_OPN_01 = 40,
    SE_DOOR_CLS_01 = 41,
    SE_DOOR_OPN_02 = 42,
    SE_DOOR_CLS_02 = 43,
    SE_FOOT0 = 44,
    SE_FOOT1 = 45,
    SE_FOOT2 = 46,
    SE_FOOT3 = 47,
    SE_FOOT4 = 48,
    SE_FOOT5 = 49,
    SE_FOOT6 = 50,
    SE_FOOT7 = 51,
    SE_SURROUND0 = 52,
    SE_SURROUND1 = 53,
    SE_ENE0_OMEN = 54,
    SE_ENE0_APPROACH = 55,
    SE_ENE0_FLINCH_LOW = 56,
    SE_ENE0_FLINCH_MDL = 57,
    SE_ENE0_FLINCH_HI = 58,
    SE_ENE0_ATACK = 59,
    SE_ENE0_DEATH = 60,
    SE_ENE0_SPE0 = 61,
    SE_ENE0_SPE1 = 62,
    SE_ENE0_SPE2 = 63,
    SE_ENE0_SPE3 = 64,
    SE_ENE1_OMEN = 65,
    SE_ENE1_APPROACH = 66,
    SE_ENE1_FLINCH_LOW = 67,
    SE_ENE1_FLINCH_MDL = 68,
    SE_ENE1_FLINCH_HI = 69,
    SE_ENE1_ATACK = 70,
    SE_ENE1_DEATH = 71,
    SE_ENE1_SPE0 = 72,
    SE_ENE1_SPE1 = 73,
    SE_ENE1_SPE2 = 74,
    SE_ENE1_SPE3 = 75,
    SE_ENE2_OMEN = 76,
    SE_ENE2_APPROACH = 77,
    SE_ENE2_FLINCH_LOW = 78,
    SE_ENE2_FLINCH_MDL = 79,
    SE_ENE2_FLINCH_HI = 80,
    SE_ENE2_ATACK = 81,
    SE_ENE2_DEATH = 82,
    SE_ENE2_SPE0 = 83,
    SE_ENE2_SPE1 = 84,
    SE_ENE2_SPE2 = 85,
    SE_ENE2_SPE3 = 86,
    SE_EVENT_00 = 87,
    SE_EVENT_01 = 88,
    SE_EVENT_02 = 89,
    SE_EVENT_10 = 90,
    SE_EVENT_11 = 91,
    SE_EVENT_12 = 92,
    SE_HANYOU = 93,
    SE_ENE_JIDOU0 = 94,
    SE_ENE_JIDOU1 = 95,
    SE_ENE_JIDOU2 = 96,
    SE_ENE_JIDOU3 = 97,
    SE_NO = 255,
    SE_TMP_KOTO0 = 81,
    SE_TMP_KOTO1 = 82,
    SE_TMP_JREI001 = 255,
    SE_TMP_JREI104 = 255,
    SE_TMP_JREI109 = 255,
    SE_TMP_JREI110 = 255,
    SE_ENE0_FLINCH = 51,
    SE_DOOR_SP_OPN = 31,
    SE_DOOR_SP_CLS = 32,
    SE_DOOR_LG_LCK = 33,
    SE_UNLOCK_MD = 36,
    SE_DOOR_LG_OPN = 26,
    SE_DOOR_LG_CLS = 27,
    SE_DOOR_MD_OPN = 28,
    SE_DOOR_MD_CLS = 29,
    SE_FUSUMA_OPN = 30
};

enum CDVD_SE_TRANS_STATUS {
    CDVD_SE_NOCMD = 0,
    CDVD_SE_WAIT = 1,
    CDVD_SE_FINISHED = 2
};

typedef struct { // 0x10
    /* 0x0 */ unsigned int adrs;
    /* 0x4 */ short unsigned int attribute;
    /* 0x6 */ short unsigned int pitch;
    /* 0x8 */ short unsigned int adsr1;
    /* 0xa */ short unsigned int adsr2;
    /* 0xc */ char efct;
    /* 0xd */ char vol;
    /* 0xe */ char pan;
} SE_PARAM;

typedef struct { // 0x20
    /* 0x00 */ SE_PARAM* param;
    /* 0x00 */ unsigned int attribute;
    /* 0x08 */ short int v_no;
    /* 0x0a */ short int prm_no;
    /* 0x0c */ short unsigned int pitch;
    /* 0x0e */ short unsigned int vol_l;
    /* 0x10 */ short unsigned int vol_r;
    /* 0x12 */ short unsigned int adsr1;
    /* 0x14 */ short unsigned int adsr2;
    /* 0x16 */ short unsigned int tgt_vol_l;
    /* 0x18 */ short unsigned int tgt_vol_r;
    /* 0x1a */ char inc_vol_l;
    /* 0x1b */ char inc_vol_r;
    /* 0x1c */ unsigned char buf_no;
} SE_WRK_SET;

typedef struct { // 0x32
    /* 0x00 */ short int btlhit;
    /* 0x02 */ short int voice;
    /* 0x04 */ short int door[3];
    /* 0x0a */ short int foot[8];
    /* 0x1a */ short int srund[2];
    /* 0x1e */ short int ghost[3];
    /* 0x24 */ short int event[2];
    /* 0x28 */ short int wide;
    /* 0x2a */ short int jidou[4];
} SE_START_POINT;

enum SE_VOICE_STAT {
    VOICE_FREE = 0,
    VOICE_USE = 1,
    VOICE_LOOP = 2,
    VOICE_RESERVED = 3
};

extern u_int se_start_flg;
extern u_int se_stop_flg;

extern u_int snd_buf_top[];

void ISeInit(int mode);
void ISeCmd(IOP_COMMAND* icp);
void ISeMain();

u_int SeGetSndBufTop(int pos);
void SeSetStartPoint(u_char type, u_int no);
int ICdvdTransSeEnd();
void SeSetMasterVol(u_short mvol);

SE_PARAM* SeGetSeParamTbl(int num);
int GetSeAdrs(int se_no);

#endif // IOPSE_H_
