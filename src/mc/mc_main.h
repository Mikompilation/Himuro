#ifndef MC_MC_MAIN_H
#define MC_MC_MAIN_H

#include "typedefs.h"
#include "sce/libmc.h"


typedef struct {
	char sync;
	char sta;
	char card_sta;
	char func_no;
	char retry_cnt;
	char req;
	char step;
} MC_ACCESS;

typedef struct {
	int type;
	int free;
	int format;
} MC_STATUS;

typedef struct {
	sceMcTblGetDir table[18];
	short int file_num;
	short int gamefile_num;
	u_int size;
} MC_DIR;

typedef struct {
	char path[128];
	char dir[128];
	char name[128];
	char *buf;
	int fd;
	int size;
	int mode;
	int step;
	int header_flg;
} MC_RW;

typedef struct {
	u_char file_no[5];
	u_char sta[5];
	char file_num;
	char type;
} MC_ALBUM;

typedef struct {
	sceMcIconSys icon;
	MC_DIR dir;
	MC_ACCESS acs;
	MC_STATUS card[2];
	MC_RW rw;
	MC_ALBUM album;
	int port;
	int slot;
	int port_mes;
	int mode;
	int step;
	int stepbak;
	int sub_step;
	int msg_no;
	int now_cur;
	int sel_cur;
	int sel_file;
	u_int dir_size;
	u_int *work_addr;
#ifdef BUILD_EU_VERSION
	int cnt;
#endif
} MC_CTRL;

typedef struct {
	u_int map_flg;
	u_int msn_no;
	u_int room_no;
	u_int hour;
	u_int minute;
	u_int sec;
	u_int frame;
#ifdef BUILD_EU_VERSION
	u_char msn_flg;
	u_char language;
#else
	u_short msn_flg;
#endif
	u_char clear_flg;
	u_char difficult;
} MC_GAME_HEADER;

typedef struct {
	u_int map_flg;
	u_int photo_num;
	u_int type;
	u_int pad[5];
} MC_ALBUM_HEADER;

typedef union {
	MC_GAME_HEADER game;
	MC_ALBUM_HEADER album;
} MC_HEADER;

extern MC_CTRL mc_ctrl;
extern MC_HEADER mc_header;
// extern MC_HEADER mc_header_buf[16];
extern u_int mc_game_size;
// extern u_int mc_album_size;
extern u_char mc_start_flg;
extern u_char mc_msn_flg;
extern u_char mc_newalbum_flg;
extern u_int mc_album_type;
extern u_int mc_photo_num;

#ifdef BUILD_EU_VERSION
extern u_char mc_language;
// extern short int mc_check_timer;
#endif

char mcSaveMain();
char mcLoadMain();
char mcCtrlCheck();
char mcCtrlSelSlot();
char mcCtrlFileSel();
char mcCtrlLoad();
char mcCtrlSave();
char mcCtrlMakeDir();
char mcCtrlFormat();
char mcCtrlDelete();
char mcCtrlDispErr();
char mcCtrlYesno(int def_pos);
char mcCtrl3taku(int def_pos);
void mcInit(u_char mode, u_int *work_addr, u_char msn_flg);
void mcSubInit();
void mcClearHeader();
void mcEnd();
void mcSetStep(int step, int msg_no);
void mcSetStepAfterCheck();

#ifdef BUILD_EU_VERSION
void mcFormatEndInit();
int mcFormatEnd();
#endif

#endif // MC_MC_MAIN_H
