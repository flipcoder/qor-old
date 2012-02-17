#ifndef _SPECTATOR_H
#define _SPECTATOR_H

#include "Input.h"
#include "Node.h"
#include "EulerNode.h"
#include "Util.h"
#include "Audio.h"

// We can use EulerNode for this because the physics for our actor is only based on position :)
class Spectator : public EulerNode
{
protected:

    glm::vec3 m_vMove;
    float m_fAccel;
    float m_fDecel;
    float m_fRadius;
    float m_fHeight;
    bool m_bJump;
    float m_fJumpForce;
    float m_fJumpAccel;
    float m_fJumpLength;
    float m_fNormalSpeed;
    float m_fRunSpeed;
    float m_fSpeed;

    Input* m_pInput;
    Audio::Listener m_Listener;

public:

    Spectator(Input* input = NULL);
    virtual ~Spectator();

    virtual bool hasAttribute(unsigned int attr) const {
        return const_cast<Spectator*>(this)->getAttribute(attr)!=NULL ||
            const_cast<Spectator*>(this)->Node::getAttribute(attr)!=NULL;
    }

     //TODO: should take interfaceController in future
    virtual void logicSelf(unsigned int a);

    virtual float radius() { return m_fRadius; }
    virtual float height() { return m_fHeight; }

    // remember when rendering, to set visible(false) if the player is local ;)
    //virtual void renderSelf() {}
    
    virtual void sync(glm::mat4* m) {
        //matrix()->setTranslation(m->translation());
        Matrix::translation(*matrix(), Matrix::translation(*m));
    }

    //virtual IPhysicsObject::Type getPhysicsType() { return IPhysicsObject::SPECTATOR; }
    //virtual unsigned int physicsLogic(float timestep, float mass, glm::vec3& force, glm::vec3& omega, glm::vec3& torque, glm::vec3& velocity);
    //virtual float mass() { return 500.0f; }

    void listen();

    // change this to InputController later
    void input(Input* input) { m_pInput = input; }
    Input* input() const { return m_pInput; }
};

#endif

