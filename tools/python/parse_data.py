# pyright: reportInvalidTypeForm=false

import os
import re
import io
import enum
import math
import tqdm
import argparse
import pydantic
import subprocess

from ctypes import c_uint16, c_int16, c_uint64, c_uint32, c_int32, c_uint8, c_float, sizeof as c_sizeof
from typing import Protocol, BinaryIO, Type, Iterable, cast
from pathlib import Path

from cstruct import CStructure, ctypes_types, CTypeType, c_addr_ptr, c_str, print_carr


def vram2offset(vram: int):
    return vram - 0xFF000


sceVu0FVECTOR = c_float * 4
sceVu0IVECTOR = c_int32 * 4

# CTypeTypeEX = CTypeType | type[sceVu0FVECTOR] | type[sceVu0IVECTOR]


# struct SPRT_DAT { // size:0x20
#     u_long tex0;
#     u_short u;
#     u_short v;
#     u_short w;
#     u_short h;
#     int x;
#     int y;
#     int pri;
#     u_char alpha;
# };
class SPRT_DAT(CStructure):
    tex0: c_uint64
    u: c_uint16
    v: c_uint16
    w: c_uint16
    h: c_uint16
    x: c_int32
    y: c_int32
    pri: c_int32
    alpha: c_uint8


# typedef struct { // 0x8
#     int file_no;
#     u_short vol;
# } ADPCM_VOL;
class ADPCM_VOL(CStructure):
    file_no: c_int32
    vol: c_uint16


# typedef struct { // 0x4
#     u_short se_no0;
#     u_short se_no1;
# } SE_STE;
class SE_STE(CStructure):
    se_no0: c_uint16
    se_no1: c_uint16


# typedef struct { // 0x8
#     int file_no;
#     u_char fg_no;
# } FG_FILE_TBL;
class FG_FILE_TBL(CStructure):
    file_no: c_int32
    fg_no: c_uint8


# typedef struct {
# 	int file_no;
# 	u_short vol;
# } ADPCM_ROOM_PLAY;
class ADPCM_ROOM_PLAY(CStructure):
    file_no: c_int32
    vol: c_uint16


# typedef struct {
# 	u_short rvol;
# 	u_char door_no;
# 	u_char foot_no;
# 	u_char srund_no;
# 	u_char event_no;
# 	u_char adpcm[5];
# } ROOM_SOUND_INFO;
class ROOM_SOUND_INFO(CStructure):
    rvol: c_uint16
    door_no: c_uint8
    foot_no: c_uint8
    srund_no: c_uint8
    event_no: c_uint8
    adpcm: c_uint8 * 5


# typedef struct {
#   u_int fno[3];
# } ROOM_DOOR_SE;
class ROOM_DOOR_SE(CStructure):
    fno: c_uint32 * 3


# typedef struct {
#   u_int fno[5];
# } ROOM_FOOT_SE;
class ROOM_FOOT_SE(CStructure):
    fno: c_uint32 * 5


# typedef struct {
#   u_char num;
#   u_char rate[4];
# } SE_FOOT_RAND_ITA;
class SE_FOOT_RAND_ITA(CStructure):
    num: c_uint8
    rate: c_uint8 * 4


# typedef struct { // 0x40
# 	/* 0x00 */ sceVu0FVECTOR fall_speed;
# 	/* 0x10 */ sceVu0IVECTOR wind;
# 	/* 0x20 */ sceVu0FVECTOR rotate;
# 	/* 0x30 */ int stop_time;
# 	/* 0x34 */ short int r;
# 	/* 0x36 */ short int g;
# 	/* 0x38 */ short int b;
# 	/* 0x3a */ short int a;
# } FALL_TABLE;
class FALL_TABLE(CStructure, align=64):
    fall_speed: sceVu0FVECTOR
    wind: sceVu0IVECTOR
    rotate: sceVu0FVECTOR
    stop_time: c_int32
    r: c_int16
    g: c_int16
    b: c_int16
    a: c_int16


# typedef struct { // 0xe
# 	/* 0x0 */ short int u;
# 	/* 0x2 */ short int v;
# 	/* 0x4 */ short int w;
# 	/* 0x6 */ short int h;
# 	/* 0x8 */ short int x;
# 	/* 0xa */ short int y;
# 	/* 0xc */ u_char pri;
# 	/* 0xd */ u_char alp;
# } SPRT_SDAT;
class SPRT_SDAT(CStructure):
    u: c_int16
    v: c_int16
    w: c_int16
    h: c_int16
    x: c_int16
    y: c_int16
    pri: c_uint8
    alp: c_uint8


# typedef struct { // 0x8
# 	/* 0x0 */ short int cx;
# 	/* 0x2 */ short int cy;
# 	/* 0x4 */ float rot;
# } SPRT_SROT;
class SPRT_SROT(CStructure):
    cx: c_int16
    cy: c_int16
    rot: c_float


# typedef struct { // 0x8
# 	/* 0x0 */ short int cx;
# 	/* 0x2 */ short int cy;
# 	/* 0x4 */ short int sw;
# 	/* 0x6 */ short int sh;
# } SPRT_SSCL;
class SPRT_SSCL(CStructure):
    cx: c_int16
    cy: c_int16
    sw: c_int16
    sh: c_int16


