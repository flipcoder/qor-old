#ifndef _EDITORSTATE_H
#define _EDITORSTATE_H

#include "../Engine.h"
#include "../Scene.h"
#include "../Spectator.h"
#include "../Freq.h"
#include "../GUI.h"
#include "Selection.h"
#include "../math/common.h"
#include "../transition/TransitionInfo.h"

#include <string>
#include <memory>

#include <FTGL/ftgl.h>

class EditorState : public Engine::State
{
public:

    EditorState(Engine* engine, TransitionInfo* trans = NULL);
    virtual ~EditorState();
    void loadGUI();
    void loadScene();

    virtual int logic(unsigned int advance);
    virtual void render() const;
    void syncView() const;

    // x...() prefixed methods are GUI callbacks
    void xFileMenu(GUI::ToggleButton* b);
    void xAddMenu(GUI::ToggleButton* b);
    void xViewMenu(GUI::ToggleButton* b);
    void xNew(GUI::Menu* menu, GUI::Menu::Option* op);
    void xOpen(GUI::Menu* menu, GUI::Menu::Option* op);
    void xSave(GUI::Menu* menu, GUI::Menu::Option* op);
    void xQuit(GUI::Menu* menu, GUI::Menu::Option* op);
    void xTest(GUI::Menu* menu, GUI::Menu::Option* op);
    
    void xAddEnvironment(GUI::Menu* menu, GUI::Menu::Option* op);
    void xAddObject(GUI::Menu* menu, GUI::Menu::Option* op);
    void xAddByFilename(GUI::Menu* menu, GUI::Menu::Option* op, std::string filename);

private:

    bool m_bError;
    std::string m_sError;
    glm::vec2 m_vButtonSize;

    Spectator* m_pPlayer;
    Engine* m_pEngine; //weakref
    std::unique_ptr<Scene> m_spScene;

    std::shared_ptr<FTPixmapFont> m_spFont;
    std::shared_ptr<FTPixmapFont> m_spBoldFont;
    std::unique_ptr<GUI> m_spGUI;
    std::shared_ptr<GUI::Menu> m_spFileMenu;
    std::shared_ptr<GUI::Menu> m_spAddMenu;
    std::shared_ptr<GUI::Menu> m_spViewMenu;

    glm::vec3 mouseWorldSpace() const;
    glm::vec3 mouseLocalSpace() const;
    glm::vec3 mouseWorldSpaceXZ() const;
    glm::vec3 mouseWorldSpaceXnZ() const;
    glm::vec3 mouseLocalSpaceXZ() const;
    
    Selection m_Selection;

    //float m_fOverviewScale;
    Freq::Timed<float> m_OverviewScale;
    Freq::Timed<glm::vec3> m_vView;
    float m_fEdgeScrollSpeed;
    unsigned int m_ZoomTime;
    float m_fZoomRatio;
    Freq::Time m_FadeTime;
        

    void nullify();
    //void setError(std::string err) {
    //    m_bError = true;
    //    m_sError = err;
    //    Log::get().error(err);
    //}
};

#endif

