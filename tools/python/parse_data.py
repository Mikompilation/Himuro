# pyright: reportInvalidTypeForm=false
from __future__ import annotations

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
sceVu0FMATRIX = sceVu0FVECTOR * 4

qword = c_int32 * 4


class ANI_CODE(c_uint16):
    def __str__(self):
        return f"{self.value}"


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


###########################################################################
# typedef struct {
# 	sceVu0FVECTOR p;
# 	sceVu0FVECTOR i;
# 	float roll;
# 	float fov;
# 	float nearz;
# 	float farz;
# 	float ax;
# 	float ay;
# 	float cx;
# 	float cy;
# 	float zmin;
# 	float zmax;
# 	float pad[2];
# 	sceVu0FMATRIX vs;
# 	sceVu0FMATRIX vc;
# 	sceVu0FMATRIX vcv;
# 	sceVu0FMATRIX wv;
# 	sceVu0FMATRIX ws;
# 	sceVu0FMATRIX wc;
# 	sceVu0FMATRIX wcv;
# 	sceVu0FVECTOR zd;
# 	sceVu0FVECTOR yd;
# } SgCAMERA;
class SgCAMERA(CStructure):
    p: sceVu0FVECTOR
    i: sceVu0FVECTOR
    roll: c_float
    fov: c_float
    nearz: c_float
    farz: c_float
    ax: c_float
    ay: c_float
    cx: c_float
    cy: c_float
    zmin: c_float
    zmax: c_float
    pad: c_float * 2
    vs: sceVu0FMATRIX
    vc: sceVu0FMATRIX
    vcv: sceVu0FMATRIX
    wv: sceVu0FMATRIX
    ws: sceVu0FMATRIX
    wc: sceVu0FMATRIX
    wcv: sceVu0FMATRIX
    zd: sceVu0FVECTOR
    yd: sceVu0FVECTOR


###########################################################################
# typedef struct {
# 	u_short file_no;
# 	u_char file_type;
# 	u_char tmp_no;
# 	u_int addr;
# } MSN_LOAD_DAT;
class MSN_LOAD_DAT(CStructure):
    file_no: c_uint16
    file_type: c_uint8
    tmp_no: c_uint8
    addr: c_uint32


###########################################################################
# typedef struct {
# 	u_short start;
# 	u_short end;
# 	u_short type;
# } SCN_VIB_DATA;
class SCN_VIB_DATA(CStructure):
    start: c_uint16
    end: c_uint16
    type: c_uint16


# typedef struct {
# 	int time0;
# 	int time1;
# 	int val;
# } SCN_VIB_TYPE;
class SCN_VIB_TYPE(CStructure):
    time0: c_int32
    time1: c_int32
    val: c_int32


# typedef struct {
# 	u_int type;
# 	float sclx;
# 	float scly;
# 	u_int alpha;
# 	float rate;
# 	float trate;
# } SCN_ENE_EF_PDF;
class SCN_ENE_EF_PDF(CStructure):
    type: c_uint32
    sclx: c_float
    scly: c_float
    alpha: c_uint32
    rate: c_float
    trate: c_float


# typedef struct {
# 	u_short scene_no;
# 	u_short mdl_no;
# 	float aura_size;
# 	float aura_rate;
# 	u_int aura_rgba;
# 	SCN_ENE_EF_PDF pdf1;
# 	SCN_ENE_EF_PDF pdf2;
# 	float pdf_dist;
# 	float aura_pos_ajst;
# 	int mdl_alpha;
# } SCN_ENE_EFCT;
class SCN_ENE_EFCT(CStructure):
    scene_no: c_int16
    mdl_no: c_int16
    aura_size: c_float
    aura_rate: c_float
    aura_rgba: c_uint32
    pdf1: SCN_ENE_EF_PDF
    pdf2: SCN_ENE_EF_PDF
    pdf_dist: c_float
    aura_pos_ajst: c_float
    mdl_alpha: c_int32