# typedef struct { // 0x14
#     /* 0x00 */ short int x_trans;
#     /* 0x02 */ short int y_trans;
#     /* 0x04 */ short int x_scale;
#     /* 0x06 */ short int y_scale;
#     /* 0x08 */ short int rotation;
#     /* 0x0a */ short int alpha;
#     /* 0x0c */ short int counter;
#     /* 0x10 */ u_int attribute;
# } ANM2D_DAT_TABLE;
class ANM2D_DAT_TABLE(CStructure):
    x_trans: c_int16
    y_trans: c_int16
    x_scale: c_int16
    y_scale: c_int16
    rotation: c_int16
    alpha: c_int16
    counter: c_int16
    attribute: c_uint32


# typedef struct { // 0x110
#     /* 0x000 */ int anm_index;
#     /* 0x004 */ SPRT_SDAT *sdat_p;
#     /* 0x008 */ SPRT_SDAT sdat[8];
#     /* 0x078 */ SPRT_SROT srot[8];
#     /* 0x0b8 */ SPRT_SSCL sscl[8];
#     /* 0x0f8 */ int sp_no;
#     /* 0x0fc */ void *table_p;
#     /* 0x100 */ short int x_speed;
#     /* 0x102 */ short int y_speed;
#     /* 0x104 */ short int xscal_off;
#     /* 0x106 */ short int yscal_off;
#     /* 0x108 */ short int rot_off;
#     /* 0x10a */ short int alpha_off;
#     /* 0x10c */ short int counter;
#     /* 0x10e */ short int pad;
# } ANM2D_WRK_TABLE;
class ANM2D_WRK_TABLE(CStructure):
    anm_index: c_int32
    sdat_p: c_addr_ptr  # SPRT_SDAT
    sdat: SPRT_SDAT * 8
    srot: SPRT_SROT * 8
    sscl: SPRT_SSCL * 8
    sp_no: c_int32
    table_p: c_addr_ptr
    x_speed: c_int16
    y_speed: c_int16
    xscal_off: c_int16
    yscal_off: c_int16
    rot_off: c_int16
    alpha_off: c_int16
    counter: c_int16
    pad: c_int16


# typedef struct { // 0x17b0
# 	/* 0x0000 */ sceVu0FVECTOR mpos_keep;
# 	/* 0x0010 */ void *pos_p;
# 	/* 0x0014 */ short int at_ground[600];
# 	/* 0x04c4 */ short int rgba[600][4];
# 	/* 0x1784 */ int fnum_keep;
# 	/* 0x1788 */ int fall_count;
# 	/* 0x178c */ float fall_rate;
# 	/* 0x1790 */ float rate_remain;
# 	/* 0x1794 */ short int mode_keep;
# 	/* 0x1796 */ short int area_keep;
# 	/* 0x1798 */ short int dist;
# 	/* 0x179a */ short int init_flg;
# 	/* 0x179c */ short int offs[3];
# } LEAVES_TABLE;
class LEAVES_TABLE(CStructure):
    mpos_keep: sceVu0FVECTOR
    pos_p: c_addr_ptr
    at_ground: c_int16 * 600
    rgba: c_int16 * 4 * 600
    fnum_keep: c_int32
    fall_count: c_int32
    fall_rate: c_float
    rate_remain: c_float
    mode_keep: c_int16
    area_keep: c_int16
    dist: c_int16
    init_flg: c_int16
    offs: c_int16 * 3


# typedef struct { // 0x10
# 	/* 0x0 */ u_long tex_keep;
# 	/* 0x8 */ short int tm2_id;
# 	/* 0xa */ short int mode;
# 	/* 0xc */ short int load_id;
# 	/* 0xe */ short int pad;
# } BTL_ANM_LOAD;
class BTL_ANM_LOAD(CStructure):
    tex_keep: c_uint64
    tm2_id: c_int16
    mode: c_int16
    load_id: c_int16
    pad: c_int16


# typedef struct { // 0x60
# 	/* 0x00 */ sceGsTex0 g_GsTex0;
# 	/* 0x08 */ u_int g_nTexSizeW;
# 	/* 0x0c */ u_int g_nTexSizeH;
# 	/* 0x10 */ u_char g_bMipmapLv;
# 	/* 0x18 */ long unsigned int g_GsMiptbp1;
# 	/* 0x20 */ long unsigned int g_GsMiptbp2;
# 	/* 0x28 */ float pos_x;
# 	/* 0x2c */ float pos_y;
# 	/* 0x30 */ int pos_z;
# 	/* 0x34 */ float size_w;
# 	/* 0x38 */ float size_h;
# 	/* 0x3c */ float scale_w;
# 	/* 0x40 */ float scale_h;
# 	/* 0x44 */ int clamp_u;
# 	/* 0x48 */ int clamp_v;
# 	/* 0x4c */ u_int rot_center;
# 	/* 0x50 */ float angle;
# 	/* 0x54 */ u_char r;
# 	/* 0x55 */ u_char g;
# 	/* 0x56 */ u_char b;
# 	/* 0x57 */ u_char alpha;
# 	/* 0x58 */ u_int mask;
# } SPRITE_DATA;
class SPRITE_DATA(CStructure):
    g_GsTex0: c_uint64  # sceGsTex0
    g_nTexSizeW: c_uint32
    g_nTexSizeH: c_uint32
    g_bMipmapLv: c_uint8
    g_GsMiptbp1: c_uint64
    g_GsMiptbp2: c_uint64
    pos_x: c_float
    pos_y: c_float
    pos_z: c_int32
    size_w: c_float
    size_h: c_float
    scale_w: c_float
    scale_h: c_float
    clamp_u: c_int32
    clamp_v: c_int32
    rot_center: c_uint32
    angle: c_float
    r: c_uint8
    g: c_uint8
    b: c_uint8
    alpha: c_uint8
    mask: c_uint32


