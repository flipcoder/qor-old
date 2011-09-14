#include "Spectator.h"
#include "GfxAPI.h"
#include "math/common.h"
#include "Audio.h"

//#include <iostream>
//using namespace std;
Spectator::Spectator(Input* input)
{
	m_pInput = input;
	m_fNormalSpeed = 10.0f;
	m_fRunSpeed = m_fNormalSpeed * 4.0f;
	m_fSpeed = m_fNormalSpeed;

	clampPitch(90.0f);
	Matrix::translation(*matrix(), glm::vec3(0.0f, 1.0f, 0.0f));
	m_vMove = glm::vec3(0.0f);
}

Spectator::~Spectator()
{
}

void Spectator::logicSelf(unsigned int a)
{
	if(!m_pInput)
		return;

	glm::vec3 delta(0.0f);
	float turn_speed = 0.1f;
	float timestep = a * 0.001f;

	//if(m_pInput->keyd(SDLK_EQUALS))
	//    m_fAccel *= 2.0;
	//else if(m_pInput->keyd(SDLK_MINUS))
	//    m_fAccel /= 2.0;

	//cout<<m_pInput->getMouseRelX()<<endl;
	rot(-turn_speed * m_pInput->getMouseRelX());
	pitch(-turn_speed * m_pInput->getMouseRelY());
	glm::vec3 forward = heading();

	if(m_pInput->key(SDLK_e))
		delta += forward;
		//delta.setRel(
		//    forward.x,
		//    0.0f,
		//    forward.z);
	if(m_pInput->key(SDLK_d))
		delta -= forward;
		//delta.setRel(
		//    -forward.x,
		//    0.0f,
		//    -forward.z);

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

	if(m_pInput->key(SDLK_SPACE) || m_pInput->key(SDLK_g))
		delta.y += 1.0f;
	if(m_pInput->key(SDLK_a) || m_pInput->key(SDLK_c) || m_pInput->key(SDLK_LCTRL))
		delta.y -= 1.0f;

	if(glm::length(delta) > EPSILON)
		delta = glm::normalize(delta) * m_fSpeed;
	
	//m_vMove = delta;
	Matrix::translate(*matrix(), delta * timestep);

	//listen();
}

void Spectator :: listen()
{
	// TODO: World translation
	Audio::Listener listener;
	m_Listener.at = Matrix::heading(*matrix_c());
	m_Listener.up = Matrix::up(*matrix_c());
	glm::vec3 old_pos = m_Listener.pos;
	m_Listener.pos = Matrix::translation(*matrix());
	m_Listener.vel = m_Listener.pos - old_pos;
	m_Listener.listen();
}

