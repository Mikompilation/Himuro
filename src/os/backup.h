#ifndef OS_BACKUP_H
#define OS_BACKUP_H

#include "typedefs.h"

void SaveDataSet(u_char *addr0);
void SavePCFile();
void LoadPCFile();
void LoadDataDevelop(u_char *addr0);

#endif // OS_BACKUP_H
