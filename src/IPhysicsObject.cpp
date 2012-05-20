#include "IPhysicsObject.h"

using namespace std;

IPhysicsObject :: ~IPhysicsObject()
{
    if(m_pWorld && m_pBody)
        NewtonDestroyBody(m_pWorld, m_pBody);

    //shared_ptr<Physics> physics = m_wpPhysicsSystem.lock();

    //delete m_pMotionState;
    //if(m_pPhysics && m_pPhysicsBody)
    //    m_pPhysics->deleteBody(static_cast<btRigidBody*>(m_pPhysicsBody));
    //else
    //    delete m_pPhysicsBody;
}

unsigned int IPhysicsObject :: physicsLogic(float timestep, float mass, glm::vec3& force, glm::vec3& omega, glm::vec3& torque, glm::vec3& velocity)
{
    return Physics::USER_FORCE;
}