# typedef struct { // 0x30
# 	/* 0x00 */ u_long tex1;
# 	/* 0x08 */ u_long alpha;
# 	/* 0x10 */ u_long zbuf;
# 	/* 0x18 */ u_long test;
# 	/* 0x20 */ u_long clamp;
# 	/* 0x28 */ u_long prim;
# } DRAW_ENV;
class DRAW_ENV(CStructure):
    tex1: c_uint64
    alpha: c_uint64
    zbuf: c_uint64
    test: c_uint64
    clamp: c_uint64
    prim: c_uint64


###########################################################################


# typedef struct { // 0x14
# 	/* 0x00 */ u_char sw;
# 	/* 0x01 */ u_char alp;
# 	/* 0x04 */ int scl;
# 	/* 0x08 */ int rot;
# 	/* 0x0c */ float x;
# 	/* 0x10 */ float y;
# } BLUR_STR;
class BLUR_STR(CStructure):
    sw: c_uint8
    alp: c_uint8
    scl: c_int32
    rot: c_int32
    x: c_float
    y: c_float


# typedef struct { // 0x4
# 	/* 0x0 */ u_char sw;
# 	/* 0x1 */ u_char type;
# 	/* 0x2 */ u_char col;
# 	/* 0x3 */ u_char alp;
# } CONTRAST_STR;
class CONTRAST_STR(CStructure):
    sw: c_uint8
    type: c_uint8
    col: c_uint8
    alp: c_uint8


# typedef struct { // 0x2
# 	/* 0x0 */ u_char sw;
# 	/* 0x1 */ u_char alp;
# } FFRAME_STR;
class FFRAME_STR(CStructure):
    sw: c_uint8
    alp: c_uint8


# typedef struct { // 0x10
# 	/* 0x0 */ u_char sw;
# 	/* 0x1 */ u_char type;
# 	/* 0x4 */ float spd;
# 	/* 0x8 */ float alp;
# 	/* 0xc */ u_char amax;
# 	/* 0xd */ u_char cmax;
# } DITHER_STR;
class DITHER_STR(CStructure):
    sw: c_uint8
    type: c_uint8
    spd: c_float
    alp: c_float
    amax: c_uint8
    cmax: c_uint8


# typedef struct { // 0x3
# 	/* 0x0 */ u_char sw;
# 	/* 0x1 */ u_char type;
# 	/* 0x2 */ u_char rate;
# } DEFORM_STR;
class DEFORM_STR(CStructure):
    sw: c_uint8
    type: c_uint8
    rate: c_uint8


# typedef struct { // 0x4
# 	/* 0x0 */ u_char sw;
# 	/* 0x1 */ u_char col;
# 	/* 0x2 */ u_char alp;
# 	/* 0x3 */ u_char alp2;
# } NEGA_STR;
class NEGA_STR(CStructure):
    sw: c_uint8
    col: c_uint8
    alp: c_uint8
    alp2: c_uint8


# typedef struct { // 0x1
# 	/* 0x0 */ u_char sw;
# } MONO_STR;
class MONO_STR(CStructure):
    sw: c_uint8


# typedef struct { // 0x34
# 	/* 0x00 */ BLUR_STR bl;
# 	/* 0x14 */ CONTRAST_STR cn;
# 	/* 0x18 */ FFRAME_STR ff;
# 	/* 0x1c */ DITHER_STR dt;
# 	/* 0x2c */ DEFORM_STR df;
# 	/* 0x2f */ NEGA_STR ng;
# 	/* 0x33 */ MONO_STR mn;
# } SBTSET;
class SBTSET(CStructure):
    bl: BLUR_STR
    cn: CONTRAST_STR
    ff: FFRAME_STR
    dt: DITHER_STR
    df: DEFORM_STR
    ng: NEGA_STR
    mn: MONO_STR


# typedef struct { // 0x28
# 	/* 0x00 */ u_int flow;
# 	/* 0x04 */ u_int cnt;
# 	/* 0x08 */ u_int in;
# 	/* 0x0c */ u_int keep;
# 	/* 0x10 */ u_int out;
# 	/* 0x14 */ u_int alp;
# 	/* 0x18 */ float scl;
# 	/* 0x1c */ float rot;
# 	/* 0x20 */ float cx;
# 	/* 0x24 */ float cy;
# } EFF_BLUR;
class EFF_BLUR(CStructure):
    flow: c_uint32
    cnt: c_uint32
    _in: c_uint32
    keep: c_uint32
    out: c_uint32
    alp: c_uint32
    scl: c_float
    rot: c_float
    cx: c_float
    cy: c_float


# typedef struct { // 0x18
# 	/* 0x00 */ u_int flow;
# 	/* 0x04 */ u_int cnt;
# 	/* 0x08 */ u_int in;
# 	/* 0x0c */ u_int keep;
# 	/* 0x10 */ u_int out;
# 	/* 0x14 */ u_int max;
# } EFF_FOCUS;
class EFF_FOCUS(CStructure):
    flow: c_uint32
    cnt: c_uint32
    _in: c_uint32
    keep: c_uint32
    out: c_uint32
    max: c_uint32


