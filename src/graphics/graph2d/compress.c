#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "compress.h"

#include "graphics/graph3d/sglib.h"

// most of the code in this file has been taken from the following sources:
// https://github.com/mattseabrook/LZSS/blob/main/1989/LZSS.C
// https://www-igm.univ-mlv.fr/~mac/ENS/DOC/COMP/BITIO.C
// https://www-igm.univ-mlv.fr/~mac/ENS/DOC/COMP/DCT.C

#define COLS      384
#define THRESHOLD	2   // encode string into position and length

#define PI 3.1415927f

// This macro is used to ensure correct rounding of integer values.
#define ROUND(a) (((a) < 0) ? (int)((a) - 0.5f) : (int)((a) + 0.5f))

typedef struct {
    char *file;
    u_char mask;
    int rack;
} BIT_FILE;

DCT_ROOT ZigZag[N * N] = 
{
    {0, 0},
    {0, 1}, {1, 0},
    {2, 0}, {1, 1}, {0, 2},
    {0, 3}, {1, 2}, {2, 1}, {3, 0},
    {4, 0}, {3, 1}, {2, 2}, {1, 3}, {0, 4},
    {0, 5}, {1, 4}, {2, 3}, {3, 2}, {4, 1}, {5, 0},
    {6, 0}, {5, 1}, {4, 2}, {3, 3}, {2, 4}, {1, 5}, {0, 6},
    {0, 7}, {1, 6}, {2, 5}, {3, 4}, {4, 3}, {5, 2}, {6, 1}, {7, 0},
    {7, 1}, {6, 2}, {5, 3}, {4, 4}, {3, 5}, {2, 6}, {1, 7},
    {2, 7}, {3, 6}, {4, 5}, {5, 4}, {6, 3}, {7, 2},
    {7, 3}, {6, 4}, {5, 5}, {4, 6}, {3, 7},
    {4, 7}, {5, 6}, {6, 5}, {7, 4},
    {7, 5}, {6, 6}, {5, 7},
    {6, 7}, {7, 6},
    {7, 7}
};
PHOTO_EXPAND photo_expand = {0};
short int dad[BUF_SIZE + 1] = {0};
short int lson[BUF_SIZE + 1] = {0};
short int rson[BUF_SIZE + 1 + 256] = {0};
u_char text[BUF_SIZE + F - 1] = {0};
short int matchpos = 0;
short int matchlen = 0;
static u_char pixelstrip[N][COLS];
static float C[N][N];
static float Ct[N][N];
static int InputRunLength;
static int OutputRunLength;
static int quantum[N][N];
static char repair_flg;

static void InitCompress(char quality);
static void ForwardDCT(u_char **input, int (*output)[N]);
static void InverseDCT(int (*input)[N], u_char **output);
static void WriteDCTData(BIT_FILE *output_file, int (*output_data)[N]);
static u_short *ReadPixelStrip(u_short *input, u_char (*strip)[COLS], u_char type);
static int InputCode(BIT_FILE *input_file);
static void ReadDCTData(BIT_FILE *input_file, int (*input_data)[N]);
static void OutputCode(BIT_FILE *output_file, int code);
static u_short *WritePixelStrip(u_short *output, u_char (*strip)[COLS], u_char type);
static void OutputBits(BIT_FILE *bit_file, u_int code, int count);
static int InputBits(BIT_FILE *bit_file, int bit_count);

