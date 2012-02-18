#include "NodeFactory.h"
#include "EnvironmentNode.h"
#include "Entity.h"
#include "Scene.h"
#include "Light.h"

namespace NodeFactory{
    
    int getNodeTypeID(std::string s) {
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

        if(nodeTypeID == (int)NodeType::ENVIRONMENT)
        {
            Node* node = NULL;
            if(fn.empty())
                return NULL;
            
            // TODO: add path testing to loadAI() code instead of the messy stuff here:
            node = scene->loadAI(fn, glm::vec3(), 0, parent);//new EnvironmentNode(Node::defaultFlags(), fn);
            if(!node)
                node = scene->loadAI("data/custom/environment/" + fn, glm::vec3(), 0, parent);
            if(!node)
                node = scene->loadAI("data/custom/assets/" + fn, glm::vec3(), 0, parent);
            if(!node)
                Log::get().write("Node failed to load.");
            return node;
            //else return new EnvironmentNode(Node::defaultFlags());
        }
        else if(nodeTypeID == (int)NodeType::ENTITY)
            return new Entity();
        else if (nodeTypeID == (int)NodeType::LIGHT)
            return new Light();
        else if(nodeTypeID == (int)NodeType::NODE)
            return new Node();
            //return new Entity(Node::defaultFlags(), fn);
        //else if(nodeTypeName == "zone")
        //    return new Zone();
        return NULL;
    }
}

