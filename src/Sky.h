#ifndef _SKY_H
#define _SKY_H

#include "Node.h"

class Sky
    : public Node
{
    private:

        
        
    public:

        Sky() {}
        virtual ~Sky() {}

        virtual void renderSelf() const;
        virtual void renderSelf(IPartitioner* partitioner = NULL, unsigned int flags = 0) const;
        virtual bool inView(IPartitioner* partitioner = NULL) const {
            return true;
        }
};

#endif