float SlideEncode(u_char *base, u_char *addrs, int max_size)
{
    short int s;
    short int r;
    short int len;
    short int codeptr;
    short int i;
    short int lastmatchlen;
    u_char c;
    u_char code[17];
    u_char mask;
    u_int incount;
    u_int size;
    u_int *savep;
    int save_size;

    incount = 0;
    size = 0;
    savep = (u_int *)addrs;
    save_size = max_size;

    addrs += 16;
    
    init_tree();
    
    code[0] = 0;
    codeptr = mask = 1;
    s = 0;
    r = BUF_SIZE - F;
    
    for (i = s; i < r; i++)
    {
        text[i] = 0;
    }

    for (len = 0; len < F && max_size != 0; len++)
    {
        c = *base++;
        max_size--;
        text[r + len] = c;
    }

    if ((incount = len) == 0) return;
    
    for (i = 1; i <= F; i++)
    {
        insert_node(r - i);
    }
    
    insert_node(r);
    
    do
    {
        if (matchlen > len)
        {
            matchlen = len;
        }
        
        if (matchlen <= THRESHOLD)
        {
            matchlen = 1;
            code[0] |= mask;
            code[codeptr++] = text[r];
        }
        else
        {
            code[codeptr++] = matchpos;
            code[codeptr++] = ((matchpos >> 4) & 0xf0) | (matchlen - (THRESHOLD + 1));
        }
        
        if ((mask <<= 1) == 0)
        {
            for (i = 0; i < codeptr; i++)
            {
                *addrs++ = code[i];
                size++;
            }
            
            code[0] = 0;
            codeptr = mask = 1;
        }
        
        lastmatchlen = matchlen;
        
        for (i = 0; i < lastmatchlen && max_size != 0; i++)
        {
            c = *base++;
            max_size--;
            delete_node(s);
            text[s] = c;
            if (s < F - 1)
            {
                text[s + BUF_SIZE] = c;
            }

            s = (s + 1) & (BUF_SIZE - 1);
            r = (r + 1) & (BUF_SIZE - 1);

            insert_node(r);
        }

        incount += i;
        
        while (i++ < lastmatchlen)
        {   
            delete_node(s);
            s = (s + 1) & (BUF_SIZE - 1);
            r = (r + 1) & (BUF_SIZE - 1);
            if (--len)
            {
                insert_node(r);
            }
        }
    } while (len > 0);
    
    if (codeptr > 1)
    {
        for (i = 0; i < codeptr; i++)
        {
            *addrs++ = code[i];
            size++;
        }
    }
    
    if (incount && size/incount)
    {
        // debug code?
    }
    
    savep[0] = size;
    for (i = 0; i < 3; i++)
    {
        savep[i + 1] = 0;
    }

    return (float)size / save_size;
}

void SlideDecode(u_char *base, u_char *addrs)
{
    u_char textbuf[BUF_SIZE + F - 1];
    int i;
    int j;
    int k;
    int r;
    int c;
    u_short flags;
    u_int rest;
    
    rest = *(int *)base;
    base += 16;

    for (i = 0; i < BUF_SIZE - F; i++)
    {
        textbuf[i] = 0;
    }
    
    r = BUF_SIZE - F;
    flags = 0;
    
    while(1)
    {
        if (((flags >>= 1) & 0x100) == 0)
        {
            c = *base++;
            if (rest == 0)
            {
                break;
            }
            rest--;
            
            flags = c | 0xff00;
        }
        
        if (flags & 1)
        {
            c = *base++;
            if (rest == 0)
            {
                break;
            }
            rest--;       
            
            *addrs++ = c;
            
            textbuf[r++] = c;
            r &= (BUF_SIZE - 1);
        }
        else
        {
            i = *base++;
            if (rest == 0)
            {
                break;
            }
            rest--;
            
            j = *base++;
            if (rest == 0)
            {
                break;
            }
            rest--;
            
            i |= ((j & 0xf0) << 4);
            j = (j & 0x0f) + THRESHOLD;

            for (k = 0; k <= j; k++)
            {
                c = textbuf[(i + k) & (BUF_SIZE - 1)];
                *addrs++ = c;
                textbuf[r++] = c;
                r &= (BUF_SIZE - 1);
            }
        }
    }
}

void init_tree()
{
    short int i;
    
    for (i = BUF_SIZE + 1; i < BUF_SIZE + 1 + 256; i++)
    {
        rson[i] = BUF_SIZE;
    }
    
    for (i = 0; i < BUF_SIZE; i++)
    {
        dad[i] = BUF_SIZE;
    }
}

void insert_node(short int r)
{
    short int p;
    short int cmp;
    short int i;
    u_char *key;

	cmp = 1;
    key = &text[r];
    p = BUF_SIZE + 1 + key[0];
	rson[r] = lson[r] = BUF_SIZE;
    matchlen = 0;
    
	while (1)
    {
        if (cmp >= 0)
        {
			if (rson[p] != BUF_SIZE)
            {
                p = rson[p];
            }
			else
            {
                rson[p] = r;
                dad[r] = p;
                return;
            }
		}
        else
        {
			if (lson[p] != BUF_SIZE)
            {
                p = lson[p];
            }
			else
            {
                lson[p] = r;
                dad[r] = p;
                return;
            }
		}
        
		for (i = 1; i < F; i++)
        {
			if ((cmp = key[i] - text[p + i]) != 0)
            {
                break;
            }
        }
        
		if (i > matchlen)
        {
			matchpos = p;
            matchlen = i;
			if (matchlen >= F)
            {
                break;
            }
		}
	}
    
	dad[r] = dad[p];
    lson[r] = lson[p];
    rson[r] = rson[p];
	dad[lson[p]] = r;
    dad[rson[p]] = r;
    
	if (rson[dad[p]] == p)
    {
        rson[dad[p]] = r;
    }
	else
    {
        lson[dad[p]] = r;
    }
    
	dad[p] = BUF_SIZE;
}

