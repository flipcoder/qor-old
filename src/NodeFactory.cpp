#include "NodeFactory.h"
#include "EnvironmentNode.h"
#include "Entity.h"
#include "Scene.h"
#include "Light.h"

namespace NodeFactory{
    
    static int getNodeTypeID(std::string s) {
        for(unsigned int i=0; i<NodeTypeStrings.size(); i++)
            if(NodeTypeStrings[i] == s)
                return i;
        return -1;
    }

    Node* create(Scene* scene, Node* parent, std::string nodeTypeName, std::string fn)
    {
        ASSERT(scene);
        if(!parent)
            parent = scene->root();

        int nodeTypeID = getNodeTypeID(nodeTypeName);
        if(nodeTypeID < 0)
            return NULL;

        switch(nodeTypeID) {
            case (int)NodeType::ENVIRONMENT:
            {
                Node* node = NULL;
                if(fn.empty())
                    return NULL;
                
                // TODO: add path testing to loadModel() code instead of the messy stuff here:
                node = scene->loadModel(fn, glm::vec3(), 0, parent);//new EnvironmentNode(Node::defaultFlags(), fn);
                if(!node)
                    node = scene->loadModel("data/ne/environment/" + fn, glm::vec3(), 0, parent);
                if(!node)
                    node = scene->loadModel("data/ne/assets/" + fn, glm::vec3(), 0, parent);
                if(node)
                    scene->clearError();
                else
                    return NULL;
                return node;
                //else return new EnvironmentNode(Node::defaultFlags());
                break;
            }
            case (int)NodeType::ENTITY:
                return new Entity();
                break;
            case (int)NodeType::LIGHT:
                return new Light();
                break;
            case (int)NodeType::NODE:
                return new Node();
                break;
            //case (int)NodeType::ZONE:
            //case (int)NodeType::SKY:
        }
        
            //return new Entity(Node::defaultFlags(), fn);
        //else if(nodeTypeName == "zone")
        //    return new Zone();
        return NULL;
    }
}