# typedef struct { // 0x4
# 	/* 0x0 */ u_char type;
# 	/* 0x1 */ u_char otype;
# 	/* 0x2 */ u_char init;
# 	/* 0x3 */ u_char pass;
# } EFF_DEFORM;
class EFF_DEFORM(CStructure):
    type: c_uint8
    otype: c_uint8
    init: c_uint8
    _pass: c_uint8


###########################################################################


# typedef struct { // 0x10
# 	/* 0x0 */ int mesno;
# 	/* 0x4 */ u_int in;
# 	/* 0x8 */ u_int out;
# 	/* 0xc */ u_int attr;
# } SUBTITLES;
class SUBTITLES(CStructure):
    mesno: c_int32
    _in: c_uint32
    out: c_uint32
    attr: c_uint32


# typedef struct { // 0x20
# 	/* 0x00 */ int pos_x;
# 	/* 0x04 */ int pos_y;
# 	/* 0x08 */ int path;
# 	/* 0x0c */ int fr_rate;
# 	/* 0x10 */ int pri;
# 	/* 0x14 */ u_int attr;
# 	/* 0x18 */ int *tbl;
# 	/* 0x1c */ SUBTITLES **list;
# } SUBTITLES_DAT;
class SUBTITLES_DAT(CStructure):
    pos_x: c_int32
    pos_y: c_int32
    path: c_int32
    fr_rate: c_int32
    pri: c_int32
    attr: c_uint32
    tbl: c_addr_ptr
    list: c_addr_ptr


# typedef struct { // 0x10
# 	/* 0x0 */ u_int type;
# 	/* 0x4 */ u_int no;
# 	/* 0x8 */ u_int cnt;
# 	/* 0xc */ u_char alp;
# 	/* 0xd */ u_char flg;
# 	/* 0xe */ u_char run;
# 	/* 0xf */ u_char dummy;
# } SUBTITLES_SYS;
class SUBTITLES_SYS(CStructure):
    type: c_uint32
    no: c_uint32
    cnt: c_uint32
    alp: c_uint8
    flg: c_uint8
    run: c_uint8
    dummy: c_uint8


###########################################################################
# typedef struct
# { // 0xc
#     /* 0x0 */ char *name;
#     /* 0x4 */ int subnum;
#     /* 0x8 */ int nmax;
# } DEBUG_SUB_MENU;
class DEBUG_SUB_MENU(CStructure, pack=1):
    name: c_str
    subnum: c_int32
    nmax: c_int32


# typedef struct {
# 	int parent;
# 	int off_num;
# 	int mnum;
# 	int kai;
# 	int max;
# 	int pos;
# 	char *title;
# 	DEBUG_SUB_MENU submenu[10];
# } DEBUG_MENU;
class DEBUG_MENU(CStructure):
    parent: c_int32
    off_num: c_int32
    mnum: c_int32
    kai: c_int32
    max: c_int32
    pos: c_int32
    title: c_str
    submenu: DEBUG_SUB_MENU * 10


###########################################################################
# typedef struct { // 0x10
# 	/* 0x0 */ u_int type;
# 	/* 0x4 */ u_char rgba[4];
# 	/* 0x8 */ float lscl;
# 	/* 0xc */ float tscl;
# } EFRENZ;
class EFRENZ(CStructure):
    type: c_uint32
    rgba: c_uint8 * 4
    lscl: c_float
    tscl: c_float


###########################################################################
# typedef struct { // 0x50
# 	/* 0x00 */ sceVu0FVECTOR bpos;
# 	/* 0x10 */ sceVu0FVECTOR brot;
# 	/* 0x20 */ sceVu0FVECTOR lpos;
# 	/* 0x30 */ float ww;
# 	/* 0x34 */ float hh;
# 	/* 0x38 */ int power;
# 	/* 0x3c */ u_char r;
# 	/* 0x3d */ u_char g;
# 	/* 0x3e */ u_char b;
# 	/* 0x3f */ u_char a;
# 	/* 0x40 */ void *adr;
# } SUNSHINE;
class SUNSHINE(CStructure, align=16):
    bpos: sceVu0FVECTOR
    brot: sceVu0FVECTOR
    lpos: sceVu0FVECTOR
    ww: c_float
    hh: c_float
    power: c_int32
    r: c_uint8
    g: c_uint8
    b: c_uint8
    a: c_uint8
    adr: c_addr_ptr


# typedef struct {
# 	sceVu0FVECTOR ligdiff;
# 	sceVu0FVECTOR ligpos;
# 	sceVu0FVECTOR fpos;
# 	void *ebuf;
# 	float ligpow;
# 	float wavew;
# 	float rate;
# 	float szw;
# 	float szh;
# 	float sw;
# 	float sh;
# 	u_short furn_id;
# 	u_char lignum;
# 	u_char usefl;
# 	u_char sta;
# } BURN_FIRE;
class BURN_FIRE(CStructure, align=16):
    ligdiff: sceVu0FVECTOR
    ligpos: sceVu0FVECTOR
    fpos: sceVu0FVECTOR
    ebuf: c_addr_ptr
    ligpow: c_float
    wavew: c_float
    rate: c_float
    szw: c_float
    szh: c_float
    sw: c_float
    sh: c_float
    furn_id: c_uint16
    lignum: c_uint8
    usefl: c_uint8
    sta: c_uint8


