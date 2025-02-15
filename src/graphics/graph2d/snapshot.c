#include "common.h"
#include "typedefs.h"
#include "snapshot.h"

#include "sce/sifdev.h"

typedef struct {
	u_char bfType[2];
} BITMAPFILEHEADER1;

typedef struct {
	u_int bfSize;
	u_short bfReserved1;
	u_short bfReserved2;
	u_int bfOffbits;
} BITMAPFILEHEADER2;

typedef struct {
	u_int biSize;
	u_int biWidth;
	u_int biHeight;
	u_short biPlanes;
	u_short biBitCount;
	u_int biCompression;
	u_int biSizeImage;
	u_int biXPelsPerMeter;
	u_int biYPelsPerMeter;
	u_int biClrUsed;
	u_int biClrImportant;
} BITMAPINFOHEADER;

#define BI_RGB 0x0000

static BITMAPFILEHEADER1 bfhp1 = {
    .bfType = {'B', 'M'},
};
static int now_no[2] = {0, 0};

#define SCREEN_WIDTH   640
#define SCREEN_HEIGHT  224

void TakeSnapshot(char *data_i, char *data_o, int size_w, int size_h, int type)
{
	int bx;
	int by;
	int x;
	int y;
	int oneli;
	int onelo;
	int fd;
	char fname[256];
	int k;
	int nBytes;
	int offset;
	int tex_size;
	int bitc;
	BITMAPFILEHEADER2 bfhp2;
	BITMAPINFOHEADER bihp;
    int ch; // not in STAB

    ch = 3;
    tex_size = size_w * size_h * ch;
    bitc = 24;

    nBytes = tex_size + sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER1) + sizeof(BITMAPFILEHEADER2);
    offset = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER1) + sizeof(BITMAPFILEHEADER2);

    bx = (-size_w / 2) + (SCREEN_WIDTH / 2);
    by = (-size_h / 2) + (SCREEN_HEIGHT / 2);

    oneli = 2560;
    onelo = (((size_w - 1) / 64) + 1) * 192;

    bfhp2.bfSize = nBytes;
    bfhp2.bfReserved1 = 0;
    bfhp2.bfReserved2 = 0;
    bfhp2.bfOffbits = offset;

    bihp.biSize = sizeof(BITMAPINFOHEADER);
    bihp.biWidth = size_w;
    bihp.biHeight = size_h;
    bihp.biPlanes = 1;
    bihp.biBitCount = bitc;
    bihp.biCompression = BI_RGB;
    bihp.biSizeImage = 0; // 0 for BI_RGB bitmaps
    bihp.biXPelsPerMeter = 3780;  // 96dpi equivalent
    bihp.biYPelsPerMeter = 3780;
    bihp.biClrUsed = 0;
    bihp.biClrImportant = 0;

    for (y = 0; y < size_h; y++)
    {
        k = (size_h - y) - 1;

        for (x = 0; x < size_w; x++)
        {
            (data_o + (x * 3) + (k * onelo))[0] = (data_i + ((bx + x) * 4) + ((by + y) * oneli))[2];
            (data_o + (x * 3) + (k * onelo))[1] = (data_i + ((bx + x) * 4) + ((by + y) * oneli))[1];
            (data_o + (x * 3) + (k * onelo))[2] = (data_i + ((bx + x) * 4) + ((by + y) * oneli))[0];
        }
    }

    sprintf(fname, "host:snapshot%d%04d.bmp", type % 2 + 1, now_no[type % 2]);

    fd = sceOpen(fname, SCE_CREAT | SCE_WRONLY);

    if (fd != -1)
    {
        sceWrite(fd, &bfhp1, sizeof(BITMAPFILEHEADER1));
        sceWrite(fd, &bfhp2, sizeof(BITMAPFILEHEADER2));
        sceWrite(fd, &bihp, sizeof(bihp));
        sceWrite(fd, data_o, tex_size);
        sceClose(fd);

        now_no[type % 2]++;
    }
}
