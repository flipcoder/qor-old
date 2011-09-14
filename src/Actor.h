#ifndef _ACTOR_H
#define _ACTOR_H

#include "Input.h"
#include "Node.h"
#include "EulerNode.h"
#include "IPhysicsObject.h"
#include "Util.h"
#include "Audio.h"
#include "math/common.h"

// We can use EulerNode for this because the physics for our actor is only based on position :)
class Actor : public EulerNode, public IPhysicsObject
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

	Actor(Input* input = NULL);
	virtual ~Actor();

	virtual bool hasAttribute(unsigned int attr) const {
		if(attr == NodeAttributes::PHYSICS)
			return true;
		return const_cast<Actor*>(this)->getAttribute(attr)!=NULL ||
			const_cast<Actor*>(this)->Node::getAttribute(attr)!=NULL;
	}

 	//TODO: should take interfaceController in future
	virtual void logicSelf(unsigned int a);

	virtual float radius() { return m_fRadius; }
	virtual float height() { return m_fHeight; }

	// remember when rendering, to set visible(false) if the player is local ;)
	//virtual void renderSelf() {}
	
	virtual void sync(glm::mat4* m) {
		Matrix::translation(*matrix(), Matrix::translation(*m));
		pendWorldMatrix();
	}

	virtual IPhysicsObject::Type getPhysicsType() { return IPhysicsObject::ACTOR; }
	virtual unsigned int physicsLogic(float timestep, float mass, glm::vec3& force, glm::vec3& omega, glm::vec3& torque, glm::vec3& velocity);
	virtual float mass() { return 500.0f; }

	void listen();

	// change this to InputController later
	void input(Input* input) { m_pInput = input; }
	Input* input() const { return m_pInput; }
	
	virtual SCOPED_ENUM_TYPE(NodeType) nodeType() const { return NodeType::ACTOR; }

	//test
	//virtual void writeSelf(std::ofstream& file, int indent) const {file << "ACTOR" << std::endl;}
};

#endif