# typedef struct {
# 	void *adr;
# 	short int furn_id;
# 	u_short dummy;
# } EFFRDR_RSV;
class EFFRDR_RSV(CStructure):
    adr: c_addr_ptr
    furn_id: c_int16
    dummy: c_uint16


###########################################################################
# typedef struct {
# 	u_short pk2;
# 	u_short mpk;
# 	u_short acs;
# 	u_short anm;
# 	u_short bwc;
# 	u_short clt;
# } PLYR_FILE_ID;
class PLYR_FILE_ID(CStructure):
    pk2: c_uint16
    mpk: c_uint16
    acs: c_uint16
    anm: c_uint16
    bwc: c_uint16
    clt: c_uint16


###########################################################################
# typedef struct {
# 	float rot0;
# 	float rot1;
# 	float spd;
# 	u_short timer;
# 	u_short move_type;
# } PLYR_FURI_DAT;
class PLYR_FURI_DAT(CStructure):
    rot0: c_float
    rot1: c_float
    spd: c_float
    timer: c_uint16
    move_type: c_uint16


###########################################################################
# typedef struct { // 0x8
# 	/* 0x0 */ SPRT_DAT *spr;
# 	/* 0x4 */ u_int num;
# } RARE_ENE_1DAT;
class RARE_ENE_1DAT(CStructure):
    spr: c_addr_ptr
    num: c_uint32


# typedef struct { // 0x10
# 	/* 0x0 */ RARE_ENE_1DAT *re1d;
# 	/* 0x4 */ float sclw;
# 	/* 0x8 */ float sclh;
# 	/* 0xc */ u_short attr;
# 	/* 0xe */ u_char attr_sub;
# 	/* 0xf */ u_char pat;
# } RARE_ENE_DAT;
class RARE_ENE_DAT(CStructure):
    re1d: c_addr_ptr
    sclw: c_float
    sclh: c_float
    attr: c_uint16
    attr_sub: c_uint8
    pat: c_uint8


###########################################################################
# typedef struct { // 0x40
# 	/* 0x00 */ u_int attr1;
# 	/* 0x04 */ u_short dst_gthr;
# 	/* 0x06 */ u_char way_gthr;
# 	/* 0x07 */ u_char atk_ptn;
# 	/* 0x08 */ u_char wspd;
# 	/* 0x09 */ u_char rspd;
# 	/* 0x0a */ u_short hp;
# 	/* 0x0c */ u_short atk_rng;
# 	/* 0x0e */ u_short hit_rng;
# 	/* 0x10 */ u_short chance_rng;
# 	/* 0x12 */ short int hit_adjx;
# 	/* 0x14 */ u_short atk_p;
# 	/* 0x16 */ u_short atk_h;
# 	/* 0x18 */ u_char atk;
# 	/* 0x19 */ u_char atk_tm;
# 	/* 0x1a */ u_short mdl_no;
# 	/* 0x1c */ u_short anm_no;
# 	/* 0x20 */ u_int se_no;
# 	/* 0x24 */ u_int adpcm_no;
# 	/* 0x28 */ int dead_adpcm;
# 	/* 0x2c */ u_short point_base;
# 	/* 0x2e */ u_char hint_pic;
# 	/* 0x2f */ u_char aura_alp;
# 	/* 0x30 */ u_char area[6];
# 	/* 0x36 */ u_short dir;
# 	/* 0x38 */ u_short px;
# 	/* 0x3a */ short int py;
# 	/* 0x3c */ u_short pz;
# } ENE_DAT;
class ENE_DAT(CStructure):
    attr1: c_uint32
    dst_gthr: c_uint16
    way_gthr: c_uint8
    atk_ptn: c_uint8
    wspd: c_uint8
    rspd: c_uint8
    hp: c_uint16
    atk_rng: c_uint16
    hit_rng: c_uint16
    chance_rng: c_uint16
    hit_adjx: c_int16
    atk_p: c_uint16
    atk_h: c_uint16
    atk: c_uint8
    atk_tm: c_uint8
    mdl_no: c_uint16
    anm_no: c_uint16
    se_no: c_uint32
    adpcm_no: c_uint32
    dead_adpcm: c_int32
    point_base: c_uint16
    hint_pic: c_uint8
    aura_alp: c_uint8
    area: c_uint8 * 6
    dir: c_uint16
    px: c_uint16
    py: c_int16
    pz: c_uint16


# typedef struct { // 0x20
# 	/* 0x00 */ u_char dat_no;
# 	/* 0x01 */ u_char soul_no;
# 	/* 0x02 */ u_short dir;
# 	/* 0x04 */ u_short px;
# 	/* 0x06 */ short int py;
# 	/* 0x08 */ u_short pz;
# 	/* 0x0a */ u_short adpcm_tm;
# 	/* 0x0c */ int adpcm_no;
# 	/* 0x10 */ u_short rng;
# 	/* 0x12 */ u_short mdl_no;
# 	/* 0x14 */ u_short anm_no;
# 	/* 0x16 */ u_short point_base;
# 	/* 0x18 */ u_int se_no;
# 	/* 0x1c */ int se_foot;
# } AENE_INFO_DAT;
class AENE_INFO_DAT(CStructure):
    dat_no: c_uint8
    soul_no: c_uint8
    dir: c_uint16
    px: c_uint16
    py: c_int16
    pz: c_uint16
    adpcm_tm: c_uint16
    adpcm_no: c_int32
    rng: c_uint16
    mdl_no: c_uint16
    anm_no: c_uint16
    point_base: c_uint16
    se_no: c_uint32
    se_foot: c_int32


