#ifndef GRAPHICS_GRAPH2D_NUMBER_H
#define GRAPHICS_GRAPH2D_NUMBER_H

#include "typedefs.h"

// extern SPRT_DAT number_tex[0][10];

void NumberDisp(int number, short int pos_x, short int pos_y, u_char font, u_char r, u_char g, u_char b, u_char alpha, int pri, int digit, int type);
void NumberFontDisp(u_char number, u_char no, short int pos_x, short int pos_y, u_char font, u_char r, u_char g, u_char b, int pri, int alpha);

#endif // GRAPHICS_GRAPH2D_NUMBER_H
