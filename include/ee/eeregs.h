#ifndef EE_EEREGS_H
#define EE_EEREGS_H

typedef struct {
    unsigned EN1:1;
    unsigned EN2:1;
    unsigned CRTMD:3;
    unsigned MMOD:1;
    unsigned AMOD:1;
    unsigned SLBG:1;
    unsigned ALP:8;
    unsigned p0:16;
    unsigned int p1;    
} tGS_PMODE;

typedef struct {
    unsigned INT:1;
    unsigned FFMD:1;
    unsigned DPMS:2;
    unsigned p0:28;
    unsigned int p1;
} tGS_SMODE2;

typedef struct {
    unsigned FBP:9;
    unsigned FBW:6;
    unsigned PSM:5;
    unsigned p0:12;
    unsigned DBX:11;
    unsigned DBY:11;
    unsigned p1:10;
} tGS_DISPFB2;

typedef struct {
    unsigned DX:12;
    unsigned DY:11;
    unsigned MAGH:4;
    unsigned MAGV:2;
    unsigned p0:3;
    unsigned DW:12;
    unsigned DH:11;
    unsigned p1:9;
} tGS_DISPLAY2;

typedef struct {
    unsigned R:8;
    unsigned G:8;
    unsigned B:8;
    unsigned p0:8;
    unsigned int p1;
} tGS_BGCOLOR;

#endif // EE_EEREGS_H