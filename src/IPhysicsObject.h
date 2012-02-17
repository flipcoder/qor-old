#ifndef _IPHYSICSOBJECT_H
#define _IPHYSICSOBJECT_H

#include <memory>

#include "Physics.h"
class Physics;
#include "math/common.h"
// Simple user data interface for physics for nodes

class IPhysicsObject
{
protected:
    void* m_pPhysicsBody;
    Physics* m_pPhysics;

public:

    enum Type {
        NONE,
        STATIC,
        DYNAMIC,
        ACTOR
    };

    IPhysicsObject():
        m_pPhysicsBody(NULL) {}
    virtual ~IPhysicsObject();
    
    void* getPhysicsBody() { return m_pPhysicsBody; }
    void setPhysicsBody(Physics* sys, void* p) {
        m_pPhysics = sys;
        m_pPhysicsBody=p;
    }
    virtual void sync(glm::mat4* m) {}
    
    virtual float radius() { return 0.0f; }
    virtual float height() { return 0.0f; }

    //virtual void sync(glm::mat4* m) {}
    virtual Type getPhysicsType() { return NONE; }
    virtual unsigned int physicsLogic(float timestep, float mass, glm::vec3& force, glm::vec3& omega, glm::vec3& torque, glm::vec3& velocity);
    virtual float mass() { return 0.0f; }
};

#endif

