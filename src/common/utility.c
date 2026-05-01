#include "common.h"
#include "typedefs.h"
#include "enums.h"
#include "utility.h"

char *StrToLower(char *str)
{
    char *strp;

    strp = str;

    while (*strp != 0)
    {
        *strp = tolower(*strp);
        strp++;
    }

    return str;
}
