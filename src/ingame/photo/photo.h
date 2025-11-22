#ifndef INGAME_PHOTO_PHOTO_H
#define INGAME_PHOTO_PHOTO_H

#include "typedefs.h"
#include "photo_types.h"

#include "ingame/menu/ig_rank.h"

long int PhotoScoreCount();
int SubjectScoreCount(SUBJECT_WRK *sj);
int SpecialPhotoMakeJudge();
void AddPhotoData();
int GetSavePhotoNo();
void DeletePhotoData(u_char no);
void GetRecordSubject(PICTURE_WRK *pic);

#endif // INGAME_PHOTO_PHOTO_H
