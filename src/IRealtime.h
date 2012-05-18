#ifndef _IREALTIME_H
#define _IREALTIME_H

#include "IFallible.h"

class IRealtime
{
    public:
        virtual ~IRealtime() {}
        virtual void logic(unsigned int t) = 0;
        //virtual bool logic(unsigned int advance) {return logic(advance*0.001f);}
        //virtual bool logic(unsigned int advance) {return false;}
};

#endif