###########################################################################
# typedef struct {
# 	float scale;
# 	u_char neck_id;
# 	u_char waist_id;
# 	u_char leg_id;
# 	WMIM_DAT *wdat;
# 	CLOTH_DAT *cdat;
# 	COLLISION_DAT *collision;
# } MANMDL_DAT;
class MANMDL_DAT(CStructure):
    scale: c_float
    neck_id: c_uint8
    waist_id: c_uint8
    leg_id: c_uint8
    wdat: c_addr_ptr
    cdat: c_addr_ptr
    collision: c_addr_ptr


###########################################################################
# typedef struct {
# 	u_char scn_no;
# 	MSN_LOAD_DAT *load_dat;
# 	u_short *del_dat;
# } SCN_LOAD_DAT;
class SCN_LOAD_DAT(CStructure):
    scn_no: c_uint8
    load_dat: c_addr_ptr
    del_dat: c_addr_ptr


###########################################################################
# typedef struct {
# 	u_short score;
# 	u_char efct_no;
# 	u_char room;
# 	short pos[3];
# } PGOST_DAT;
class PGOST_DAT(CStructure):
    score: c_uint16
    efct_no: c_uint8
    room: c_uint8
    pos: c_int16 * 3


###########################################################################
# typedef struct {
#     u_short st_x0;
#     short st_y0;
#     u_short st_z0;
#     u_short st_x1;
#     short st_y1;
#     u_short st_z1;
#     u_short ed_x0;
#     short ed_y0;
#     u_short ed_z0;
#     u_short ed_x1;
#     short ed_y1;
#     u_short ed_z1;
# } SGEF_DAT;
class SGEF_DAT(CStructure):
    st_x0: c_uint16
    st_y0: c_int16
    st_z0: c_uint16
    st_x1: c_uint16
    st_y1: c_int16
    st_z1: c_uint16
    ed_x0: c_uint16
    ed_y0: c_int16
    ed_z0: c_uint16
    ed_x1: c_uint16
    ed_y1: c_int16
    ed_z1: c_uint16


###########################################################################
# typedef struct {
# 	u_char room_no;
# 	u_char ene_type;
# 	u_char pos_num;
# 	u_short cmr_no;
# 	u_short rot[3];
# 	int pos[3][3];
# } FGST_AP_DAT;
class FGST_AP_DAT(CStructure):
    room_no: c_uint8
    ene_type: c_uint8
    pos_num: c_uint8
    cmr_no: c_uint16
    rot: c_uint16 * 3
    pos: c_int32 * 3 * 3


# typedef struct {
# 	u_char room;
# 	u_char ap_step0;
# 	u_char ap_step1[4];
# 	u_char ap_step2[6];
# 	u_short ap_near;
# 	u_short ap_far;
# 	u_short ap_stts;
# } RGOST_AP_DAT;
class RGOST_AP_DAT(CStructure):
    room: c_uint8
    ap_step0: c_uint8
    ap_step1: c_uint8 * 4
    ap_step2: c_uint8 * 6
    ap_near: c_uint16
    ap_far: c_uint16
    ap_stts: c_uint16


# typedef struct {
# 	u_short score;
# 	u_char center_no;
# 	u_char center_num;
# 	u_char start;
# 	u_char inter;
# 	u_char end;
# 	u_char dummy;
# 	short int pos[3];
# } RGOST_DAT;
class RGOST_DAT(CStructure):
    score: c_uint16
    center_no: c_uint8
    center_num: c_uint8
    start: c_uint8
    inter: c_uint8
    end: c_uint8
    dummy: c_uint8
    pos: c_int16 * 3


# typedef struct {
# 	u_short flame;
# 	u_char alp_no;
# 	u_char ani_no;
# 	u_char efct_no;
# 	u_char rate_no;
# } RG_DISP_DAT;
class RG_DISP_DAT(CStructure):
    flame: c_uint16
    alp_no: c_uint8
    ani_no: c_uint8
    efct_no: c_uint8
    rate_no: c_uint8


