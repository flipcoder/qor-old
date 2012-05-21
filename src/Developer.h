#ifndef _DEVELOPER_H
#define _DEVELOPER_H

class Engine;

#include "Input.h"
#include "IRealtime.h"
#include "IRenderable.h"
#include "Node.h"

class Developer : public IRealtime, public IRenderable
{
    private:

        Input* m_pInput; //weak
        std::weak_ptr<Node> m_wpMovable;

    public:
        Developer(Input* input);
        virtual ~Developer();

        virtual void logic(unsigned int a);
        virtual void render() const;

        void setMovable(std::shared_ptr<Node>& movable) {
            m_wpMovable = movable;
        }
};

#endif

