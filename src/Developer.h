#ifndef _DEVELOPER_H
#define _DEVELOPER_H

class Engine;

#include "Input.h"
#include "IRealtime.h"
#include "IRenderable.h"

class Developer : public IRealtime, public IRenderable
{
    private:

        Input* m_pInput; //weak

    public:
        Developer(Input* input);
        virtual ~Developer();

        virtual void logic(unsigned int a);
        virtual void render() const;
};

#endif

