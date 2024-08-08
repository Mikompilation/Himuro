#ifndef EE_EESTRUCT_H
#define EE_EESTRUCT_H

#include "typedefs.h"

#define SCR_WIDTH 640
#define SCR_HEIGHT 224

#define DISP_WIDTH 640
#define DISP_HEIGHT 448

#define GS_X_COORD(x) ((2048 - (SCR_WIDTH / 2) + x) * 16)
#define GS_Y_COORD(y) ((2048 - (SCR_HEIGHT / 2) + y) * 16)

#define DMArefe             (0<<28)
#define DMAcnt              (1<<28)
#define DMAnext             (2<<28)
#define DMAref              (3<<28)
#define DMArefs             (4<<28)
#define DMAcall             (5<<28)
#define DMAret              (6<<28)
#define DMAend              (7<<28)

#define	SCE_GS_FALSE			(0)
#define	SCE_GS_TRUE			    (1)

#define SCE_GS_ZNOUSE			(0)
#define SCE_GS_ZALWAYS			(1)
#define SCE_GS_ZGEQUAL			(2)
#define SCE_GS_ZGREATER			(3)

#define SCE_GS_PSMCT32			(0)
#define SCE_GS_PSMCT24			(1)
#define SCE_GS_PSMCT16			(2)
#define SCE_GS_PSMCT16S			(10)
#define SCE_GS_PSMT8			(19)
#define SCE_GS_PSMT4			(20)
#define SCE_GS_PSMT8H			(27)
#define SCE_GS_PSMT4HL			(36)
#define SCE_GS_PSMT4HH			(44)
#define SCE_GS_PSMZ32			(48)
#define SCE_GS_PSMZ24			(49)
#define SCE_GS_PSMZ16			(50)
#define SCE_GS_PSMZ16S			(58)

#define SCE_GS_ALPHA_NEVER		(0)
#define SCE_GS_ALPHA_ALWAYS		(1)
#define SCE_GS_ALPHA_LESS		(2)
#define SCE_GS_ALPHA_LEQUAL		(3)
#define SCE_GS_ALPHA_EQUAL		(4)
#define SCE_GS_ALPHA_GEQUAL		(5)
#define SCE_GS_ALPHA_GREATER	(6)
#define SCE_GS_ALPHA_NOTEQUAL	(7)

#define	SCE_GS_AFAIL_KEEP		(0)
#define	SCE_GS_AFAIL_FB_ONLY	(1)
#define SCE_GS_AFAIL_ZB_ONLY	(2)
#define SCE_GS_AFAIL_RGB_ONLY	(3)

#define SCE_GS_DEPTH_NEVER		(0)
#define SCE_GS_DEPTH_ALWAYS		(1)
#define SCE_GS_DEPTH_GEQUAL		(2)
#define SCE_GS_DEPTH_GREATER	(3)

#define SCE_GS_ALPHA_CS			(0)
#define SCE_GS_ALPHA_CD			(1)
#define SCE_GS_ALPHA_ZERO		(2)

#define SCE_GS_ALPHA_AS			(0)
#define SCE_GS_ALPHA_AD			(1)
#define SCE_GS_ALPHA_FIX		(2)

#define SCE_GIF_PACKED          (0)
#define SCE_GIF_REGLIST         (1)
#define SCE_GIF_IMAGE           (2)
#define SCE_GIF_PACKED_AD       (14)

#define SCE_GS_NOCLEAR			(0)
#define SCE_GS_CLEAR			(1)

#define SCE_GS_MODULATE			(0)
#define SCE_GS_DECAL			(1)
#define SCE_GS_HILIGHT			(2)
#define SCE_GS_GHLIGHT2   		(3)

#define SCE_GS_NEAREST			        (0)
#define SCE_GS_LINEAR			        (1)
#define SCE_GS_NEAREST_MIPMAP_NEAREST	(2)
#define SCE_GS_NEAREST_MIPMAP_LINEAR	(3)
#define SCE_GS_LINEAR_MIPMAP_NEAREST	(4)
#define SCE_GS_LINEAR_MIPMAP_LINEAR	    (5)

