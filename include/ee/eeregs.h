#ifndef EE_EEREGS_H
#define EE_EEREGS_H

#define D_CTRL_RELE_M     (0x01<<1)

#define REG_DMAC_CTRL     (volatile u_int *)0x1000e000
#define REG_DMAC_STAT     (volatile u_int *)0x1000e010

#define VU0_MICRO_ADDR    ((u_int)(0x11000000))
#define VU0_MEM_ADDR      ((u_int)(0x11004000))
#define VU1_MICRO_ADDR    ((u_int)(0x11008000))
#define VU1_MEM_ADDR      ((u_int)(0x1100c000))

#define IPU_CMD           ((volatile u_int *)(0x10002000))
#define IPU_CTRL          ((volatile u_int *)(0x10002010))
#define IPU_BP            ((volatile u_int *)(0x10002020))

#define DGET_IPU_CTRL()   (*IPU_CTRL)
#define DPUT_IPU_CMD(x)   (*IPU_CMD = (x))

#define DGET_IPU_CTRL()   (*IPU_CTRL)
#define DGET_IPU_BP()     (*IPU_BP)

#define D_ENABLER         ((volatile u_int *)(0x1000f520))
#define D_ENABLEW         ((volatile u_int *)(0x1000f590))
#define D3_CHCR           ((volatile u_int *)(0x1000b000))
#define D4_CHCR           ((volatile u_int *)(0x1000b400))

#define D3_CHCR           ((volatile u_int *)(0x1000b000))
#define D3_MADR           ((volatile u_int *)(0x1000b010))
#define D3_QWC            ((volatile u_int *)(0x1000b020))

#define D4_CHCR           ((volatile u_int *)(0x1000b400))
#define D4_MADR           ((volatile u_int *)(0x1000b410))
#define D4_QWC            ((volatile u_int *)(0x1000b420))
#define D4_TADR           ((volatile u_int *)(0x1000b430))


#define GS_CSR          ((volatile u_long *)(0x12001000))

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