void delete_node(short int p)
{
    short int q;
    
    if (dad[p] == BUF_SIZE)
    {
        return;  /* not in tree */
    }
    
	if (rson[p] == BUF_SIZE)
    {
        q = lson[p];
    }
	else if (lson[p] == BUF_SIZE)
    {
        q = rson[p];
    }
	else
    {
		q = lson[p];
        
		if (rson[q] != BUF_SIZE)
        {
			do
            {
                q = rson[q];
            } while (rson[q] != BUF_SIZE);
            
			rson[dad[q]] = lson[q];
            dad[lson[q]] = dad[q];
			lson[q] = lson[p];
            dad[lson[p]] = q;
		}
        
		rson[q] = rson[p];  dad[rson[p]] = q;
	}
    
	dad[q] = dad[p];
    
    if (rson[dad[p]] == p)
    {
        rson[dad[p]] = q;
    }
    else
    {
        lson[dad[p]] = q;
    }
	
    dad[p] = BUF_SIZE;
}

float CompressFile(u_short *input, char *output, u_int max_tmp, char quality)
{
    int row;
    int col;
    int i;
    int j;
    u_char *input_array[N];
    int output_array[N][N];
    BIT_FILE bit_file;
    u_int size;
    u_short *top;
    u_short *addr;
    u_int tmp;

    ((int*)output)[2] = 0;
    ((int*)output)[1] = 1;
    ((int*)output)[3] = 0;
    ((int*)output)[7] = quality;


    bit_file.file = (char*)&(((int*)output)[N]);
    bit_file.mask = 0x80;
    bit_file.rack = 0;
    
    InitCompress(quality);
    
    top = &input[N];
    
    for (i = 0; i < 3; i++)
    {
        OutputRunLength = 0;
        InputRunLength = 0;
        
        bit_file.mask = 0x80;
        bit_file.rack = 0;
        
        ((int *)output)[i + 4] = bit_file.file - output;

        addr = top;
        
        for (row = 0, addr = top; row < 128; row += N)
        {
            addr = ReadPixelStrip(addr, pixelstrip, i);

            for (col = 0; col < COLS; col += N)
            {                
                for (j = 0; j < N; j++)
                {
                    input_array[j] = &pixelstrip[j][col];
                }

                ForwardDCT(input_array, output_array);
                WriteDCTData(&bit_file, output_array);
            }
        }

        OutputCode(&bit_file, 1);

        bit_file.file++;
        tmp = (u_int)bit_file.file;
        
        if (((u_int)tmp & 0xf) != 0) 
        {
            tmp = (u_int)bit_file.file + (16 - ((u_int)tmp & 0xf));
        }

        bit_file.file = (char *)tmp;
    }

    size = tmp - (int)output;
    *(u_int *)output = size;
    
    return (float)size / max_tmp;
}


char CheckPhotoExpandEnd()
{
    return photo_expand.sta == 1;
}

void InitPhotoExpand()
{
    photo_expand.sta = 0;
    photo_expand.cnt = 0;
}

void ReqPhotoExpand()
{
    InitPhotoExpand();
}

void ExpandFile(char *input, u_short *output2)
{
    int row;
    int col;
    int i;
    int j;
    int input_array[N][N];
    u_char *(output_array[N]);
    u_int quality;
    static u_int *in_header;
    static u_int *out_header;
    static u_short *top;
    static BIT_FILE bit_file;
    static u_short *output;
    u_int end;
    
    if (photo_expand.sta == 1)
    {
        return;
    }

    if (photo_expand.cnt == 0)
    {
        ((u_int *)output2)[0] = ((u_int *)input)[0];
        ((u_int *)output2)[3] = 0;
        ((u_int *)output2)[2] = 0;
        ((u_int *)output2)[1] = 0;
        
        top = &output2[N];
        
        bit_file.file = (char *)&((u_int *)input)[N];
        bit_file.mask = 0x80;
        bit_file.rack = 0;
        
        quality = ((u_int *)input)[7];
        
        out_header = (u_int *)output2;
        output = output2;
        in_header = (u_int *)input;

        InitCompress(quality);
    }
    
    row = photo_expand.cnt;
    i = photo_expand.cnt >> 3;
    
    if (i < 3)
    {
        if ((row & 7) == 0)
        {
            output = top;
            
            bit_file.file = (char *)((int)in_header[i + 4] + (int)input);
            bit_file.mask = 0x80;
            bit_file.rack = 0;
            
            OutputRunLength = 0;
            InputRunLength = 0;
        }
        
        end = (row & 7) * 16 + 16;

        for (row = end - 16; row < end; row += N)
        {
            repair_flg = 0;
            
            if (row >= 120)
            {
                repair_flg = 1;
            }
            
            for (col = 0; col < COLS; col += N)
            {
                for (j = 0; j < N; j++)
                {
                    output_array[j] = pixelstrip[j] + col;
                }
                
                ReadDCTData(&bit_file, input_array);
                InverseDCT(input_array, output_array);
            }
            
            output = WritePixelStrip(output, pixelstrip, i);
        }
        
        photo_expand.cnt++;
    }
    
    if (photo_expand.cnt >= 24)
    {
        photo_expand.cnt = 0;
        photo_expand.sta = 1;
    }
}

