#ifndef OS_FILELOAD_H
#define OS_FILELOAD_H

#include "typedefs.h"

int FileLoadInit();
void FileLoadNReq(int file_no, long int addr);
int FileLoadNEnd(int file_no);
void FileLoadB(int file_no, long int addr);

#endif // OS_FILELOAD_H
