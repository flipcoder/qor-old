#ifndef _ENTITY_H
#define _ENTITY_H

#include "Node.h"
#include "IMeshContainer.h"
#include "ResourceMap.h"
#include "IPhysicsObject.h"

class Entity : public Node, public IPhysicsObject, public IMeshContainer
{
public:

    enum eFlags {
        F_FLOATING = BIT(0),
        F_PASSABLE = BIT(1),
        F_EXPLOSIVE = BIT(2),
        F_ONTOP = BIT(3)
    };

protected:
    
    unsigned int m_EntityFlags;

public:

    Entity(unsigned int entflags = 0);
    Entity(const std::shared_ptr<Mesh>& m, unsigned int entflags = 0);
    Entity(const std::shared_ptr<Mesh>& m, glm::vec3 pos, unsigned int entflags = 0);
    Entity(std::string fn, ResourceMap<Mesh>& mesh_map, ResourceMap<Texture>& tex_map, glm::vec3 pos = glm::vec3(), unsigned int flags = 0/*, std::string override_name = "", float minlod = 0.1f*/);
    virtual ~Entity();

    virtual bool hasAttribute(unsigned int attr) {
        if(attr == NodeAttributes::PHYSICS)
            return !(m_Flags & F_PASSABLE);
        return Node::hasAttribute(attr);
    }
    virtual void* getAttribute(unsigned int attr) {
        return Node::getAttribute(attr);
    }

    virtual void logicSelf(unsigned int advance){
        IMeshContainer::logic(advance);
    }
    virtual void renderSelf(IPartitioner* partitioner, unsigned int flags = 0) const;
    //virtual void sync(glm::mat4& m) {
    //    *matrix() = m;
    //}

    virtual float mass() { return 10.0f; } // temp
    virtual IPhysicsObject::Type getPhysicsType() { return m_Flags & F_PASSABLE ? NONE : DYNAMIC; }
    virtual unsigned int physicsLogic(float timestep, float mass, glm::vec3& force, glm::vec3& omega, glm::vec3& torque, glm::vec3& velocity){
        //if(m_Flags & F_FLOATING)
        //    force.zero();
        if(m_Flags & F_FLOATING)
            return 0;//NONE
        return Physics::USER_FORCE;
    }
    
    virtual SCOPED_ENUM_TYPE(NodeType) nodeType() const { return NodeType::ENTITY; }
    virtual std::string nodeTypeString() const { return "entity"; }
};

#endif

