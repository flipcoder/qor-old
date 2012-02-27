
#include "GfxAPI.h"
#include "Engine.h"
#include "Input.h"
#include "Freq.h"
#include "Version.h"
#include "game/GameState.h"
#include "editor/EditorState.h"
#include <sstream>
#include "Util.h"
#include "pn/pn.h"

using namespace std;

//Engine::Engine()
//{
//    nullify();
//}

Engine::Engine(std::vector<std::string>& args)
{
    nullify();
    m_vArgs = move(args);
}


Engine::~Engine()
{
    
}

void Engine::nullify()
{
    //m_pRenderer = NULL;
    m_pInput = NULL;
    //m_pTimer = NULL;
    m_pConsole = NULL;
    //m_pSettings = NULL;
    m_pAudio = NULL;
    m_uiLastAdv = 0;
    m_bQuitFlag = false;
}

//Engine* Engine::create(const std::vector<std::string>& args)
//{
//    Engine* d = new Engine(args);

//    if(d->hasError()) {
//        delete d;
//        return NULL;
//    }

//    return d;
//}

// Game Loop (Blocks)
bool Engine::run()
{
    if(!init())
        return false;

    Freq::Alarm FPSAlarm;
    //FPSAlarm.assignToTimer(Freq::ptr());
    const int fpsRefresh = Settings::get().getProperty("Console", "StatDisplaySec", 5);
    if(fpsRefresh)
        FPSAlarm.setSeconds(fpsRefresh);
    int frames = 0;

    while(pollState() && logic() != 1 && !quitFlag())
    {
        render();
        Renderer::get().draw();

        // FPS Counter
        frames++;
        if(fpsRefresh && FPSAlarm.hasElapsed())
        {
            FPSAlarm.setSeconds(fpsRefresh);
            //std::cout<<"FPS: "<<frames/5<<std::endl;
            ostringstream oss("");
            oss << "Simulation FPS: " << (frames/fpsRefresh);
            Log::get().write(oss.str());
            frames = 0;
        }
    }
    
    cleanup();
    return true;
}

bool Engine::init()
{
    if(!Renderer::ptr())
    {
        Renderer::get(new Renderer(
            Settings::get().getProperty("Video","Width",1024),
            Settings::get().getProperty("Video","Height",768),
            0,
            (Settings::get().getProperty("Video","Windowed","true") != "true"),
            PROGRAM_NAME));

        if(Renderer::get().error()){
            // fail
            Log::get().write("Failed to initialize OpenGL/SDL Renderer.");
            return false;
        }
        if(!Renderer::get().startGL()){
            Log::get().write("Failed to initialize OpenGL.");
            return false;
        }
        if(!Renderer::startIL()) {
            Log::get().write("Failed to initialize IL.");
            return false;
        }
    }

    Log::get().write("OpenGL/SDL Renderer initialized.");

    if(!m_pConsole)
        m_pConsole = new Console();
    Log::get().write("Console initialized.");

    if(!m_pInput)
        m_pInput = new Input();
    Log::get().write("Input initialized.");

    if(!m_pAudio)
        m_pAudio = new Audio();
    Log::get().write("Audio initialized.");

    if(!pnInit()) {
        Log::get().write("Failed to initialize ProlificNet.");
        return false;
    }
    Log::get().write("Net initialized.");

    Freq::get(new Freq());
    Log::get().write("Timer initialized.");
    Log::get().write("Engine initialized.");
    
    if(Settings::get().getProperty("Developer", "Enabled", "true") == "true")
        m_spDeveloper.reset(new Developer(this, input()));

    clearToState("game");
    return true;
}

int Engine::logic()
{
    unsigned long now = Freq::get().getElapsedTime();
    unsigned long a = now - m_uiLastAdv;
    
    if(m_pInput->logic() == 1)
        return 1;

    if(m_spDeveloper)
        m_spDeveloper->logic(a);
    if(m_pConsole)
        m_pConsole->logic(a, m_pInput);
    if(currentState())
        if(!currentState()->logic(a))
            return 1;

    m_uiLastAdv = now;
    
    return 0;
}

void Engine::render() const
{
    Renderer::get().clear();
    Renderer::get().shaders(Renderer::BIND_SHADERS);
    Renderer::get().lighting(Renderer::BIND_LIGHTING);

    if(currentState())
        currentState()->render();

    Renderer::get().viewport(Renderer::VIEW_ORTHO);
    Renderer::get().lighting(Renderer::UNBIND_LIGHTING);
    Renderer::get().shaders(Renderer::UNBIND_SHADERS);

    if(m_pConsole)
        m_pConsole->render();
    if(m_spDeveloper)
        m_spDeveloper->render();
}

void Engine::cleanup()
{
    destroyStateManager();

    //Log::get().write("Unloading state...");
    //delete m_pState;
    Log::get().write("Unloading console...");
    delete m_pConsole;
    //Log::get().write("Unloading renderer...");
    //delete m_pRenderer;
    Log::get().write("Unloading input...");
    delete m_pInput;
    //Log::get().write("Unloading timer...");
    //delete m_pTimer;
    Log::get().write("Unloading audio...");
    delete m_pAudio;
    Log::get().write("Application finished.");

    Settings::get().save();
}


IState* Engine :: newState(const std::string id)
{
    IState* state = nullptr;
    if(id == "game")
        state = new GameState();
    else if(id == "editor")
        state = new EditorState();
    else {
        Log::get().error("Unknown state request.");
        return nullptr;
    }

    Log::get().write("State Change: " + boost::to_upper_copy(id));

    if(state && state->hasError())
        Log::get().error(state->getError());
        
    return state;
}

