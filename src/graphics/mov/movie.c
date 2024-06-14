#include "common.h"

INCLUDE_ASM(const s32, "graphics/mov/movie", MovieInitWrk);

INCLUDE_ASM(const s32, "graphics/mov/movie", ReqMpegEvent);

INCLUDE_ASM(const s32, "graphics/mov/movie", PlayMpegEvent);

INCLUDE_ASM(const s32, "graphics/mov/movie", movie);

INCLUDE_ASM(const s32, "graphics/mov/movie", switchThread);

INCLUDE_ASM(const s32, "graphics/mov/movie", readMpeg);

INCLUDE_ASM(const s32, "graphics/mov/movie", isAudioOK);

INCLUDE_ASM(const s32, "graphics/mov/movie", initMov);

INCLUDE_ASM(const s32, "graphics/mov/movie", termMov);

INCLUDE_ASM(const s32, "graphics/mov/movie", defMain);

void ErrMessage(void) {
}

void usage(void) {
}

INCLUDE_ASM(const s32, "graphics/mov/movie", proceedAudio);

INCLUDE_ASM(const s32, "graphics/mov/movie", MoviePlay);

INCLUDE_ASM(const s32, "graphics/mov/movie", MovieTest);

INCLUDE_ASM(const s32, "graphics/mov/movie", PadSyncCallback2);

INCLUDE_ASM(const s32, "graphics/mov/movie", movVblankPad);

INCLUDE_ASM(const s32, "graphics/mov/movie", ReqLogoMovie);

INCLUDE_ASM(const s32, "graphics/mov/movie", audioDecCreate);

INCLUDE_ASM(const s32, "graphics/mov/movie", audioDecDelete);

INCLUDE_ASM(const s32, "graphics/mov/movie", audioDecPause);

INCLUDE_ASM(const s32, "graphics/mov/movie", audioDecResume);

INCLUDE_ASM(const s32, "graphics/mov/movie", audioDecStart);

INCLUDE_ASM(const s32, "graphics/mov/movie", audioDecReset);

INCLUDE_ASM(const s32, "graphics/mov/movie", audioDecBeginPut);

INCLUDE_ASM(const s32, "graphics/mov/movie", audioDecEndPut);

INCLUDE_ASM(const s32, "graphics/mov/movie", audioDecIsPreset);

INCLUDE_ASM(const s32, "graphics/mov/movie", audioDecSendToIOP);

INCLUDE_ASM(const s32, "graphics/mov/movie", iopGetArea);

INCLUDE_ASM(const s32, "graphics/mov/movie", sendToIOP2area);

INCLUDE_ASM(const s32, "graphics/mov/movie", sendToIOP);

INCLUDE_ASM(const s32, "graphics/mov/movie", changeMasterVolume);

INCLUDE_ASM(const s32, "graphics/mov/movie", changeInputVolume);

INCLUDE_ASM(const s32, "graphics/mov/movie", clearGsMem);

INCLUDE_ASM(const s32, "graphics/mov/movie", setImageTag);

INCLUDE_ASM(const s32, "graphics/mov/movie", vblankHandlerM);

INCLUDE_ASM(const s32, "graphics/mov/movie", handler_endimage);

INCLUDE_ASM(const s32, "graphics/mov/movie", startDisplay);

INCLUDE_ASM(const s32, "graphics/mov/movie", endDisplay);

INCLUDE_ASM(const s32, "graphics/mov/movie", videoCallback);

INCLUDE_ASM(const s32, "graphics/mov/movie", pcmCallback);

INCLUDE_ASM(const s32, "graphics/mov/movie", copy2area);

INCLUDE_ASM(const s32, "graphics/mov/movie", readBufCreate);

void readBufDelete(void) {
}

INCLUDE_ASM(const s32, "graphics/mov/movie", readBufBeginPut);

INCLUDE_ASM(const s32, "graphics/mov/movie", readBufEndPut);

INCLUDE_ASM(const s32, "graphics/mov/movie", readBufBeginGet);

INCLUDE_ASM(const s32, "graphics/mov/movie", readBufEndGet);

INCLUDE_ASM(const s32, "graphics/mov/movie", strFileOpen);

INCLUDE_ASM(const s32, "graphics/mov/movie", strFileClose);

INCLUDE_ASM(const s32, "graphics/mov/movie", strFileRead);

