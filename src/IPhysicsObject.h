#ifndef _IPHYSICSOBJECT_H
#define _IPHYSICSOBJECT_H

#include <memory>
#include <vector>

//#include <newton/Newton.h>
#include "Physics.h"
class Physics;
#include "math/common.h"
#include "Node.h"

class IPhysicsObject/*: public btMotionState*/
{
protected:
    //std::unique_ptr<btCollisionObject> m_pBody;
    //std::unique_ptr<btActionInterface> m_ActionInterface;
    //std::unique_ptr<btGhostPairCallback> m_GhostPairCallback;
    //std::vector<std::unique_ptr<btStridingMeshInterface>> m_StridingMeshInterfaces;
    //std::vector<std::unique_ptr<btCollisionShape>> m_CollisionShapes;
    //btMotionState* m_pMotionState;
    
    //NewtonWorld* m_pWorld; // weak
    //std::unique_ptr<NewtonBody> m_pBody;
    //NewtonBody* m_pBody;
    Physics* m_pPhysics; // weak

public:

    enum Type {
        NONE,
        STATIC,
        DYNAMIC,
        ACTOR
    };

    IPhysicsObject():
        //m_pWorld(NULL),
        //m_pBody(NULL),
        m_pPhysics(NULL) {}
    virtual ~IPhysicsObject();
    
    Physics* getPhysics() { return m_pPhysics; }
    //btMotionState* getMotionState() { return this; }
    //NewtonBody* getPhysicsBody() { return m_pBody; }
    void* getPhysicsBody() { return NULL; } //placeholder
    void setPhysics(Physics* sys) {
        m_pPhysics = sys;
    }
    //void setBody(NewtonBody* obj) {
    //    m_pBody = obj;
    //}
    //NewtonBody* getBody() { return m_pBody; }
    
    virtual float radius() { return 0.0f; }
    virtual float height() { return 0.0f; }

    virtual void sync(glm::mat4* m) {}
    virtual Type getPhysicsType() { return NONE; }
    //virtual unsigned int physicsLogic(float timestep, float mass, glm::vec3& force, glm::vec3& omega, glm::vec3& torque, glm::vec3& velocity);
    virtual float mass() { return 0.0f; }
    
    //virtual void setWorldTransform(const btTransform& worldTrans) {
    //    Node* node = dynamic_cast<Node*>(this);
    //    glm::mat4 matrix = Physics::fromBulletTransform(worldTrans);
    //    if(!sync(matrix))
    //        *node->matrix() = matrix;
    //    node->pendWorldMatrix();
    //}
    //virtual void getWorldTransform(btTransform& worldTrans) const {
    //    const Node* node = dynamic_cast<const Node*>(this);
    //    worldTrans = Physics::toBulletTransform(*node->matrix_c());
    //}
    
    //// returns true if the object will sync its own properties, otherwise false to autosync
    ////virtual bool sync(glm::mat4& m) {
    ////    return false;
    ////}

    //virtual void setKinematicPos(btTransform &currentPos) {}

    //void addStridingMeshInterface(std::unique_ptr<btStridingMeshInterface>& a) {
    //    m_StridingMeshInterfaces.push_back(std::move(a));
    //}
    //void addCollisionShape(std::unique_ptr<btCollisionShape>& a) {
    //    m_CollisionShapes.push_back(std::move(a));
    //}
    //void setAction(std::unique_ptr<btActionInterface>& a) {
    //    m_ActionInterface = std::move(a);
    //}
    //btActionInterface* getAction() {
    //    return m_ActionInterface.get();
    //}
    //KinematicCharacterController* getCharacter() {
    //    return dynamic_cast<KinematicCharacterController*>(m_ActionInterface.get());
    //}
    //void setGhostPairCallback(std::unique_ptr<btGhostPairCallback>& a) {
    //    m_GhostPairCallback = std::move(a);
    //}
};

#endif

