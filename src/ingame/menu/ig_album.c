#include "common.h"

#ifdef MATCHING_DECOMP
#define INCLUDING_FROM_IG_ALBUM_C
#include "ig_album.h"
#undef INCLUDING_FROM_IG_ALBUM_C
#else
#include "ig_album.h"
#endif

INCLUDE_ASM(const s32, "ingame/menu/ig_album", NewgameMenuAlbumInit);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", LoadgameMenuAlbumInit);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", StartAlbumModeInit);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", IngameMenuAlbum);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", IngameAlbumAllPad);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", IngameAlbumLstPad);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", IngameAlbumBigPad);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", IngameAlbumMenu);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", CmndMenuVrgn);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", CmndMenuClrB);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", IngameAlbumMenuDsp);

void IngameAlbumDispPhotoLarge(void) {
}

INCLUDE_ASM(const s32, "ingame/menu/ig_album", IngameAlbumDisp);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", DrawAlbum);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", PhotOnlyAll);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", PhotOnlyLst);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", PhotOnlyBig);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", GetSubjectNameAddr);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", IngameAlbumSort);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", DspMainMenu);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", DspSideMenu);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", DspMenuWin);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", DspDetail);

void DspNotice(void) {
}

INCLUDE_ASM(const s32, "ingame/menu/ig_album", DspSumnale);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", DspPhtLst);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", DspFilmCut);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", DspSideSmnl);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", DspBtmDtl);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", DspCherry);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", AlbmTrFlsh);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", TimeIsMoney);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", MoneyIsTime);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", PutDate);

void AlbmCntInit(void) {
}

INCLUDE_ASM(const s32, "ingame/menu/ig_album", AlbmCntRenew);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", AlbmModeInOut);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", AlbmModeInOut2);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", SealChk);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", OutGameInitPhoto);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", isPhotoEnd);

INCLUDE_ASM(const s32, "ingame/menu/ig_album", GetPhtDispAlpha);
