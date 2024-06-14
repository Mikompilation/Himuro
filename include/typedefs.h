#ifndef TYPEDEFS_H 
#define TYPEDEFS_H

typedef long s_long;
typedef short s_short;
typedef char s_char;
typedef int s_int;
typedef int s_long128 __attribute__ ((mode (TI)));

typedef unsigned long u_long;
typedef unsigned short u_short;
typedef unsigned char u_char;
typedef unsigned int u_int;
typedef unsigned int u_long128 __attribute__ ((mode (TI)));

typedef int qword[4] 	__attribute__ ((aligned(16)));
typedef int sceVu0IVECTOR[4] __attribute__((aligned (16)));
typedef float sceVu0FVECTOR[4] __attribute__((aligned (16)));
typedef float sceVu0FMATRIX[4][4] __attribute__((aligned (16)));

#endif /* TYPEDEFS_H */