# typedef struct {
# 	u_short flame;
# 	u_char alp;
# 	u_char stts;
# } RG_ALP_DAT;
class RG_ALP_DAT(CStructure):
    flame: c_uint16
    alp: c_uint8
    stts: c_uint8


# typedef struct {
# 	u_char flame;
# 	u_char tim_no;
# } RG_ANI_DAT;
class RG_ANI_DAT(CStructure):
    flame: c_uint8
    tim_no: c_uint8


# typedef struct {
# 	u_short flame;
# 	u_char rate;
# 	u_char stts;
# } RG_RATE_DAT;
class RG_RATE_DAT(CStructure):
    flame: c_uint16
    rate: c_uint8
    stts: c_uint8


###########################################################################
# typedef struct {
#     sceVu0FVECTOR pos;
#     u_char floor;
# } FREE_DAT;
class FREE_DAT(CStructure, align=16):
    pos: sceVu0FVECTOR
    floor: c_uint8


###########################################################################
# typedef struct {
#     u_short attr_no;
#     u_short dist_n;
#     u_short dist_f;
#     u_short score;
#     u_short fact_no;
#     u_char fp_num;
#     u_char fp_no;
#     u_char fp_rot;
#     u_char fp_mak;
#     u_char acs_flg;
#     u_char ef_type;
#     u_char ef_no;
# } FURN_DAT;
class FURN_DAT(CStructure):
    attr_no: c_uint16
    dist_n: c_uint16
    dist_f: c_uint16
    score: c_uint16
    fact_no: c_uint16
    fp_num: c_uint8
    fp_no: c_uint8
    fp_rot: c_uint8
    fp_mak: c_uint8
    acs_flg: c_uint8
    ef_type: c_uint8
    ef_no: c_uint8


###########################################################################
# typedef struct {
# 	u_short time;
# 	u_short score;
# 	u_short rank[5];
# 	u_char clear;
# 	u_char film[2];
# 	u_char reiseki;
# 	u_char map_no;
# 	u_char room;
# 	u_char floor;
# 	u_char ene_no[3];
# 	u_char ene_ap[3];
# 	u_char rgst_no[5];
# 	u_short fly_mdl_no;
# 	short int prot;
# 	sceVu0FVECTOR ppos;
# 	sceVu0FVECTOR epos[3];
# 	sceVu0FVECTOR rgpos[5];
# } STAGE_DAT;
class STAGE_DAT(CStructure):
    time: c_uint16
    score: c_uint16
    rank: c_uint16 * 5
    clear: c_uint8
    film: c_uint8 * 2
    reiseki: c_uint8
    map_no: c_uint8
    room: c_uint8
    floor: c_uint8
    ene_no: c_uint8 * 3
    ene_ap: c_uint8 * 3
    rgst_no: c_uint8 * 5
    fly_mdl_no: c_uint16
    prot: c_int16
    _pad_0: c_uint8 * 12
    ppos: sceVu0FVECTOR
    epos: sceVu0FVECTOR * 3
    rgpos: sceVu0FVECTOR * 5


###########################################################################
# typedef struct {
# 	u_char *str;
# 	int pos_x;
# 	int pos_y;
# 	int type;
# 	u_int r;
# 	u_int g;
# 	u_int b;
# 	int alpha;
# 	int pri;
# } STR_DAT;
class STR_DAT(CStructure):
    str: c_str
    pos_x: c_int32
    pos_y: c_int32
    type: c_int32
    r: c_uint32
    g: c_uint32
    b: c_uint32
    alpha: c_int32
    pri: c_int32


# typedef struct {
# 	u_int w;
# 	u_int h;
# 	int x;
# 	int y;
# 	u_int pri;
# 	u_char r;
# 	u_char g;
# 	u_char b;
# 	u_char alpha;
# } SQAR_DAT;
class SQAR_DAT(CStructure):
    w: c_uint32
    h: c_uint32
    x: c_int32
    y: c_int32
    pri: c_uint32
    r: c_uint8
    g: c_uint8
    b: c_uint8
    alpha: c_uint8


