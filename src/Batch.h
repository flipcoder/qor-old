#ifndef BATCH_H
#define BATCH_H

// Graphics polygon batch
// Used for emulation of immediate mode

#include <vector>
#include "math/common.h"

class Batch
{
    public:

        std::vector<glm::vec3> m_verts;
        
        Batch() {}
        virtual ~Batch() {}

        void clear() { 
            m_verts.clear();
        }
        void render() const {
            
        }
};

#endif