static void InitCompress(char quality)
{
    u_int i;
    u_int j;
    float val;


    for (i = 0 ; i < N ; i++)
    {
        for (j = 0 ; j < N ; j++)
        {
            quantum[i][j] = (j * 2 / 3 + i * 2 / 3) * quality + quality + 1;
        }
    }
    
    val = 1.0f / SgSqrtf(N);

    for (i = 0 ; i < N ; i++)
    {
        C[0][i] = val;
        Ct[i][0] = C[0][i];
    }
    
    val = SgSqrtf(2.0f / N);
    
    for (i = 1 ; i < N ; i++)
    {
        for (j = 0 ; j < N ; j++)
        {
            C[i][j] = val * SgCosf(PI * (2 * j + 1) * i / (2.0f * N));
            Ct[j][i] = C[i][j];
        }
    }
}

static void ForwardDCT(u_char **input, int (*output)[N])
{
    float temp[N][N];
    float temp1;
    u_int i;
    u_int j;
    u_int k;

    // MatrixMultiply(temp, input, Ct);
    for (i = 0 ; i < N ; i++)
    {
        for (j = 0 ; j < N ; j++)
        {
            temp[i][j] = 0.0f;
            
            for (k = 0 ; k < N ; k++)
            {
                 temp[i][j] += (input[i][k] - 128) * Ct[k][j];
            }
        }
    }

    // MatrixMultiply(output, C, temp);
    for (i = 0 ; i < N ; i++)
    {
        for (j = 0 ; j < N ; j++)
        {
            temp1 = 0.0f;
            
            for (k = 0 ; k < N ; k++)
            {
                temp1 += C[i][k] * temp[k][j];
            }
            
            output[i][j] = ROUND(temp1);
        }
    }
}

static void InverseDCT(int (*input)[N], u_char **output)
{
    float temp[N][N];
    float temp1;
    u_int i;
    u_int j;
    u_int k;

    // MatrixMultiply(temp, input, C);
    for (i = 0 ; i < N ; i++)
    {
        for (j = 0 ; j < N ; j++)
        {
            temp[i][j] = 0.0f;
            
            for (k = 0 ; k < N ; k++)
            {
                temp[i][j] += input[i][k] * C[k][j];
            }
        }
    }

    // MatrixMultiply(output, Ct, temp);
    for (i = 0 ; i < N ; i++)
    {
        for (j = 0 ; j < N ; j++)
        {
            temp1 = 0.0f;
            
            for (k = 0 ; k < N ; k++)
            {
                temp1 += Ct[i][k] * temp[k][j];
            }
            
            temp1 += 128.0f;
            
            if (temp1 < 0)
            {
                output[i][j] = 0;
            }
            else if (temp1 > 255)
            {
                output[i][j] = 255;
            }
            else
            {
                output[i][j] = ROUND(temp1);
            }
        }
    }
}

static void WriteDCTData(BIT_FILE *output_file, int (*output_data)[N])
{
    int i;
    int row;
    int col;
    float result;

    for (i = 0; i < (N * N); i++)
    {
        row = ZigZag[i].row;
        col = ZigZag[i].col;
        result = (float)output_data[row][col] / quantum[row][col];
        
        OutputCode(output_file, ROUND(result));
    }
}

static u_short *ReadPixelStrip(u_short *input, u_char (*strip)[COLS], u_char type)
{
    int row;
    int col;
    u_short c;

    for (row = 0 ; row < N ; row++)
    {
        for (col = 0 ; col < COLS ; col++)
        {
            c = *input;
            
            if (type == 0)
            {
                strip[row][col] = (c / 1024) & 0x1f;
            }
            else if (type == 1)
            {
                strip[row][col] = (c / 32) & 0x1f;
            }
            else if (type == 2)
            {
                strip[row][col] = c & 0x1f;
            }
            
            if (strip[row][col] == 0)
            {
                strip[row][col] = 1;
            }
            
            input++;
        }
    }

    return input;
}

