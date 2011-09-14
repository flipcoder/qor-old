#ifndef _TRANSITIONSTATE_H
#define _TRANSITIONSTATE_H

#include "../Engine.h"

#include <string>
#include <memory>
#include <boost/concept_check.hpp>
#include <boost/concept/requires.hpp>

class TransitionState : public Engine::State
{
public:
	
	TransitionState(Engine* engine){
		m_pNextState = NULL;
	}
	virtual ~TransitionState() {
		//m_Transition.kill(true);
		//if(!m_Transition.done())
		//    Log::get().warning("Transition state was aborted before thread stopped.");
		//while(!m_Transition.done()) {} // not safe: m_Transition may be accessed after .done() by external caller (Engine)
		
		//delete m_pNextState;
	}
	
	// call this method in another thread
	template<class T>
	BOOST_CONCEPT_REQUIRES(
		((boost::Convertible<T, Engine::State>)),
	(void)) transition()
	{
		m_pNextState = new T(engine, &m_Transition);
		m_Transition.kill(m_pNextState->hasError());
		m_Transition.progress(1.0f);
		m_Transition.done(true);
	}

	virtual int logic(unsigned int advance) {}
	virtual void render() const {}
	
	Engine::State* moveState()
	{
		Engine::State* state = m_pNextState;
		m_pNextState = NULL;
		return state;
	}

private:
	Engine* m_pEngine; //weakref
	Engine::State* m_pNextState;
	TransitionInfo m_Transition;
	void nullify();
};

#endif

