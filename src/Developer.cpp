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
    float t = a * 0.001f;

    if(m_pInput->keyd(SDLK_F1))
        Engine::get().clearToState("game");
    else if(m_pInput->keyd(SDLK_F2))
        Engine::get().clearToState("editor");
    
    if(m_pInput->keyd(SDLK_F12))
        m_pInput->toggleHideMouse();

    if(m_pInput->keyd(SDLK_ESCAPE))
        Engine::get().quit();

    std::shared_ptr<Node> movable;
    if(movable = m_wpMovable.lock()) {
        float speed = 5.0f;
        if(m_pInput->key(SDLK_UP))
            movable->move(glm::vec3(0.0f, 0.0f, -speed*t));
        if(m_pInput->key(SDLK_DOWN))
            movable->move(glm::vec3(0.0f, 0.0f, speed*t));
        if(m_pInput->key(SDLK_LEFT))
            movable->move(glm::vec3(-speed*t, 0.0f, 0.0f));
        if(m_pInput->key(SDLK_RIGHT))
            movable->move(glm::vec3(speed*t, 0.0f, 0.0f));
    }
}

void Developer :: render() const
{
    
}

