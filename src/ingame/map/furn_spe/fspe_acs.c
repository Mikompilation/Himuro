#include "common.h"
#include "typedefs.h"
#include "addresses.h"
#include "furn_spe.h"

void FSpeMapDataMapping()
{
    u_int *addr_top;
    u_int *addr_data;
    int i;
    u_short data_num;

    addr_top = (u_int *)(LOAD_ADDRESS_02 + 4 * 4);
    addr_top = (u_int *)(*addr_top + LOAD_ADDRESS_02);

    addr_data = (u_int *)(*addr_top + LOAD_ADDRESS_02);

    data_num = ((u_int)addr_data - (u_int)addr_top) / 4;

    addr_data = addr_top;

    for (i = 0; i < data_num; i++)
    {
        *addr_data += LOAD_ADDRESS_02;

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

    addr = (u_int *)(LOAD_ADDRESS_02 + 4 * 4);
    addr = (u_int *)(*addr + LOAD_ADDRESS_02);

    return (u_char *)addr[fact_no];
}