# typedef struct { // 0x8
# 	/* 0x0 */ u_short dmg;
# 	/* 0x2 */ u_short hit_rng;
# 	/* 0x4 */ u_short mdl_no;
# 	/* 0x6 */ u_short cond;
# } FLY_DATA;
class FLY_DATA(CStructure):
    dmg: c_uint16
    hit_rng: c_uint16
    mdl_no: c_uint16
    cond: c_uint16


###########################################################################
# typedef struct { // 0x6
# 	/* 0x0 */ u_char destination_id;
# 	/* 0x1 */ u_char message_id;
# 	/* 0x2 */ u_short face_id;
# 	/* 0x4 */ u_short adpcm_id;
# } WANDER_SOUL_DAT;
class WANDER_SOUL_DAT(CStructure):
    destination_id: c_uint8
    message_id: c_uint8
    face_id: c_uint16
    adpcm_id: c_uint16


###########################################################################
# typedef struct {
# 	float center[3];
# 	float r;
# } SPHERE;
class SPHERE(CStructure):
    center: c_float * 3
    r: c_float


# typedef struct {
# 	float p0[3];
# 	float p1[3];
# 	float r;
# 	u_int axis;
# } TUBE;
class TUBE(CStructure):
    p0: c_float * 3
    p1: c_float * 3
    r: c_float
    axis: c_uint32


# typedef struct {
# 	u_char *name;
# 	float mass;
# 	float Ks;
# 	float dec;
# 	sceVu0FVECTOR pos;
# 	sceVu0FVECTOR gravity;
# } WMIM;
class WMIM(CStructure):
    name: c_addr_ptr
    mass: c_float
    Ks: c_float
    dec: c_float
    pos: sceVu0FVECTOR
    gravity: sceVu0FVECTOR


# typedef struct {
# 	WMIM *dat;
# 	u_char bone_id;
# } WMIM_DAT;
class WMIM_DAT(CStructure):
    dat: c_addr_ptr
    bone_id: c_uint8


# typedef struct {
# 	float *vtx;
# 	float gravity;
# 	float Kd;
# 	float Ke;
# 	u_int fixed_num;
# } ROPE_DAT;
class ROPE_DAT(CStructure):
    vtx: c_addr_ptr
    gravity: c_float
    Kd: c_float
    Ke: c_float
    fixed_num: c_uint32


# typedef struct {
# 	float gravity;
# 	float Kd;
# 	float Ke;
# 	u_char w;
# 	u_char h;
# 	u_char type;
# } CLOTH;
class CLOTH(CStructure):
    gravity: c_float
    Kd: c_float
    Ke: c_float
    w: c_uint8
    h: c_uint8
    type: c_uint8


# typedef struct {
# 	void *dat;
# 	u_char type;
# 	u_char bone_id;
# } COLLISION_DAT;
class COLLISION_DAT(CStructure):
    dat: c_addr_ptr
    type: c_uint8
    bone_id: c_uint8


# typedef struct {
# 	CLOTH *dat;
# 	sceVu0FVECTOR *rist_vtx;
# 	u_char bone_id;
# 	u_char bone_id2;
# 	u_char sgd_id;
# 	u_char flg;
# } CLOTH_DAT;
class CLOTH_DAT(CStructure):
    dat: c_addr_ptr
    rist_vtx: c_addr_ptr
    bone_id: c_uint8
    bone_id2: c_uint8
    sgd_id: c_uint8
    flg: c_uint8


elf_names: dict[str, str] = {
    "us": "SLUS_203.88",
    "eu": "SLES_508.21",
}


class VRamElf(BinaryIO):
    def __init__(self, elf: BinaryIO):
        self.elf = elf

    def read(self, n: int = -1):
        return self.elf.read(n)

    def seek(self, offset: int, whence: int = 0):
        return self.elf.seek(vram2offset(offset), whence)


# matches "var_name = 0x12345678; // attr1:val1 attr2:val2 ... attrN:valN"
line_pattern = re.compile(r"^([^\s=]+)\s*=\s*(0x[0-9a-fA-F]+);\s*\/\/((?:\s*[0-9a-zA-Z_]+(:?:[0-9a-zA-Z_\*]+)?)+)\s*$")
# matches multiple attributes in the form of attr:val or attr
attr_pattern = re.compile(r"([0-9a-zA-Z_]+)(?::([0-9a-zA-Z_\*]+))?")
# match address lines: "0xaabbccdd = &var"
addr_pattern = re.compile(r"^\s*(0x[0-9a-fA-F]+)\s*=\s*(.*)\s*;\s*(\/\/.*)?$")


