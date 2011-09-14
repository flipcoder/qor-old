#include "pnUtil.h"
#include "pn.h"

// some handy conversion functions

pnUInt htonf(float f)
{
    pnUInt p;
    pnUInt sign;
    if (f < 0)
    {
        sign = 1;
        f = -f;
    }
    else
        sign = 0;
        
    p = ((((pnUInt)f)&0x7fff)<<16) | (sign<<31);
    p |= (pnUInt)(((f - (int)f) * 65536.0f))&0xffff;
    return p;
}

float ntohf(pnUInt p)
{
    float f = (float)((p>>16)&0x7fff);
    f += (p&0xffff) / 65536.0f;
    if (((p>>31)&0x1) == 0x1)
        f = -f;
    return f;
}
