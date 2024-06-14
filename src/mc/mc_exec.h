#ifndef MC_MC_EXEC_H
#define MC_MC_EXEC_H

#include "typedefs.h"
#include "sce/libmc.h"

extern u_char save_pht_in_game;

void mcSetPathDir(char file_no);
void mcMakeGameFileName(char *fname, char file_id);
void mcMakeAlbumFileName(char *fname, char file_id);
void mcMakeIconsysFileName(char *fname);
void mcMakeIconDataFileName(char *fname, char file_id);
char mcCheckEmpty(u_int offset);
void mcAcsFileReq(char type, int mode, int header);
char mcWriteFile();
char mcReadFile();
char mcCheckFileList();
char mcCheckRootList();
u_int mcGetFileSizeCeil(u_int file_id, u_int block);
u_int mcGetFileSize(u_int file_id);
char mcCheckHeaderFile();
int strindex(char *s, char *t);
void mcMakeSaveFile(u_int *work_addr, u_char file_id);
char mcSetLoadFile(u_int *work_addr, u_char file_id);
char mcCompareCheckSum(u_char *data_ptr, int data_size);
void mcMakeHeaderFile();
void mcSetHeaderFile();
char mcGetAlbumNo();
void mcSetIconSys(sceMcIconSys *pSys);

#endif // MC_MC_EXEC_H
