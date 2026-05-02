#ifndef TYPEDEFS_H 
#define TYPEDEFS_H

typedef long s_long;
typedef short s_short;
typedef char s_char;
typedef int s_int;
typedef int s_long128 __attribute__ ((mode (TI)));

typedef unsigned long u_long;
typedef unsigned short u_short;
typedef unsigned char u_char;
typedef unsigned int u_int;
typedef unsigned int u_long128 __attribute__ ((mode (TI)));

typedef long int dword[2] __attribute__ ((aligned(16)));;
typedef int qword[4] __attribute__ ((aligned(16)));
typedef int sceVu0IVECTOR[4] __attribute__((aligned (16)));
typedef float sceVu0FVECTOR[4] __attribute__((aligned (16)));
typedef float sceVu0FMATRIX[4][4] __attribute__((aligned (16)));

typedef union {
	int ui32;
	float fl32;
} U32DATA;

typedef union {
	u_long128 ul128;
	u_long ul64[2];
	u_int ui32[4];
	float fl32[4];
	u_short us16[8];
	u_char uc8[16];
	sceVu0FVECTOR fv;
	sceVu0IVECTOR iv;
} Q_WORDDATA;

typedef union {
	u_char *pu8;
	u_short *pu16;
	u_int *pu32;
	u_long *pu64;
	s_char *ps8;
	s_short *ps16;
	int *ps32;
	s_long *ps64;
	long int wrk;
} P_INT;

#if defined(BUILD_JP_VERSION)
    #define VER_LOAD_REQ_LANG LoadReq
	#define VER_ATAN2F atan2f
	#define VER_SINF sinf
	#define VER_COSF cosf
	#define VER_SQRTF sqrtf
#elif defined(BUILD_US_VERSION)
    #define VER_LOAD_REQ_LANG LoadReq
	#define VER_ATAN2F SgAtan2f
	#define VER_SINF SgSinf
	#define VER_COSF SgCosf
	#define VER_SQRTF SgSqrtf
#elif defined(BUILD_EU_VERSION)
    #define VER_LOAD_REQ_LANG LoadReqLanguage
	#define VER_ATAN2F SgAtan2f
	#define VER_SINF SgSinf
	#define VER_COSF SgCosf
	#define VER_SQRTF SgSqrtf
#endif

// #include <cstdlib.h>
// RAND_MAX = (2**31-1)
#define RAND_MAX 2147483647

#if defined(BUILD_JP_VERSION)
#define VER_RAND() rand() / (float)RAND_MAX
#elif defined(BUILD_US_VERSION)
#define VER_RAND() vu0Rand()
#elif defined(BUILD_EU_VERSION)
#define VER_RAND() vu0Rand()
#endif


#define PAD_DPAD_UP      0
#define PAD_DPAD_DOWN    1
#define PAD_DPAD_LEFT    2
#define PAD_DPAD_RIGHT   3

#define PAD_TRIANGLE     4
#define PAD_CROSS        5
#define PAD_SQUARE       6
#define PAD_CIRCLE       7

#define PAD_L1           8
#define PAD_L2           9
#define PAD_R1          10
#define PAD_R2          11

#define PAD_START       12
#define PAD_SELECT      13
#define PAD_L3          14
#define PAD_R3          15

#define PAD_LANA_UP      0
#define PAD_LANA_RIGHT   1
#define PAD_LANA_DOWN    2
#define PAD_LANA_LEFT    3


#define PAD_REPEAT_DELAY   25
#define PAD_REPEAT_RATE     5

#define PAD_REPEAT(cnt) \
    (((cnt) == 1) || ((cnt) > PAD_REPEAT_DELAY && ((cnt) % PAD_REPEAT_RATE) == 1))

#define PAD_BTN_PRESSED(pad)  (*key_now[(pad)] == 1)
#define PAD_BTN_REPEAT(pad)   PAD_REPEAT(*key_now[(pad)])
#define PAD_BTN_HELD(pad)     (*key_now[(pad)] != 0)

#define PAD_LANA_PRESSED(dir) (Ana2PadDirCnt((dir)) == 1)
#define PAD_LANA_REPEAT(dir)  PAD_REPEAT(Ana2PadDirCnt((dir)))
#define PAD_LANA_HELD(dir)    (Ana2PadDirCnt((dir)) != 0)


#endif /* TYPEDEFS_H */
