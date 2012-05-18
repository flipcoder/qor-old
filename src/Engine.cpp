
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
    // TODO: set cleanup() to a scoped callback (take out cleanup() call below if so)

    Freq::Alarm FPSAlarm;
    //FPSAlarm.assignToTimer(Freq::ptr());
    const int fpsRefresh = Settings::get().getProperty("Console", "StatDisplaySec", 5);
    if(fpsRefresh)
        FPSAlarm.setSeconds(fpsRefresh);
    int frames = 0;

    while(pollState() && !quitFlag())
    {
        logic();
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

    cleanup(); // TODO: eventually make this a scoped callback, I don't want it in the destructor
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

    if(!m_spConsole)
        m_spConsole.reset(new Console());
    Log::get().write("Console initialized.");

    if(!m_spInput)
        m_spInput.reset(new Input());
    Log::get().write("Input initialized.");

    if(!m_spAudio)
        m_spAudio.reset(new Audio());
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
    {
        Log::get().write("Developer mode enabled.");
        m_spDeveloper.reset(new Developer(input()));
    }

    clearToState("game");
    return true;
}

void Engine::logic(unsigned int t)
{
    unsigned long now, a;
    now = Freq::get().getElapsedTime();
    a = now - m_uiLastAdv;
    
    m_spInput->logic(a);
    if(m_spInput->quit())
        quit();

    if(m_spDeveloper)
        m_spDeveloper->logic(a);
    if(m_spConsole)
        m_spConsole->logic(a, m_spInput.get());
    if(currentState())
        currentState()->logic(a);

    m_uiLastAdv = now;
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

    if(m_spConsole)
        m_spConsole->render();
    if(m_spDeveloper)
        m_spDeveloper->render();
}

void Engine::cleanup()
{
    destroyStateManager(); // this needs to happen before other components

    Log::get().write("Unloading console...");
    m_spConsole.reset();
    Log::get().write("Unloading input...");
    m_spInput.reset();
    Log::get().write("Unloading audio...");
    m_spAudio.reset();
    Log::get().write("Application finished.");
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