#define SCE_GS_PRIM             0x00
#define SCE_GS_RGBAQ            0x01
#define SCE_GS_ST               0x02
#define SCE_GS_UV               0x03
#define SCE_GS_XYZF2            0x04
#define SCE_GS_XYZ2             0x05
#define SCE_GS_FOG              0x0a
#define SCE_GS_XYZF3            0x0c
#define SCE_GS_XYZ3             0x0d
#define SCE_GS_XYOFFSET_1       0x18
#define SCE_GS_XYOFFSET_2       0x19
#define SCE_GS_PRMODECONT       0x1a

#define SCE_GS_PRMODE           0x1b
#define SCE_GS_TEX0_1           0x06
#define SCE_GS_TEX0_2           0x07
#define SCE_GS_TEX1_1           0x14
#define SCE_GS_TEX1_2           0x15
#define SCE_GS_TEX2_1           0x16
#define SCE_GS_TEX2_2           0x17
#define SCE_GS_TEXCLUT          0x1c
#define SCE_GS_SCANMSK          0x22
#define SCE_GS_MIPTBP1_1        0x34
#define SCE_GS_MIPTBP1_2        0x35
#define SCE_GS_MIPTBP2_1        0x36
#define SCE_GS_MIPTBP2_2        0x37
#define SCE_GS_CLAMP_1          0x08
#define SCE_GS_CLAMP_2          0x09
#define SCE_GS_TEXA             0x3b
#define SCE_GS_FOGCOL           0x3d
#define SCE_GS_TEXFLUSH         0x3f

#define SCE_GS_SCISSOR_1        0x40
#define SCE_GS_SCISSOR_2        0x41
#define SCE_GS_ALPHA_1          0x42
#define SCE_GS_ALPHA_2          0x43
#define SCE_GS_DIMX             0x44
#define SCE_GS_DTHE             0x45
#define SCE_GS_COLCLAMP         0x46
#define SCE_GS_TEST_1           0x47
#define SCE_GS_TEST_2           0x48
#define SCE_GS_PABE             0x49
#define SCE_GS_FBA_1            0x4a
#define SCE_GS_FBA_2            0x4b

#define SCE_GS_FRAME_1          0x4c
#define SCE_GS_FRAME_2          0x4d
#define SCE_GS_ZBUF_1           0x4e
#define SCE_GS_ZBUF_2           0x4f

#define SCE_GS_BITBLTBUF        0x50
#define SCE_GS_TRXPOS           0x51
#define SCE_GS_TRXREG           0x52
#define SCE_GS_TRXDIR           0x53
#define SCE_GS_HWREG            0x54

#define SCE_GS_SIGNAL           0x60
#define SCE_GS_FINISH           0x61
#define SCE_GS_LABEL            0x62
#define SCE_GS_NOP              0x7f

#define SCE_GS_SET_TEX0_1   SCE_GS_SET_TEX0
#define SCE_GS_SET_TEX0_2   SCE_GS_SET_TEX0
#define SCE_GS_SET_TEX0(tbp, tbw, psm, tw, th, tcc, tfx, cbp, cpsm, csm, csa, cld) \
    ((u_long)(tbp)         | ((u_long)(tbw) << 14) | \
    ((u_long)(psm) << 20)  | ((u_long)(tw) << 26) | \
    ((u_long)(th) << 30)   | ((u_long)(tcc) << 34) | \
    ((u_long)(tfx) << 35)  | ((u_long)(cbp) << 37) | \
    ((u_long)(cpsm) << 51) | ((u_long)(csm) << 55) | \
    ((u_long)(csa) << 56)  | ((u_long)(cld) << 61))

#define SCE_GIF_SET_TAG(nloop, eop, pre, prim, flg, nreg) \
    ((u_long)(nloop) | ((u_long)(eop)<<15) | ((u_long)(pre) << 46) | \
    ((u_long)(prim)<<47) | ((u_long)(flg)<<58) | ((u_long)(nreg)<<60))

#define SCE_GIF_CLEAR_TAG(tp) \
    (*(u_long128 *)(tp) = (u_long128)0)

#define SCE_GS_SET_ALPHA_1  SCE_GS_SET_ALPHA
#define SCE_GS_SET_ALPHA_2  SCE_GS_SET_ALPHA
#define SCE_GS_SET_ALPHA(a, b, c, d, fix) \
    ((u_long)(a)       | ((u_long)(b) << 2)     | ((u_long)(c) << 4) | \
    ((u_long)(d) << 6) | ((u_long)(fix) << 32))

