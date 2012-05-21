#ifndef _ENGINE_H
#define _ENGINE_H

#include <string>
#include <vector>
#include <boost/unordered_map.hpp>
#include <boost/optional.hpp>
#include <boost/array.hpp>
#include "Renderer.h"
#include "Input.h"
#include "Freq.h"
#include "Log.h"
#include "Console.h"
#include "Settings.h"
#include "Audio.h"
#include "IFallible.h"
#include "IStateManager.h"
#include "IState.h"

class Developer;
#include "Developer.h"

class Engine:
    public IRealtime,
    public IRenderable,
    public IStaticInstance<Engine>,
    public IFallible,
    public IStateManager<std::string, IState>
{
public:

    enum eOption {
        OP_SCENE,
        MAX_OP
    };

private:

    //std::string m_sError;
    //std::string m_sStateDest;
    std::vector<std::string> m_vArgs;
    boost::array<std::string, MAX_OP> m_Options;

    //Renderer* m_pRenderer;
    std::unique_ptr<Input> m_spInput;
    //Freq* m_pTimer;
    //Log* m_pLog;
    std::unique_ptr<Console> m_spConsole;
    //Settings* m_pSettings;
    std::unique_ptr<Audio> m_spAudio;
    std::unique_ptr<Developer> m_spDeveloper;

    unsigned int m_uiLastAdv;
    bool m_bQuitFlag;

    // Perform state changes as necessary
    void nullify();

public:

    Engine(std::vector<std::string>& args);
    //static Engine* create(const std::vector<std::string>& args);

    std::string getArg(unsigned int idx) const {
        if(idx < 0 || idx >= m_vArgs.size())
            return "";
        return m_vArgs[idx];
    }
    
    std::string getOption(eOption op) {
        if(op < MAX_OP)
            return m_Options[op];
        return "";
    }
    //std::string getOption(const std::string& name) {
    //    if(m_Options.find(name) != m_Options.end()){
    //        return m_Options[name];
    //    }
    //    return "";
    //}
    
    virtual ~Engine();

    bool init();
    virtual void logic(unsigned int t = 0);
    virtual void render() const;
    void cleanup();
    bool run();
    
    //Renderer* renderer() const { return m_pRenderer; }
    Input* input() const { return m_spInput.get(); }
    //Freq* timer() const { return m_pTimer; }
    Audio* audio() const { return m_spAudio.get(); }
    Developer* developer() const { return m_spDeveloper.get(); }

// IStateManager
    virtual IState* newState(const std::string id);

// Accessors:
    //bool hasError() const { return (m_sError!=""); }
    //void setState(std::string s) { m_sStateDest = s; }
    //void swapState(State* next_state);

    bool quitFlag() const { return m_bQuitFlag; }
    void quit(bool b = true) { m_bQuitFlag = b; }
};

#endif

