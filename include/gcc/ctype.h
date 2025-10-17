/*
 *  This header takes the place of the original ctype.h which, along with
 *  other gcc's header, is missing since we are using a barebone gcc environment.
 */
#ifndef GCC_CTYPE_H
#define GCC_CTYPE_H

#define __IMPORT
#define _CONST const
extern __IMPORT _CONST char    _ctype_[];
#define _L 02
#define islower(c) ((_ctype_+1)[(unsigned)(c)]&_L)
#ifdef __GNUC__
# define toupper(c) \
    __extension__ ({ int __x = (c); islower(__x) ? (__x - 'a' + 'A') : __x;})
# define tolower(c) \
    __extension__ ({ int __x = (c); isupper(__x) ? (__x - 'A' + 'a') : __x;})
#endif

#endif // GCC_CTYPE_H
