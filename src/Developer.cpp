#include "Developer.h"

Developer :: Developer(Engine* core, Input* input)
{
    m_pEngine = core;
    m_pInput = input;
}

Developer :: ~Developer()
{
    
}

int Developer :: logic(unsigned int a)
{
    if(m_pInput->keyd(SDLK_F1))
        m_pEngine->clearToState("game");
    else if(m_pInput->keyd(SDLK_F2))
        m_pEngine->clearToState("editor");
    
    if(m_pInput->keyd(SDLK_F12))
        m_pInput->toggleHideMouse();
}

void Developer :: render() const
{
    
}

