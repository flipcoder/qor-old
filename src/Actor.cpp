#include "Actor.h"
#include "GfxAPI.h"
#include "math/common.h"
#include "Audio.h"

//#include <iostream>
//using namespace std;
Actor::Actor(Input* input)
{
    m_pInput = input;

    m_bJump = false;
    m_fAccel = 80.0f;
    m_fDecel = 20.0f;
    m_fRadius = 0.2f;
    m_fHeight = 1.2f, // 1.6
    m_fJumpAccel = 100.0f;
    m_fJumpLength = 1.0f/16.0f;
    m_fJumpForce = m_fJumpLength;
    m_fNormalSpeed = 10.0f;
    m_fRunSpeed = m_fNormalSpeed * 2.0f;
    m_fSpeed = m_fNormalSpeed;

    clampPitch(90.0f);
    Matrix::translation(*matrix(), glm::vec3(0.0f, 1.0f, 0.0f));
    m_vMove = glm::vec3(0.0f);
}

Actor::~Actor()
{
}

void Actor::logicSelf(unsigned int a)
{
    if(!m_pInput)
        return;

    // no motion states on a character controller, so we sync manually
    //setWorldTransform(getBody()->getWorldTransform()); // this will call sync() and then pend the matrix

    glm::vec3 delta(0.0f);
    float turn_speed = 0.1f;

    float t = a * 0.001f;

    //if(m_pInput->keyd(SDLK_EQUALS))
    //    m_fAccel *= 2.0;
    //else if(m_pInput->keyd(SDLK_MINUS))
    //    m_fAccel /= 2.0;

    //cout<<m_pInput->getMouseRelX()<<endl;
    rot(-turn_speed * m_pInput->getMouseRelX());
    pitch(-turn_speed * m_pInput->getMouseRelY());
    glm::vec3 forward = getRotationHeading();

    if(m_pInput->key(SDLK_e))
        delta += glm::vec3(
            forward.x,
            0.0f,
            forward.z);
    if(m_pInput->key(SDLK_d))
        delta += glm::vec3(
            -forward.x,
            0.0f,
            -forward.z);

    if(m_pInput->key(SDLK_LSHIFT))
        m_fSpeed = m_fRunSpeed;
    else
        m_fSpeed = m_fNormalSpeed;

    if(m_pInput->key(SDLK_s))
        delta += glm::vec3(
            forward.z,
            0.0f,
            -forward.x);
    if(m_pInput->key(SDLK_f))
        delta += glm::vec3(
            -forward.z,
            0.0f,
            forward.x);
    
    if(glm::length(delta) > EPSILON)
        delta = glm::normalize(delta);// * m_fAccel;
        //delta |= m_fAccel;

    if(m_pInput->key(SDLK_g) || m_pInput->key(SDLK_SPACE)) {
        delta += glm::vec3(0.0f, 1.0f, 0.0f);
        //if(getCharacter()->canJump()) {
        //    getCharacter()->jump();
        //    // set timer
        //}
    }
    if(m_pInput->key(SDLK_a)) {
        delta -= glm::vec3(0.0f, 1.0f, 0.0f);
    }

    //if(m_pInput->key(SDLK_a))
    //    delta.setRel(0.0,
    //    -m_fAccel,
    //    0.0f);

    m_vMove = delta;
    m_vMove *= m_fSpeed;

    //if(m_vMove != glm::vec3(0.0f)) {
    //    btVector3 old = ((btRigidBody*)getBody())->getLinearVelocity();
    //    btVector3 v = Physics::toBulletVector(m_vMove);
    //    //v.setY(0.0);
    //    ((btRigidBody*)getBody())->setLinearVelocity(old + v * t);
    //}
    move(m_vMove * t);
    //getCharacter()->setVelocityForTimeInterval(Physics::toBulletVector(m_vMove),1.0f);

    //listen(); // listen needs to be called explicitly when rendering
}

unsigned int Actor :: physicsLogic(float timestep, float mass, glm::vec3& force, glm::vec3& omega, glm::vec3& torque, glm::vec3& velocity)
{
    force += m_vMove * mass;

    //float y_vel = velocity.y;
    //velocity.y = 0.0f;
    // TODO: If actor is touching ground {
    if(glm::vec3(m_vMove.x, 0.0f, m_vMove.z) != glm::vec3(0.0f))
    {
        float xz_mag = glm::length(glm::vec3(velocity.x, 0.0f, velocity.z));
        if(xz_mag >= m_fSpeed)
        {
            glm::vec3 velocity_xz = glm::normalize(glm::vec3(velocity.x, 0.0f, velocity.z)) * m_fSpeed;
            velocity = glm::vec3(velocity_xz.x, velocity.y, velocity_xz.z); // clamp to fixed speed
        }
        
        //glm::vec3::angle(m_vMove.XZ(), velocity.XZ()) >= 90.0f;
    }
    //}
        // changed velocity to m_vMove above
        
        //velocity.xz(m_vMove.XZ());
        //velocity |= xz_mag;
    // } else {
    //

    // no user-controlled xz movement
    if(glm::vec3(m_vMove.x, 0.0f, m_vMove.z) == glm::vec3(0.0f))
        velocity -= glm::vec3(velocity.x, 0.0f, velocity.z) * m_fDecel * timestep;

    if(m_bJump && m_fJumpForce >= 0.0f)
    {
        force.y += m_fJumpForce/m_fJumpLength * mass * m_fJumpAccel;
        m_fJumpForce -= timestep;
        if(m_fJumpForce<0.0f)
            m_fJumpForce = 0.0f;
    }

    return Physics::USER_FORCE | Physics::USER_OMEGA | Physics::USER_TORQUE | Physics::USER_VELOCITY;
}

void Actor :: listen()
{
    // TODO: World translation
    Audio::Listener listener;
    m_Listener.at = Matrix::heading(*matrix_c());
    m_Listener.up = Matrix::up(*matrix_c());
    glm::vec3 old_pos = m_Listener.pos;
    m_Listener.pos = Matrix::translation(*matrix_c());
    m_Listener.vel = m_Listener.pos - old_pos;
    m_Listener.listen();
}

