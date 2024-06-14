#ifndef MC_MC_DISP_H
#define MC_MC_DISP_H

#include "typedefs.h"

void mcDispMessage();
void mcSetMessage(int msg_no);
char mcCheckMsgType(int msg_no);
char mcCheckDrawButton(int msg_no);

#endif // MC_MC_DISP_H
