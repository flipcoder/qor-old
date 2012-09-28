#ifndef _IREALTIME_H
#define _IREALTIME_H

#include "Util.h"

class IRealtime
{
    public:
        virtual ~IRealtime() {}

        virtual void logic(unsigned int t) = 0;
        //virtual void logic(unsigned int t) { return logic(t * 1000.0f); };
        //virtual void logic(float t) { return logic(round_int(t * 0.001f)); }
        //virtual bool logic(unsigned int advance) {return logic(advance*0.001f);}
        //virtual bool logic(unsigned int advance) {return false;}
};

#endif

