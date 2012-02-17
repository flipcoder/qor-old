#ifndef _NODEFACTORY_H
#define _NODEFACTORY_H

#include <string>
#include "Node.h"

class Scene;

namespace NodeFactory
{
    //Node* create(std::string nodeTypeName, std::string fn = "");
    Node* create(Scene* scene, Node* parent, std::string nodeTypeName, std::string fn = "");
};

#endif
