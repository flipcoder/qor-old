#ifndef _ISTATE_H
#define _ISTATE_H

#include "IRealtime.h"
#include "IRenderable.h"
#include "IFallible.h"

class IState: public IRealtime, public IRenderable, public IFallible
{
    public:
        virtual ~IState() {}
};

#endif

