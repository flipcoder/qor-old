#include "Developer.h"
#include "Engine.h"

Developer :: Developer(Input* input)
{
    m_pInput = input;
}

Developer :: ~Developer()
{
    
}

void Developer :: logic(unsigned int a)
{
    if(m_pInput->keyd(SDLK_F1))
        Engine::get().clearToState("game");
    else if(m_pInput->keyd(SDLK_F2))
        Engine::get().clearToState("editor");
    
    if(m_pInput->keyd(SDLK_F12))
        m_pInput->toggleHideMouse();

    if(m_pInput->keyd(SDLK_ESCAPE))
        Engine::get().quit();
}

void Developer :: render() const
{
    
}

