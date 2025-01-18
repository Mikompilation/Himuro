#include "common.h"
#include "typedefs.h"
#include "object.h"

#include "graphics/graph3d/sglib.h"

qword *pstart1 = NULL;
qword *pstart3 = NULL;

void objInit()
{
	static int bufid = 0;
	static qword p3_buf[2][256];
	qword *base3;

    pstart3 = p3_buf[bufid];
    bufid = 1 - bufid;
    pbase3 = pstart3;
    SgSetWorkBase(pbase3);
}

qword *pbase1 = NULL;
qword *pbase3 = NULL;

 void objFlush()
{
    return;
}
