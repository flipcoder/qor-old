#ifndef _PHYSICS_H
#define _PHYSICS_H

#include <memory>
//#include <newton/Newton.h>
//#include <PxPhysicsAPI.h>
#include "math/common.h"
//#include <btBulletCollisionCommon.h>
//#include <btBulletDynamicsCommon.h>
//#include <BulletCollision/CollisionDispatch/btGhostObject.h>
//#include "extra/KinematicCharacterController.h"
//#include <PxPhysicsAPI.h>
#include "Log.h"
#include "Util.h"
#include "IFallible.h"
#include "IRealtime.h"

class IPhysicsObject;
class Node;
//class Scene;

class Physics: public IFallible, public IRealtime// : public std::enable_shared_from_this<Physics>
{
protected:
    virtual void failsafe();
private:

    static const int NUM_SUBSTEPS = 7;

    //physx::PxFoundation* m_pFoundation;
    //physx::PxPhysics* m_pPhysics;
    //physx::PxProfileZoneManager* m_pProfileZoneManager;
    //physx::PxCooking* m_pCooking;
    //physx::PxDefaultAllocator* m_DefaultAllocatorCallback;
    //physx::PxDefaultErrorCallback* m_DefaultErrorCallback;

    //NewtonWorld* m_pWorld;
    //std::unique_ptr<NewtonWorld> m_pWorld;
    //std::unique_ptr<btDefaultCollisionConfiguration> m_spCollisionConfig;
    //std::unique_ptr<btCollisionDispatcher> m_spDispatcher;
    //std::unique_ptr<btBroadphaseInterface> m_spBroadphase;
    //std::unique_ptr<btSequentialImpulseConstraintSolver> m_spSolver;
    //std::unique_ptr<btDiscreteDynamicsWorld> m_pWorld;

//#ifndef _NEWTON_VISUAL_DEBUGGER
//    void* m_pDebugger;
//#endif

    void generateActor(Node* node, unsigned int flags, glm::mat4* transform);
    void generateTree(Node* node, unsigned int flags, glm::mat4* transform);
    void generateDynamic(Node* node, unsigned int flags, glm::mat4* transform);

public:

    //static btVector3 toBulletVector(const glm::vec3& v) {
    //    return btVector3(v.x,v.y,v.z);
    //}
    //static glm::vec3 fromBulletVector(const btVector3& v){
    //    return glm::vec3(v.x(),v.y(),v.z());
    //}
    //static btTransform toBulletTransform(const glm::mat4& m) {
    //    btTransform t;
    //    t.setFromOpenGLMatrix(glm::value_ptr(m));
    //    return t;
    //}
    //static glm::mat4 fromBulletTransform(const btTransform& t) {
    //    glm::mat4 m;
    //    t.getOpenGLMatrix(glm::value_ptr(m));
    //    return m;
    //}

    Physics();
    virtual ~Physics();

    void nullify();

    /*! Physics Logic
     * \param advance Ticks (in ms) to advanced simulation.
     * \param root Root of physics node which can be automatically synced
     */
    virtual void logic(unsigned int advance);

    enum GenerateFlags {
        GEN_RECURSIVE = BIT(0)
    };
    /*! Generate Physics
     *  \param node Generates physics for a specific node.
     *  \param flags GenerationFlags
     *  \param matrix Current transformation matrix
     */
    void generate(Node* node, unsigned int flags = 0, std::unique_ptr<glm::mat4> transform = std::unique_ptr<glm::mat4>());
    
    enum SyncFlags {
        SYNC_RECURSIVE = BIT(0)
    };
    /*! Sync Body
     *  Syncronizes a single nodes' transformation matrix with the equivalent physics body.
     *  \param node The node to be syncronized
     *  \param flags Syncronization options (use SyncFlags)
     *  \param matrix Current transformation matrix
     */
    void syncBody(Node* node, unsigned int flags = 0);

    // IPhysicsObject param is required to also be of type Node*
    //btRigidBody* addBody(btCollisionObject* obj, IPhysicsObject* pud, glm::mat4* transform);
    //bool deleteBody(PhysicsBody* obj);

    enum {
        USER_FORCE = BIT(0),
        USER_OMEGA = BIT(1),
        USER_TORQUE = BIT(2),
        USER_VELOCITY = BIT(3)
    };
    //static void cbForceTorque(const NewtonBody* body, float timestep, int threadIndex);
    //static void cbTransform(const NewtonBody* body);
    //btCollisionWorld* getWorld() { return m_pWorld.get(); }
    //NewtonWorld* getWorld() { return m_pWorld; }
};

#endif

