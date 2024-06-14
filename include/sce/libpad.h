#ifndef SCE_LIBPAD_H
#define SCE_LIBPAD_H

#define scePadDmaBufferMax	(16)

int scePadPortOpen(int port, int slot, void* addr);
int scePadInit(int mode);
int scePadGetState(int port, int slot);
int scePadRead(int port, int slot, unsigned char* rdata);
int scePadGetState(int port, int slot);
int scePadInfoMode(int port, int slot, int term, int offs);
int scePadSetMainMode(int port, int slot, int offs, int lock);
int scePadInfoAct(int port, int slot, int actno, int term);
int scePadSetActAlign(int port, int slot, const unsigned char* data);
int scePadGetReqState(int port, int slot);
int scePadInfoPressMode(int port, int slot);
int scePadEnterPressMode(int port, int slot);
int scePadSetActDirect(int port, int slot, const unsigned char* data);

#define scePadStateDiscon	    (0)
#define scePadStateFindPad	    (1)
#define scePadStateFindCTP1	    (2)
#define scePadStateExecCmd	    (5)
#define scePadStateStable	    (6)
#define scePadStateError	    (7)
#define scePadStateClosed	    (99)

#define scePadReqStateComplete	(0)
#define scePadReqStateFaild	    (1)
#define scePadReqStateFailed	(1)
#define scePadReqStateBusy	    (2)

#define InfoModeCurID		    (1)
#define InfoModeCurExID		    (2)
#define InfoModeCurExOffs	    (3)
#define InfoModeIdTable		    (4)

#endif // SCE_LIBPAD_H