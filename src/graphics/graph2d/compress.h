#ifndef GRAPHICS_GRAPH2D_COMPRESS_H
#define GRAPHICS_GRAPH2D_COMPRESS_H

#include "typedefs.h"

typedef struct {
	u_char sta;
	u_char cnt;
} PHOTO_EXPAND;

typedef struct {
    char row;
    char col;
} DCT_ROOT;

#define N           8
#define BUF_SIZE 2048	// tmp of ring buffer
#define F		   18	// upper limit for match_length

extern DCT_ROOT ZigZag[N * N];
extern PHOTO_EXPAND photo_expand;
extern short int dad[BUF_SIZE + 1];
extern short int lson[BUF_SIZE + 1];
extern short int rson[BUF_SIZE + 1 + 256];
extern u_char text[BUF_SIZE + F - 1];
extern short int matchpos;
extern short int matchlen;

float SlideEncode(u_char *base, u_char *addrs, int max_size);
void SlideDecode(u_char *base, u_char *addrs);
void init_tree();
void insert_node(short int r);
void delete_node(short int p);
float CompressFile(u_short *input, char *output, u_int max_size, char quality);
char CheckPhotoExpandEnd();
void InitPhotoExpand();
void ReqPhotoExpand();
void ExpandFile(char *input, u_short *output2);

#endif // GRAPHICS_GRAPH2D_COMPRESS_H
