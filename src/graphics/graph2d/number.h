#ifndef GRAPHICS_GRAPH2D_NUMBER_H
#define GRAPHICS_GRAPH2D_NUMBER_H

#include "typedefs.h"

#include "graphics/graph2d/sprt.h"

extern SPRT_DAT number_tex[][10];

void NumberDisp(int number, short int pos_x, short int pos_y, u_char font, u_char r, u_char g, u_char b, u_char alpha, int pri, int digit, int type);
void NumberFontDisp(u_char number, u_char no, short int pos_x, short int pos_y, u_char font, u_char r, u_char g, u_char b, int pri, u_char alpha);

#endif // GRAPHICS_GRAPH2D_NUMBER_H
