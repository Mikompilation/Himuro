#ifndef OS_EEIOP_EEIOP_H
#define OS_EEIOP_EEIOP_H

#include "typedefs.h"
#include "sce/sifrpc.h"

typedef struct
{ // 0x8
    /* 0x0 */ int file_no;
    /* 0x4 */ u_char stat;
    /* 0x5 */ u_char renew;
} CDVD_LOAD_STAT;


typedef struct {
	CDVD_LOAD_STAT fstat[32];
	int stat;
	u_int ld_addr;
	int se_trans;
} CDVD_RET_STAT;

typedef struct {
	int status;
} SE_VSTAT;

typedef struct {
	int thread_id;
	int stat;
} BGM_STAT;

typedef struct {
	short int status;
	short int tune_no;
	int count;
} ADPCM_STAT;

typedef struct {
	SE_VSTAT sev_stat[24];
	BGM_STAT bgm;
	CDVD_RET_STAT cdvd;
	ADPCM_STAT adpcm;
	int dummy[1];
} IOP_STAT;

typedef struct {
	int cmd_no;
	int data1;
	int data2;
	int data3;
	int data4;
	int data5;
	int data6;
	int data7;
} IOP_COMMAND;

typedef struct {
	sceSifClientData gcd;
	int cmd_num;
} EI_SYS;

extern EI_SYS ei_sys;
extern IOP_COMMAND send_cmd[32];
extern IOP_STAT rcv_stat;

void EiInit();
int EiMain();
void SetIopCmdSm(int cmd_no, int data1, int data2, int data3);
void SetIopCmdLg(int cmd_no, int data1, int data2, int data3, int data4, int data5, int data6, int data7);
int checkIOP();
IOP_STAT* GetIopStatP();

#endif // OS_EEIOP_EEIOP_H
