#ifndef _ASSIMPPARSER_H
#define _ASSIMPPARSER_H

#include "math/common.h"
#include <assimp/assimp.hpp>
#include <assimp/aiScene.h>
#include <assimp/aiPostProcess.h>

namespace MeshTypeParser
{
    inline void parseVector(glm::vec3& v, const aiVector3D& aiv) {
        v = glm::vec3(aiv.x, aiv.y, aiv.z);
    }
};

#endif

