#ifndef _PARTITIONER_H
#define _PARTITIONER_H

#include <list>
#include "NodeAttributes.h"

class Node;
class Light;

class IPartitioner
{
public:
    IPartitioner() {}
    virtual ~IPartitioner() {}

    virtual std::list<Light*> getViewableLights(Node* root) = 0;
    virtual std::list<Node*> getViewableNodes(Node* root) = 0;
    virtual std::list<Node*> getLitObjects(Light* light, Node* root) = 0;
    virtual std::list<Node*> getLitObjects(Light* light, std::list<Node*>& node_list) = 0;

    virtual SCOPED_ENUM_TYPE(Space) getBoundingSpace() { return Space::LOCAL; }
private:
};

#endif

