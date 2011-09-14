#ifndef _DEVELOPER_H
#define _DEVELOPER_H

class Engine;

#include "Engine.h"
#include "Input.h"


class Developer
{
	private:

		Engine* m_pEngine; //weak
		Input* m_pInput; //weak

	public:
		Developer(Engine* core, Input* input);
		virtual ~Developer();

		virtual int logic(unsigned int a);
		virtual void render() const;
};

#endif

