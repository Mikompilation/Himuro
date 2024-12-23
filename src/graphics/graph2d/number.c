#include "common.h"
#include "typedefs.h"
#include "number.h"

#include "outgame/mode_slct.h"
#include "graphics/graph2d/sprt.h"

// #include "graphics/graph2d/message.h" // DO NOT IMPORT!!
// #include "graphics/graph2d/tim2.h" // DO NOT IMPORT!!

#include "data/number_tex.h" // SPRT_DAT number_tex[][10];

void NumberDisp(int number, short int pos_x, short int pos_y, u_char font, u_char r, u_char g, u_char b, u_char alpha, int pri, int digit, int type)
{
	int i;
	int multi10;
	int multi10_bak;
	int disp_number;
	int disp_digit;

    multi10 = 1;

    for (i = 0; i < digit; i++)
    {
        multi10 *= 10;
        multi10_bak = multi10;
    }

    disp_number = number % multi10;

    if (disp_number != 0)
    {
        for (i = 0; i < digit; i++)
        {
            multi10 /= 10;

            if (disp_number / multi10 != 0)
            {
                disp_digit = digit - i;
                break;
            }
        }
    }
    else
    {
        disp_digit = 1;
    }

    multi10 = multi10_bak;

    for (i = 0; i < digit; i++)
    {
        multi10 /= 10;

        switch (type)
        {
        case 0:
            if (digit - disp_digit <= i)
            {
                NumberFontDisp(disp_number / multi10, i, pos_x, pos_y, font, r, g, b, pri, alpha);
                disp_number -= (disp_number / multi10) * multi10;
            }
        break;
        case 1:
            if (digit - disp_digit <= i)
            {
                NumberFontDisp(disp_number / multi10, i - (digit - disp_digit), pos_x, pos_y, font, r, g, b, pri, alpha);
                disp_number -= (disp_number / multi10) * multi10;
            }
        break;
        case 2:
            NumberFontDisp(disp_number / multi10, i, pos_x, pos_y, font, r, g, b, pri, alpha);
            disp_number -= (disp_number / multi10) * multi10;
        break;
        }
    }
}

void NumberFontDisp(u_char number, u_char no, short int pos_x, short int pos_y, u_char font, u_char r, u_char g, u_char b, int pri, u_char alpha)
{
	u_short font_w[5] = { 0x18, 0x15, 0x18, 0x18, 0x18 };
	DISP_SPRT ds;

    if (font == 0 || font == 3)
    {
        SetInteger3(pri, font_w[font] * no + pos_x, pos_y, 0, r, g, b, alpha, number);
    }
    else
    {
        CopySprDToSpr(&ds, &number_tex[font][number]);

        ds.x = font_w[font] * no + pos_x;
        ds.y = pos_y;
        ds.pri = pri;
        ds.r = r;
        ds.g = g;
        ds.b = b;
        ds.z = 0xfffffff - pri;
        ds.alpha = alpha;

        DispSprD(&ds);
    }
}