#define SCE_GS_SET_TEST_1 SCE_GS_SET_TEST
#define SCE_GS_SET_TEST_2 SCE_GS_SET_TEST
#define SCE_GS_SET_TEST(ate, atst, aref, afail, date, datm, zte, ztst) \
    ((u_long)(ate)         | ((u_long)(atst) << 1) | \
    ((u_long)(aref) << 4)  | ((u_long)(afail) << 12) | \
    ((u_long)(date) << 14) | ((u_long)(datm) << 15) | \
    ((u_long)(zte) << 16)  | ((u_long)(ztst) << 17))

#define SCE_GS_SET_ZBUF_1   SCE_GS_SET_ZBUF
#define SCE_GS_SET_ZBUF_2   SCE_GS_SET_ZBUF
#define SCE_GS_SET_ZBUF(zbp, psm, zmsk) \
    ((u_long)(zbp) | ((u_long)(psm) << 24) | \
    ((u_long)(zmsk) << 32))

#define SCE_VIF1_SET_FLUSH(irq) \
    (((u_int)0x11 << 24) | ((u_int)(irq) << 31))

#define SCE_VIF1_SET_DIRECT(count, irq) \
    ((u_int)(count) | ((u_int)(0x50) << 24) | ((u_int)(irq) << 31))

#define SCE_GS_SET_BITBLTBUF(sbp, sbw, spsm, dbp, dbw, dpsm) \
    ((u_long)(sbp)         | ((u_long)(sbw) << 16) | \
    ((u_long)(spsm) << 24) | ((u_long)(dbp) << 32) | \
    ((u_long)(dbw) << 48)  | ((u_long)(dpsm) << 56))

#define SCE_GS_SET_TRXDIR(xdr) ((u_long)(xdr))

#define SCE_GS_SET_TRXPOS(ssax, ssay, dsax, dsay, dir) \
    ((u_long)(ssax)        | ((u_long)(ssay) << 16) | \
    ((u_long)(dsax) << 32) | ((u_long)(dsay) << 48) | \
    ((u_long)(dir) << 59))

#define SCE_GS_SET_TRXREG(rrw, rrh) \
    ((u_long)(rrw) | ((u_long)(rrh) << 32))

#define SCE_GS_SET_UV(u, v) ((u_long)(u) | ((u_long)(v) << 16))

#define SCE_GS_SET_XYZF3 SCE_GS_SET_XYZF
#define SCE_GS_SET_XYZF2 SCE_GS_SET_XYZF
#define SCE_GS_SET_XYZF(x, y, z, f) \
    ((u_long)(x) | ((u_long)(y) << 16) | ((u_long)(z) << 32) | \
    ((u_long)(f) << 56))

#define SCE_GS_SET_FRAME_1  SCE_GS_SET_FRAME
#define SCE_GS_SET_FRAME_2  SCE_GS_SET_FRAME
#define SCE_GS_SET_FRAME(fbp, fbw, psm, fbmask) \
    ((u_long)(fbp)        | ((u_long)(fbw) << 16) | \
    ((u_long)(psm) << 24) | ((u_long)(fbmask) << 32))

#define SCE_GS_SET_TEX1_1   SCE_GS_SET_TEX1
#define SCE_GS_SET_TEX1_2   SCE_GS_SET_TEX1
#define SCE_GS_SET_TEX1(lcm, mxl, mmag, mmin, mtba, l, k) \
    ((u_long)(lcm)        | ((u_long)(mxl) << 2)  | \
    ((u_long)(mmag) << 5) | ((u_long)(mmin) << 6) | \
    ((u_long)(mtba) << 9) | ((u_long)(l) << 19) | \
    ((u_long)(k) << 32))

