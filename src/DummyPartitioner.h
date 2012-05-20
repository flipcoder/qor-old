#ifndef _DUMMYPARTITIONER_H
#define _DUMMYPARTITIONER_H

#include "NodeAttributes.h"
#include "IPartitioner.h"
#include "Light.h"

// A unoptimized implementation of the Partitioner base

class DummyPartitioner : public IPartitioner
{
public:
    DummyPartitioner() {}
    virtual ~DummyPartitioner() {}
    
    virtual std::vector<Light*> getViewableLights(Node* root)
    {
        std::vector<Light*> light_list;
        searchNodeForType<Light>(root, NodeType::LIGHT, light_list);
        return light_list;
    }
    virtual std::vector<Node*> getViewableNodes(Node* root)
    {
        std::vector<Node*> node_list;
        nodeTreeToList(root, node_list);
        return node_list;
    }
    
    virtual std::vector<Node*> getLitObjects(Light* light, Node* root)
    {
        std::vector<Node*> node_list;
        nodeTreeToList(root, node_list);
        return node_list;
    }
    virtual std::vector<Node*> getLitObjects(Light* light, std::vector<Node*>& node_list)
    {
        // copy vector
        return std::vector<Node*>(node_list);
    }
    
    
    //virtual std::vector<Node*> getAffectedObjects() {
    //    return std::vector<Node*>();
    //}
private:
    template<class T>
    void searchNodeForType(Node* n, SCOPED_ENUM_TYPE(NodeType) type, std::vector<T*>& match_list)
    {
        ASSERT(n);
        if(n->nodeType() == type)
            match_list.push_back(dynamic_cast<T*>(n));
        
        for(auto itr = n->children()->begin();
            itr != n->children()->end();
            ++itr)
        {
            searchNodeForType(itr->get(),type,match_list);
        }
    }
    
    void nodeTreeToList(Node* n, std::vector<Node*>& node_list);
};

#endif

