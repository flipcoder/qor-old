#include "IPhysicsObject.h"

using namespace std;

IPhysicsObject :: ~IPhysicsObject()
{
    //shared_ptr<Physics> physics = m_wpPhysicsSystem.lock();

    if(m_pPhysics && m_pPhysicsBody)
        m_pPhysics->deleteBody(m_pPhysicsBody);
}

unsigned int IPhysicsObject :: physicsLogic(float timestep, float mass, glm::vec3& force, glm::vec3& omega, glm::vec3& torque, glm::vec3& velocity)
{
    return Physics::USER_FORCE;
}

