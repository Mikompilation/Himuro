#ifndef GRAPHICS_GRAPH3D_LOAD3D_H
#define GRAPHICS_GRAPH3D_LOAD3D_H

#include "typedefs.h"

typedef struct {
	u_int *pk2;
	u_int *near_sgd;
	u_int *far_sgd;
	u_int *ss_sgd;
	u_int *sh_sgd;
	u_int *lit_data;
	u_int room_id;
} RMDL_ADDR;

typedef struct {
	u_short furn_id[30];
	u_short door_id[16];
	u_int *door_addr[16];
	u_int *load_addr;
	u_int furn_num;
	u_int door_num;
	u_int room_file_no;
	short int block_no;
	u_char room_no;
	u_char chapter_no;
	u_char sereq_type;
} ROOM_LOAD_BLOCK;

// extern ROOM_LOAD_BLOCK room_load_block[2];
// extern int r022_light_on;
// extern RMDL_ADDR room_addr_tbl[64];
// extern u_int *furn_addr_tbl[500];
// extern u_int *door_addr_tbl[128];
// extern u_int *item_addr_tbl[48];

u_int* RoomMdlLoadReq(u_int *addr, u_char blk_no, u_char msn_no, u_char room_no, u_char sereq_type);
int RoomMdlLoadWait();
void SetRenewDoorAddr(u_char blk_no);
void SetRenewDoorAddrForMovie();
void InitModelLoad();
void DelSameMdlID(u_short *mdl_id, int *mdl_num);
void SetPreRenderR022(int flg);
void LoadItemModel(u_int *tmpp);
void ItemLoadAfterInit(int item_no, int addr);
int InitializeRoomUnder(RMDL_ADDR *room_tbl);
int LoadInitFurnModel();
int LoadInitDoorModel();

#endif // GRAPHICS_GRAPH3D_LOAD3D_H
