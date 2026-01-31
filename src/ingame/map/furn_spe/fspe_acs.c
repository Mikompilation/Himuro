#include "common.h"
#include "typedefs.h"
#include "furn_spe.h"

 #define BASE_ADDRESS 0x7f8000

void FSpeMapDataMapping()
{
    u_int *addr_top;
    u_int *addr_data;
    int i;
    u_short data_num;

    addr_top = (u_int *)(BASE_ADDRESS + 4 * 4);
    addr_top = (u_int *)(*addr_top + BASE_ADDRESS);

    addr_data = (u_int *)(*addr_top + BASE_ADDRESS);

    data_num = ((u_int)addr_data - (u_int)addr_top) / 4;

    addr_data = addr_top;

    for (i = 0; i < data_num; i++)
    {
        *addr_data += BASE_ADDRESS;

        addr_data++;
    }
}

u_char* FSpeGetTopAddr(u_short fact_no)
{
    u_int *addr;

    if (fact_no == 0xffff)
    {
        return 0;
    }

    addr = (u_int *)(BASE_ADDRESS + 4 * 4);
    addr = (u_int *)(*addr + BASE_ADDRESS);

    return (u_char *)addr[fact_no];
}