class DataVar(pydantic.BaseModel):
    model_config = pydantic.ConfigDict(extra="forbid")

    _elf: VRamElf = pydantic.PrivateAttr()

    address: int
    name: str
    type: Type[CStructure] | CTypeType | type[sceVu0FVECTOR] | type[sceVu0IVECTOR] | type[c_str]
    numel: int | list[int] = 0
    nosize: bool = False
    static: bool = False

    num_ptr: int = 0

    @pydantic.model_validator(mode="after")
    def store_num_ptr(self):
        # transfer class attribute to instance
        if DataVar.num_ptr > 0:
            self.num_ptr = 1  # DataVar._num_ptr
        return self

    @pydantic.field_validator("type", mode="before")
    @classmethod
    def type_from_str(  # pyright: ignore
        cls, v: str | Type[CStructure] | CTypeType
    ) -> Type[CStructure] | CTypeType | sceVu0FVECTOR | sceVu0IVECTOR:
        if not isinstance(v, str):
            return v
        cls.num_ptr = v.count("*")  # temporary store num_ptr in class attribute
        v = v.rstrip("*")  # remove pointer(s)
        if v in ctypes_types:
            return ctypes_types[v]
        if v == "sceVu0FVECTOR":
            return sceVu0FVECTOR
        if v == "sceVu0IVECTOR":
            return sceVu0IVECTOR
        if v == "c_str":
            return c_str
        class_type = globals()[v]
        if issubclass(class_type, CStructure):
            return class_type
        raise ValueError(f"{v} is unknown/not a valid type")

    def data_var_dumps(self, addresses: dict[int, str]):
        self._elf.seek(self.address, os.SEEK_SET)

        if isinstance(self.numel, list):
            numel = math.prod(self.numel)
            ndims = len(self.numel)
        else:
            numel = max(1, self.numel)
            ndims = 1
        if self.num_ptr > 0:
            var_data = (c_addr_ptr * numel).from_buffer_copy(self._elf.read(numel * c_sizeof(c_addr_ptr)))
            if issubclass(self.type, CStructure):  # pyright: ignore
                type_str = self.type.__name__ + ("*" * self.num_ptr)
            elif self.type == sceVu0FVECTOR:  # pyright: ignore
                type_str = "sceVu0FVECTOR"
            elif self.type == sceVu0IVECTOR:  # pyright: ignore
                type_str = "sceVu0IVECTOR"
            else:
                type_str = next(
                    k
                    for k, v in ctypes_types.items()
                    if getattr(v, "_type_") == getattr(self.type, "_type_")  # pyright: ignore
                )
            stream = io.StringIO()
            if self.static:
                stream.write("static ")
            stream.write(f"{type_str} {self.name}")
            if numel > 1:
                numel = f"{self.numel}" if not self.nosize else ""
                var_str = f"{{ {', '.join(str(var) for var in cast(Iterable[c_addr_ptr], var_data))}, }}"  # pyright: ignore
                stream.write(f"[{numel}]")
            else:
                # should have been parsed as c_float_Array_4_Array_1
                assert len(var_data) == 1 and var_data[0].__class__ is c_addr_ptr
                var_str = str(var_data[0])
            stream.write(f" = {var_str};")
        elif issubclass(self.type, CStructure):  # pyright: ignore
            data = self._elf.read(numel * self.type.sizeof())

            return self.type.dumps(
                self.name,
                data,
                numel=self.numel,
                static=self.static,
                nosize=self.nosize,
                noarray=self.numel == 0,
            )
        elif self.type == sceVu0FVECTOR:  # pyright: ignore[reportUnknownMemberType]
            var_data = (sceVu0FVECTOR * numel).from_buffer_copy(self._elf.read(numel * c_sizeof(sceVu0FVECTOR)))
            type_str = "sceVu0FVECTOR"
            stream = io.StringIO()
            if self.static:
                stream.write("static ")
            stream.write(f"{type_str} {self.name}")

            def sceVu0FVECTOR_to_str(_v: sceVu0FVECTOR):  # pyright: ignore
                return f"{{ {', '.join(f'{flt}f' for flt in cast(Iterable[c_float], _v))} }}"

            if numel > 1:
                numel = f"{self.numel}" if not self.nosize else ""
                var_str = (
                    f"{{ {', '.join(sceVu0FVECTOR_to_str(var) for var in cast(Iterable[sceVu0FVECTOR], var_data))}, }}"  # pyright: ignore
                )
                stream.write(f"[{numel}]")
            else:
                # should have been parsed as c_float_Array_4_Array_1
                assert len(var_data) == 1 and var_data[0].__class__ is sceVu0FVECTOR
                var_str = sceVu0FVECTOR_to_str(var_data[0])
            stream.write(f" = {var_str};")
        elif self.type == sceVu0IVECTOR:  # pyright: ignore[reportUnknownMemberType]
            var_data = (sceVu0IVECTOR * numel).from_buffer_copy(self._elf.read(numel * c_sizeof(sceVu0IVECTOR)))
            type_str = "sceVu0IVECTOR"
            stream = io.StringIO()
            if self.static:
                stream.write("static ")
            stream.write(f"{type_str} {self.name}")

            def sceVu0IVECTOR_to_str(_v: sceVu0IVECTOR):  # pyright: ignore
                return f"{{ {', '.join(f'{int32}' for int32 in cast(Iterable[c_int32], _v))} }}"

            if numel > 1:
                numel = f"{self.numel}" if not self.nosize else ""
                var_str = (
                    f"{{ {', '.join(sceVu0IVECTOR_to_str(var) for var in cast(Iterable[sceVu0IVECTOR], var_data))}, }}"  # pyright: ignore
                )
                stream.write(f"[{numel}]")
            else:
                # should have been parsed as c_int_Array_4_Array_1
                assert len(var_data) == 1 and var_data[0].__class__ is sceVu0IVECTOR
                var_str = sceVu0IVECTOR_to_str(var_data[0])
            stream.write(f" = {var_str};")
        elif self.type == c_str:  # pyright: ignore
            assert numel > 1, "we only handle string arrays here, simple strings should be embedded in the source"
            var_data = (self.type * numel).from_buffer_copy(self._elf.read(numel * c_sizeof(self.type)))  # pyright: ignore
            stream = io.StringIO()
            if self.static:
                stream.write("static ")
            numel = f"{self.numel}" if not self.nosize else ""
            stream.write(f"char *{self.name}[{numel}] = {{")
            for var in cast(Iterable[c_str], var_data):
                stream.write(f"{var.to_str(self._elf)},")
            stream.write("};")
        else:
            typ = self.type  # pyright: ignore
            if ndims == 1:
                typ *= numel  # pyright: ignore
            elif isinstance(self.numel, list):
                for n in reversed(self.numel):
                    typ *= n  # pyright: ignore
            var_data = (typ).from_buffer_copy(self._elf.read(numel * c_sizeof(self.type)))  # pyright: ignore
            type_str = next(k for k, v in ctypes_types.items() if getattr(v, "_type_") == getattr(self.type, "_type_"))  # pyright: ignore
            stream = io.StringIO()
            if self.static:
                stream.write("static ")
            stream.write(f"{type_str} {self.name}")
            if ndims == 1:
                numel = f"{self.numel}" if not self.nosize else ""
                var_str = f"{{ {', '.join(f'{var}' for var in cast(Iterable[CTypeType], var_data))} }}"
                stream.write(f"[{numel}]")
            elif ndims > 1:
                assert isinstance(self.numel, list)
                numel = ""
                for n, num in enumerate(self.numel):
                    numel += "[]" if n == 0 and self.nosize else f"[{num}]"
                stream.write(numel)
                tmpbuf = io.StringIO()
                tmpbuf.write("{")
                print_carr(var_data, tmpbuf)  # pyright: ignore
                tmpbuf.write("}")
                var_str = tmpbuf.getvalue()
            else:
                var_str = f"{var_data.value}"  # pyright: ignore
            stream.write(f" = {var_str};")
        return stream.getvalue()


