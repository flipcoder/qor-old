#ifndef _GAMESTATE_H
#define _GAMESTATE_H

#include "../Engine.h"
#include "../Scene.h"
#include "../Actor.h"
#include "../Light.h"
#include "../transition/TransitionInfo.h"

#include <string>
#include <memory>

class GameState : public IState
{
public:
    GameState();
    virtual ~GameState();

    virtual void logic(unsigned int advance);
    virtual void render() const;
    //virtual bool error() { return m_bError; }
    //virtual std::string getErrorString() { return m_sError; }

private:

    Actor* m_pPlayer;
    Engine* m_pEngine; //weakref
    Scene* m_pScene;
    Light* m_pLight;
    std::unique_ptr<Audio::Stream> m_spMusic;

    void nullify();
    //void setError(std::string err) {
    //    m_bError = true;
    //    m_sError = err;
    //}
};

#endif

