#ifndef INGAME_PHOTO_PHT_MAKE_H
#define INGAME_PHOTO_PHT_MAKE_H

#include "typedefs.h"
#include "graphics/graph2d/sprt.h"

extern SPRT_DAT hintdat[];
// extern SPRT_DAT hintreadat[];
extern int pazene_load_tbl[];
// extern int frm_type;
// extern int ers_type;
extern int hint_2d;
extern int hint_rea;
extern int hint_3d;
extern int hint_2d_num;
extern int hint_3d_num;
// extern int pic_save_flag;
extern int pht_finder_off;

void InitPhotoMake();
void CopyScreenToBuffer(int addr, int szfl, int mx, int my, int mw, int mh);
void DrawPhotoBuffer(u_int pri, int addr, int szfl, int x, int y, int szw, int szh, int mszw, int mszh, int ftype, int alp, int ztype);
void CompressData(int addri, int addro, int n);
void UncompressData(int addri, int n, int addro);
void TakePhotoFromScreen();
void MakeSPhotoFromWorkArea(int n);
void MakeSPhotoFromCompress(int ni, int no);
void DrawSPhotoFromSmallPhotoArea(int n, int pri, int ftype, int x, int y, u_char alp);
void DrawSPhotoFromSmallPhotoAreaAD(int addr, int n, int pri, int ftype, int x, int y, u_char alp);
void DrawSPhotoFromSmallPhotoArea2(int n, int pri, int ftype, int x, int y, int szw, int szh, u_char alp);
void DrawSPhotoFromSmallPhotoArea2AD(int addr, int n, int pri, int ftype, int x, int y, int szw, int szh, int alp);
void CompPhotoFromWorkArea(int n);
void DrawPhotoFromWorkArea(int pri, int ftype, int x, int y, int szw, int szh, u_char alp);
void DrawPhotoFromCompress(int n, u_int pri, int ftype, int x, int y, int szw, int szh, u_char alp);
void UncompressPhoto(int n);
void UncompressPhotoAD(int addr, int n);
void DrawPhotoFromPhotoWrk(int n, u_int pri, int ftype, int x, int y, int szw, int szh, u_char alp);
void CopyPhoto(int addri, int ni, int addro, int no);
void CopySPhoto(int addri, int ni, int addro, int no);
void DrawPhotoHinttex(u_int pri, int num);
void DrawPhotoHinttex2(u_int sw, u_int pri, int num);
void DrawPhotoEffect(u_int pri, int type);
void DrawPhotoFilter(u_int pri, int type, u_char alp);
void DispPhotoFrame0();
void DispPhotoFrame00(int flag);
int DispPhotoFrame1(int fl);
void DispPhotoFrame2();
void DispPhotoFrame2_2(int type);
void DispPhotoFrame2_3(int type);
void SetNegaFilter(int type);
void PhotoMake();
void PhotoMake_EneDead();
void PhotoMakeSaveInit();
void DrawPicture(int pri, int addr, int n, float x, float y, float szw, float szh, u_char alp);
void CopyPicture(int addri, int ni, int addro, int no);
void ReqSavePicture();
void PhotoMakeSave();
void PhotoMakeSave2();

#endif // INGAME_PHOTO_PHT_MAKE_H
