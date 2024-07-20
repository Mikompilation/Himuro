/*
 *  This header takes the place of the original stdarg.h which, along with
 *  other gcc's header, is missing since we are using a barebone gcc environment.
 */
#ifndef GCC_STDARG_H
#define GCC_STDARG_H

#define _STDARG_H

#include "va-mips.h"

typedef __gnuc_va_list va_list;

#endif // GCC_STDARG_H