# typedef struct {
#     u_char move_num;
#     u_char empty_no;
#     u_char start[5];
#     u_char line[10];
# } STAR_PZL_DAT;
class STAR_PZL_DAT(CStructure):
    move_num: c_uint8
    empty_no: c_uint8
    start: c_uint8 * 5
    line: c_uint8 * 10


# typedef struct {
#     u_char dial_num;
#     u_char lock_door;
#     u_char dial_no[5];
# } DIAL_KEY_DAT;
class DIAL_KEY_DAT(CStructure):
    dial_num: c_uint8
    lock_door: c_uint8
    dial_no: c_uint8 * 5


# typedef struct {
#     u_char answer;
#     u_char order[9];
#     u_char unfade_doll[3];
#     u_char pad;
# } EV_DOLL_DAT;
class EV_DOLL_DAT(CStructure):
    answer: c_uint8
    order: c_uint8 * 9
    unfade_doll: c_uint8 * 3
    pad: c_uint8


# typedef struct {
#     short int use_flg;
#     short int set_place;
# } EV_BTZ_DAT;
class EV_BTZ_DAT(CStructure):
    use_flg: c_int16
    set_place: c_int16


# typedef struct {
#     short int t_counter;
#     short int bld_apr[10];
#     short int bld_end[10];
#     short int pad;
# } EV_BLD_DAT;
class EV_BLD_DAT(CStructure):
    t_counter: c_int16
    bld_apr: c_int16 * 10
    bld_end: c_int16 * 10
    pad: c_int16


# typedef struct {
#     short int cdl_flg[3][6];
#     short int flame_alpha[6];
#     short int flame_shape[6];
#     short int stflame_alpha[6];
#     short int stflame_shape[6];
#     short int flame_time[6];
#     short int stflame_time[6];
# } EV_CDL_DAT;
class EV_CDL_DAT(CStructure):
    cdl_flg: c_int16 * 6 * 3
    flame_alpha: c_int16 * 6
    flame_shape: c_int16 * 6
    stflame_alpha: c_int16 * 6
    stflame_shape: c_int16 * 6
    flame_time: c_int16 * 6
    stflame_time: c_int16 * 6


# typedef struct {
#     u_char answer;
#     u_char ansflg;
# } ZUSHI_WRK;
class ZUSHI_WRK(CStructure):
    answer: c_uint8
    ansflg: c_uint8


# typedef struct {
#     u_char no;
#     u_char ret;
#     u_char mode;
#     u_char time;
#     u_char count;
#     u_char csr[3];
# } SPECIAL_EVENT_WRK;
class SPECIAL_EVENT_WRK(CStructure):
    no: c_uint8
    ret: c_uint8
    mode: c_uint8
    time: c_uint8
    count: c_uint8
    csr: c_uint8 * 3


# typedef struct {
#     u_char pzl_no;
#     u_char mode;
#     u_char menu_mode;
#     u_char menu_csr[2];
#     u_char time;
#     u_char count;
#     u_char empty;
#     u_char slct_no;
#     u_char bak_no[10];
#     u_char stone[5];
#     u_char line[5][5];
# } STAR_PZL_WRK;
class STAR_PZL_WRK(CStructure):
    pzl_no: c_uint8
    mode: c_uint8
    menu_mode: c_uint8
    menu_csr: c_uint8 * 2
    time: c_uint8
    count: c_uint8
    empty: c_uint8
    slct_no: c_uint8
    bak_no: c_uint8 * 10
    stone: c_uint8 * 5
    line: c_uint8 * 5 * 5


# typedef struct {
#     u_char door_no;
#     u_char mode;
#     u_char time;
#     u_char count;
#     u_char slct_no;
#     u_char push;
#     u_char btn[10];
#     u_char push_no[5];
# } DIAL_KEY_WRK;
class DIAL_KEY_WRK(CStructure):
    door_no: c_uint8
    mode: c_uint8
    time: c_uint8
    count: c_uint8
    slct_no: c_uint8
    push: c_uint8
    btn: c_uint8 * 10
    push_no: c_uint8 * 5


