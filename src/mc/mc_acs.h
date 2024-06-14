#ifndef MC_MC_ACS_H
#define MC_MC_ACS_H

#include "typedefs.h"

// extern char (*mc_acs[0])(/* parameters unknown */);

char mcAcsMain();
void mcAcsReq(int func_no);
void mcAcsEnd();
char mcSlotCheck();
char mcFileCheck();
char mcOpen();
char mcClose();
char mcLoad();
char mcSave();
char mcExistCheck();
char mcMakeDir();
char mcFormat();
char mcDelete();
char mcSyncNB();
void mcSyncErrWork(int cmd, int result);

#endif // MC_MC_ACS_H