#define SCE_GS_SET_TEX0_1   SCE_GS_SET_TEX0
#define SCE_GS_SET_TEX0_2   SCE_GS_SET_TEX0
#define SCE_GS_SET_TEX0(tbp, tbw, psm, tw, th, tcc, tfx, cbp, cpsm, csm, csa, cld) \
    ((u_long)(tbp)         | ((u_long)(tbw) << 14) | \
    ((u_long)(psm) << 20)  | ((u_long)(tw) << 26) | \
    ((u_long)(th) << 30)   | ((u_long)(tcc) << 34) | \
    ((u_long)(tfx) << 35)  | ((u_long)(cbp) << 37) | \
    ((u_long)(cpsm) << 51) | ((u_long)(csm) << 55) | \
    ((u_long)(csa) << 56)  | ((u_long)(cld) << 61))

#define SCE_GS_SET_XYOFFSET_1   SCE_GS_SET_XYOFFSET
#define SCE_GS_SET_XYOFFSET_2   SCE_GS_SET_XYOFFSET
#define SCE_GS_SET_XYOFFSET(ofx, ofy) \
    ((u_long)(ofx) | ((u_long)(ofy) << 32))

#define SCE_GS_SET_CLAMP_1  SCE_GS_SET_CLAMP
#define SCE_GS_SET_CLAMP_2  SCE_GS_SET_CLAMP
#define SCE_GS_SET_CLAMP(wms, wmt, minu, maxu, minv, maxv) \
    ((u_long)(wms)         | ((u_long)(wmt) << 2) | \
    ((u_long)(minu) << 4)  | ((u_long)(maxu) << 14) | \
    ((u_long)(minv) << 24) | ((u_long)(maxv) << 34))

#define SCE_GS_SET_SCISSOR_1    SCE_GS_SET_SCISSOR
#define SCE_GS_SET_SCISSOR_2    SCE_GS_SET_SCISSOR
#define SCE_GS_SET_SCISSOR(scax0, scax1, scay0, scay1) \
    ((u_long)(scax0)        | ((u_long)(scax1) << 16) | \
    ((u_long)(scay0) << 32) | ((u_long)(scay1) << 48))

typedef struct {
    unsigned long CLAMP:1;
    unsigned long pad01:63;
} sceGsColclamp;

typedef struct {
    unsigned long DTHE:1;
    unsigned long pad01:63;
} sceGsDthe;

typedef struct {
    unsigned long FBP:9;
    unsigned long pad09:7;
    unsigned long FBW:6;
    unsigned long pad22:2;
    unsigned long PSM:6;
    unsigned long pad30:2;
    unsigned long FBMSK:32;
} sceGsFrame;

typedef struct {
    unsigned long AC:1;
    unsigned long pad01:63;
} sceGsPrmodecont;

typedef struct {
    unsigned long SCAX0:11;
    unsigned long pad11:5;
    unsigned long SCAX1:11;
    unsigned long pad27:5;
    unsigned long SCAY0:11;
    unsigned long pad43:5;
    unsigned long SCAY1:11;
    unsigned long pad59:5;
} sceGsScissor;

typedef struct {
    unsigned long ATE:1;
    unsigned long ATST:3;
    unsigned long AREF:8;
    unsigned long AFAIL:2;
    unsigned long DATE:1;
    unsigned long DATM:1;
    unsigned long ZTE:1;
    unsigned long ZTST:2;
    unsigned long pad19:45;
} sceGsTest;

typedef struct {
    unsigned long OFX:16;
    unsigned long pad16:16;
    unsigned long OFY:16;
    unsigned long pad48:16;
} sceGsXyoffset;

typedef struct {
    unsigned long ZBP:9;
    unsigned long pad09:15;
    unsigned long PSM:4;
    unsigned long pad28:4;
    unsigned long ZMSK:1;
    unsigned long pad33:31;
} sceGsZbuf;

typedef struct {
    unsigned long PRIM:3;
    unsigned long IIP:1;
    unsigned long TME:1;
    unsigned long FGE:1;
    unsigned long ABE:1;
    unsigned long AA1:1;
    unsigned long FST:1;
    unsigned long CTXT:1;
    unsigned long FIX:1;
    unsigned long pad11:53;
} sceGsPrim;

typedef struct {
    u_int R:8;
    u_int G:8;
    u_int B:8;
    u_int A:8;
    float Q;
} sceGsRgbaq;

typedef struct {
    unsigned long X:16;
    unsigned long Y:16;
    unsigned long Z:32;
} sceGsXyz;

#endif // EE_EESTRUCT_H