#include "GameState.h"
#include "../Scene.h"
#include "../RenderBuffer.h"
#include "../Entity.h"
#include "../Light.h"
#include "../Sound.h"
#include "../ParticleSystem.h"

GameState::GameState()
{
    nullify();

    m_pScene = new Scene("testmap.fml", Scene::F_PHYSICS);
    if(!m_pScene->hasError())
        Log::get().write("Scene initialized.");
    else
    {
        Log::get().error(m_pScene->getError());
        setError("Failed to initiate game state.");
    }

    m_pScene->add(m_pPlayer = new Actor(Engine::get().input()));

    // "Compile" Physics
    m_pScene->compile();

    // Setup renderer states to be appropriate for mode
    Engine::get().input()->hideMouse(true);
    Renderer::get().lighting(Renderer::ENABLE_LIGHTING);
    Renderer::get().shaders(Renderer::ENABLE_SHADERS);
}

GameState::~GameState()
{
    Engine::get().input()->hideMouse(false);

    delete m_pScene;
}

void GameState::nullify()
{
    m_pScene = NULL;
}

int GameState::logic(unsigned int advance)
{
    // perform logic for all objects in scene graph
    m_pScene->logic(advance);

    //m_spMusic->refresh();

    // set player as audio listener
    m_pPlayer->listen();

    return 0;
}

void GameState::render() const
{
    Renderer::get().viewport(Renderer::VIEW_PERSPECTIVE, 0.0f, 0.0f, 1.0f, 1.0f);
    m_pScene->render(m_pPlayer);
}

