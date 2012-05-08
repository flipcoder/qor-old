#ifndef _ENVIRONMENTNODE_H
#define _ENVIRONMENTNODE_H

#include <string>
#include "math/common.h"
#include "IMeshContainer.h"
#include "IPhysicsObject.h"
#include "Node.h"

// EnvironmentNode is a Node-style class that can contain static meshes

class EnvironmentNode : public Node, public IMeshContainer, public IPhysicsObject
{
    public:

        EnvironmentNode(unsigned int flags = 0,
            std::string model_file_ref = "",
            unsigned int nlayer = NodeLayer::ENVIRONMENT)
        {
            m_sModelFile = model_file_ref;
            addFlags(flags);
            layer(nlayer);
        }
        virtual ~EnvironmentNode() {}
        
        virtual void logicSelf(unsigned int advance){
            IMeshContainer::logic(advance);
        }
        
        virtual void renderSelf(IPartitioner* partitioner, unsigned int flags = 0) const;

        virtual bool hasAttribute(unsigned int attr) const {
            if(attr == NodeAttributes::PHYSICS)
                return true;
            if(attr = NodeAttributes::MESH_CONTAINER)
                return true;
            if(attr == NodeAttributes::FILENAME)
                return !m_sModelFile.empty();
            if(attr == NodeAttributes::SIZE)
                return true;
            return const_cast<EnvironmentNode*>(this)->Node::hasAttribute(attr);
        }
        virtual void* getAttribute(unsigned int attr) {
            if(attr == NodeAttributes::PHYSICS)
                return (void*)this;
            if(attr == NodeAttributes::MESH_CONTAINER)
                return (void*)this;
            if(attr == NodeAttributes::FILENAME && !m_sModelFile.empty())
                return (void*)this;
            if(attr == NodeAttributes::SIZE)
                return (void*)this;
            return Node::getAttribute(attr);
        }
        virtual void* getAttribute(unsigned int attr) const {
            return const_cast<EnvironmentNode*>(this)->getAttribute(attr);
        }

        virtual IPhysicsObject::Type getPhysicsType() { return IPhysicsObject::STATIC; }

        virtual std::string filename() const {
            return m_sModelFile;
        }
        virtual void filename(std::string fn) {
            m_sModelFile = fn;
        }
        
        virtual std::string nodeTypeString() const { return "environment"; }
        virtual SCOPED_ENUM_TYPE(NodeType) nodeType() const { return NodeType::ENVIRONMENT; }
        
        virtual const AABB* box() const { return &m_Box; }
        virtual AABB* box() { return &m_Box; }
        virtual void recalculate();
        
        virtual const Node* superParent() const {
            if(layer() == NodeLayer::SUB_ENVIRONMENT/* && getParent_c()*/)
            {
                ASSERT(getParent_c());
                return getParent_c()->superParent();
            }
            return this;
        }

    private:

        std::string m_sModelFile; // model file that this node was loaded from originally
        AABB m_Box;
};

#endif