###########################################################################
# typedef struct {
# 	u_short attribute;
# 	short int open_wait;
# 	short int close_wait;
# 	short int move_max;
# 	int se_file;
# 	u_char room_id;
# 	u_short dbl_did;
# } DOOR_TYPE_DAT;
class DOOR_TYPE_DAT(CStructure):
    attribute: c_uint16
    open_wait: c_int16
    close_wait: c_int16
    move_max: c_int16
    se_file: c_int32
    room_id: c_uint8
    dbl_did: c_uint16


# typedef struct {
# 	float sx;
# 	float sy;
# 	u_char dmd_no1;
# 	u_char dmd_no2;
# 	u_char anm_no;
# 	u_char anm_last_no;
# 	u_char c_dmd_no1;
# 	u_char c_dmd_no2;
# 	u_char c_anm_no;
# } DOOR_MTN_DAT;
class DOOR_MTN_DAT(CStructure):
    sx: c_float
    sy: c_float
    dmd_no1: c_uint8
    dmd_no2: c_uint8
    anm_no: c_uint8
    anm_last_no: c_uint8
    c_dmd_no1: c_uint8
    c_dmd_no2: c_uint8
    c_anm_no: c_uint8


# typedef struct {
# 	u_short st_tbl_pos;
# 	u_short start_cnt;
# 	u_short end_cnt;
# 	u_short sel_cnt;
# } DOOR_MOVE_DAT;
class DOOR_MOVE_DAT(CStructure):
    st_tbl_pos: c_uint16
    start_cnt: c_uint16
    end_cnt: c_uint16
    sel_cnt: c_uint16


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
    type: Type[CStructure] | CTypeType | type[sceVu0FVECTOR] | type[sceVu0IVECTOR] | type[qword] | type[c_str]
    numel: int | list[int] = 0
    nosize: bool = False
    static: bool = False

    num_ptr: int = 0

    @pydantic.model_validator(mode="after")
    def store_num_ptr(self):
        # transfer class attribute to instance
        if DataVar.num_ptr > 0:
            self.num_ptr = DataVar.num_ptr
        return self

    @pydantic.field_validator("type", mode="before")
    @classmethod
    def type_from_str(  # pyright: ignore
        cls, v: str | Type[CStructure] | CTypeType
    ) -> Type[CStructure] | CTypeType | sceVu0FVECTOR | sceVu0IVECTOR | qword | ANI_CODE:
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
        if v == "qword":
            return qword
        if v == "ANI_CODE":
            return ANI_CODE
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
            if self.type == ANI_CODE:  # pyright: ignore
                type_str = "ANI_CODE" + ("*" * self.num_ptr)
            elif issubclass(self.type, CStructure):  # pyright: ignore
                type_str = self.type.__name__ + ("*" * self.num_ptr)
            elif self.type == sceVu0FVECTOR:  # pyright: ignore
                type_str = "sceVu0FVECTOR" + ("*" * self.num_ptr)
            elif self.type == sceVu0IVECTOR:  # pyright: ignore
                type_str = "sceVu0IVECTOR" + ("*" * self.num_ptr)
            else:
                type_str = next(
                    k
                    for k, v in ctypes_types.items()
                    if getattr(v, "_type_") == getattr(self.type, "_type_")  # pyright: ignore
                ) + ("*" * self.num_ptr)
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
            if self.type.__name__ == "ANI_CODE":  # pyright: ignore
                type_str = "ANI_CODE"
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
                        if address not in addr_vals:
                            if (
                                isinstance(data_var.numel, list) or data_var.numel > 0 or data_var.type == sceVu0FVECTOR  # type: ignore
                            ):
                                addr_vals[address] = name
                            else:
                                addr_vals[address] = f"&{name}"
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