def parse_data_vars(data_vars_txt: Path, strict: bool = True):
    data_vars: list[DataVar] = []
    addr_vals: dict[int, str] = {}

    with open(data_vars_txt, mode="r") as fh:
        for n, line in enumerate(fh):
            line = line.strip()

            # skip empty lines and comments
            if not line or line.startswith("//"):
                continue

            valid = False

            if addr_match := addr_pattern.match(line):
                address = int(addr_match.group(1), 16)
                value = addr_match.group(2)
                addr_vals[address] = value
                valid = True

            elif line_match := line_pattern.match(line):
                address = int(line_match.group(2), 16)
                options = line_match.group(3)
                name = line_match.group(1)
                if attr_match := attr_pattern.findall(options):
                    attrs: dict[str, str | list[str] | bool] = {}
                    for attr_name, attr_val in cast(list[tuple[str, str]], attr_match):
                        if attr_name not in attrs:
                            attrs[attr_name] = attr_val
                        elif not isinstance(attrs[attr_name], list):
                            attrs[attr_name] = cast(list[str], [attrs[attr_name], attr_val])
                        else:
                            cast(list[str], attrs[attr_name]).append(attr_val)
                    for key, value in attrs.items():
                        if not isinstance(value, bool) and len(value) == 0:
                            attrs[key] = True
                    try:
                        data_var = DataVar(address=address, name=name, **attrs)  # pyright: ignore[reportArgumentType]
                        data_vars.append(data_var)
                        valid = True
                    except pydantic.ValidationError as ve:
                        print(f"syntax error at line {n}")
                        raise ve

            if strict and not valid:
                raise ValueError(f"syntax error at line {n}: {line}")

    return data_vars, addr_vals


def parse_data(lang: str):
    data_vars_txt = Path("config") / lang / "data_vars.txt"

    data_vars, addr_vals = parse_data_vars(data_vars_txt)

    if not data_vars:
        return

    if len(set(data_var.name for data_var in data_vars)) != len(data_vars):
        raise RuntimeError("duplicate names in data_vars.txt")

    elf_path = Path("config") / lang / elf_names[lang]

    include_path = Path("config") / lang / "include" / "data"
    include_path.mkdir(parents=True, exist_ok=True)

    c_addr_ptr.set_addresses(addr_vals)

    with open(elf_path, mode="rb") as elf:
        vram_elf = VRamElf(elf)
        CStructure.__elf__ = vram_elf

        for data_var in tqdm.tqdm(data_vars, desc="Extracting data"):
            header_path = include_path / f"{data_var.name}.h"

            with header_path.open(mode="w") as fw:
                data_var._elf = vram_elf  # pyright: ignore[reportPrivateUsage]

                fw.write(data_var.data_var_dumps(addr_vals))

            try:
                subprocess.run(["clang-format", "-i", "--style=Microsoft", header_path])
            except Exception:
                # formatting is best effort for now
                pass


def main():
    class Language(str, enum.Enum):
        US = "us"
        EU = "eu"

    languages = [lang.value for lang in Language]

    class ArgsProtocol(Protocol):
        language: Language

    parser = argparse.ArgumentParser(description="fixes asm removing gp_rel macro")
    parser.add_argument("language", type=str, choices=languages, help="language to extract data for")

    args = cast(ArgsProtocol, parser.parse_args())

    parse_data(args.language)


if __name__ == "__main__":
    main()
