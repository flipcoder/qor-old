#ifndef _DUMBYPARTITIONER_H
#define _DUMBYPARTITIONER_H

#include "NodeAttributes.h"
#include "IPartitioner.h"
#include "Light.h"

// A unoptimized implementation of the Partitioner base

class DumbyPartitioner : public IPartitioner
{
public:
    DumbyPartitioner() {}
    virtual ~DumbyPartitioner() {}
    
    virtual std::list<Light*> getViewableLights(Node* root)
    {
        std::list<Light*> light_list;
        searchNodeForType<Light>(root, NodeType::LIGHT, light_list);
        return light_list;
    }
    virtual std::list<Node*> getViewableNodes(Node* root)
    {
        std::list<Node*> node_list;
        nodeTreeToList(root, node_list);
        return node_list;
    }
    
    virtual std::list<Node*> getLitObjects(Light* light, Node* root)
    {
        std::list<Node*> node_list;
        nodeTreeToList(root, node_list);
        return node_list;
    }
    virtual std::list<Node*> getLitObjects(Light* light, std::list<Node*>& node_list)
    {
        // copy list
        return std::list<Node*>(node_list);
    }
    
    
    //virtual std::list<Node*> getAffectedObjects() {
    //    return std::list<Node*>();
    //}
private:
    template<class T>
    void searchNodeForType(Node* n, SCOPED_ENUM_TYPE(NodeType) type, std::list<T*>& match_list)
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
    
    void nodeTreeToList(Node* n, std::list<Node*>& node_list);
};

#endif