INCLUDE_ASM(const s32, "graphics/mov/movie", getFIFOindex);

INCLUDE_ASM(const s32, "graphics/mov/movie", setD3_CHCR);

INCLUDE_ASM(const s32, "graphics/mov/movie", setD4_CHCR);

INCLUDE_ASM(const s32, "graphics/mov/movie", scTag2);

INCLUDE_ASM(const s32, "graphics/mov/movie", viBufCreate);

INCLUDE_ASM(const s32, "graphics/mov/movie", viBufReset);

INCLUDE_ASM(const s32, "graphics/mov/movie", viBufBeginPut);

INCLUDE_ASM(const s32, "graphics/mov/movie", viBufEndPut);

INCLUDE_ASM(const s32, "graphics/mov/movie", viBufAddDMA);

INCLUDE_ASM(const s32, "graphics/mov/movie", viBufStopDMA);

INCLUDE_ASM(const s32, "graphics/mov/movie", viBufRestartDMA);

INCLUDE_ASM(const s32, "graphics/mov/movie", viBufDelete);

INCLUDE_ASM(const s32, "graphics/mov/movie", viBufIsActive);

INCLUDE_ASM(const s32, "graphics/mov/movie", viBufCount);

INCLUDE_ASM(const s32, "graphics/mov/movie", viBufFlush);

INCLUDE_ASM(const s32, "graphics/mov/movie", viBufModifyPts);

INCLUDE_ASM(const s32, "graphics/mov/movie", viBufPutTs);

INCLUDE_ASM(const s32, "graphics/mov/movie", viBufGetTs);

INCLUDE_ASM(const s32, "graphics/mov/movie", videoDecCreate);

INCLUDE_ASM(const s32, "graphics/mov/movie", videoDecSetDecodeMode);

INCLUDE_ASM(const s32, "graphics/mov/movie", videoDecSetStream);

INCLUDE_ASM(const s32, "graphics/mov/movie", videoDecBeginPut);

INCLUDE_ASM(const s32, "graphics/mov/movie", videoDecEndPut);

INCLUDE_ASM(const s32, "graphics/mov/movie", videoDecReset);

INCLUDE_ASM(const s32, "graphics/mov/movie", videoDecDelete);

INCLUDE_ASM(const s32, "graphics/mov/movie", videoDecAbort);

INCLUDE_ASM(const s32, "graphics/mov/movie", videoDecGetState);

INCLUDE_ASM(const s32, "graphics/mov/movie", videoDecSetState);

INCLUDE_ASM(const s32, "graphics/mov/movie", videoDecPutTs);

INCLUDE_ASM(const s32, "graphics/mov/movie", videoDecInputCount);

INCLUDE_ASM(const s32, "graphics/mov/movie", videoDecInputSpaceCount);

INCLUDE_ASM(const s32, "graphics/mov/movie", videoDecFlush);

INCLUDE_ASM(const s32, "graphics/mov/movie", videoDecIsFlushed);

INCLUDE_ASM(const s32, "graphics/mov/movie", videoDecMain);

INCLUDE_ASM(const s32, "graphics/mov/movie", decBs0);

INCLUDE_ASM(const s32, "graphics/mov/movie", mpegError);

INCLUDE_ASM(const s32, "graphics/mov/movie", mpegNodata);

INCLUDE_ASM(const s32, "graphics/mov/movie", mpegStopDMA);

INCLUDE_ASM(const s32, "graphics/mov/movie", mpegRestartDMA);

INCLUDE_ASM(const s32, "graphics/mov/movie", mpegTS);

INCLUDE_ASM(const s32, "graphics/mov/movie", cpy2area);

INCLUDE_ASM(const s32, "graphics/mov/movie", voBufCreate);

void voBufDelete(void) {
}

INCLUDE_ASM(const s32, "graphics/mov/movie", voBufReset);

INCLUDE_ASM(const s32, "graphics/mov/movie", voBufIsFull);

INCLUDE_ASM(const s32, "graphics/mov/movie", voBufIncCount);

INCLUDE_ASM(const s32, "graphics/mov/movie", voBufGetData);

INCLUDE_ASM(const s32, "graphics/mov/movie", voBufIsEmpty);

INCLUDE_ASM(const s32, "graphics/mov/movie", voBufGetTag);

INCLUDE_ASM(const s32, "graphics/mov/movie", voBufDecCount);