static int InputCode(BIT_FILE *input_file)
{
    int bit_count;
    int result;

    if (InputRunLength > 0)
    {
        InputRunLength--;
        return 0;
    }
    
    bit_count = InputBits(input_file, 2);
    
    if (bit_count == 0)
    {
        InputRunLength = InputBits(input_file, 4);
        return 0;
    }
    
    if (bit_count == 1)
    {
        bit_count = InputBits(input_file, 1) + 1;
    }
    else
    {
        bit_count = InputBits(input_file, 2) + (bit_count << 2) - 5;
    }
    
    result = InputBits(input_file, bit_count);
    
    if (result & (1 << (bit_count - 1)))
    {
        return result;
    }
    
    return result - (1 << bit_count) + 1;
}

static void ReadDCTData(BIT_FILE *input_file, int (*input_data)[N])
{
    int i;
    int row;
    int col;

    for (i = 0 ; i < (N * N) ; i++)
    {
        row = ZigZag[i].row;
        col = ZigZag[i].col;
        input_data[row][col] = InputCode(input_file) * quantum[row][col];
    }
}

static void OutputCode(BIT_FILE *output_file, int code)
{
    int top_of_range;
    int abs_code;
    int bit_count;

    if (code == 0)
    {
        OutputRunLength++;
        return;
    }
    
    if (OutputRunLength != 0)
    {
        while ( OutputRunLength > 0 )
        {
            OutputBits(output_file, 0, 2);
            if (OutputRunLength <= 16)
            {
                OutputBits(output_file, OutputRunLength - 1, 4);
                OutputRunLength = 0;
            }
            else
            {
                OutputBits(output_file, 15, 4);
                OutputRunLength -= 16;
            }
        }
    }
    
    if (code < 0)
    {
        abs_code = -code;
    }
    else
    {
        abs_code = code;
    }
    
    top_of_range = 1;
    bit_count = 1;
    
    while (abs_code > top_of_range)
    {
        bit_count++;
        top_of_range = (top_of_range + 1) * 2 - 1;
    }
    
    if (bit_count < 3)
    {
        OutputBits(output_file, (bit_count + 1), 3);
    }
    else
    {
        OutputBits(output_file, (bit_count + 5), 4);
    }
    
    if (code > 0)
    {
        OutputBits(output_file, code, bit_count);
    }
    else
    {
        OutputBits(output_file, (code + top_of_range), bit_count);
    }
}

static u_short *WritePixelStrip(u_short *output, u_char (*strip)[COLS], u_char type)
{
    int row;
    int col;

    for (row = 0 ; row < N ; row++)
    {
        for (col = 0 ; col < COLS ; col++)
        {
            if (strip[row][col] > 0x1f)
            {
                strip[row][col] = 0x1f;
            }
            
            if (type == 0)
            {
                *output = 0x8000;
                *output |= strip[row][col] << 10;
            }
            else if (type == 1)
            {
                *output |= (strip[row][col] & 0x1f) << 5;
            }
            else if (type == 2)
            {
                *output |= strip[row][col] & 0x1f;
            }
           
            output++;
        }
    }

    return output;
}

static void OutputBits(BIT_FILE *bit_file, u_int code, int count)
{
    u_int mask;

    mask = 1 << (count - 1);
    
    while (mask != 0)
    {
        if (mask & code)
        {
            bit_file->rack |= bit_file->mask;
        }
        
        bit_file->mask >>= 1;
        
        if ( bit_file->mask == 0 )
        {
            *bit_file->file = bit_file->rack;
            
            bit_file->file++;
    	    bit_file->rack = 0;
            bit_file->mask = 0x80;
        }
        
        mask >>= 1;
    }
}

static int InputBits(BIT_FILE *bit_file, int bit_count)
{
    u_int mask;
    int return_value;

    mask = 1 << (bit_count - 1);
    return_value = 0;
    
    while (mask != 0)
    {
    	if (bit_file->mask == 0x80)
        {
    	    bit_file->rack = *bit_file->file++;
    	}
        
    	if (bit_file->rack & bit_file->mask)
        {
            return_value |= mask;
        }
        
        mask >>= 1;
        bit_file->mask >>= 1;
        
        if (bit_file->mask == 0)
        {
            bit_file->mask = 0x80;
        }
    }
    
    return return_value;
}
