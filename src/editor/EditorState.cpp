#include "EditorState.h"
#include "SelectorNode.h"
#include "../Scene.h"
#include "../RenderBuffer.h"
#include "../Entity.h"
#include "../Light.h"
#include "../Sound.h"
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
using namespace std;
using namespace placeholders;

EditorState::EditorState()
{
    nullify();

    try{
        
        m_spFont.reset(new FTGLPixmapFont("data/base/fonts/liberation/LiberationSans-Regular.ttf"));
        if( !m_spFont || 
             m_spFont->Error()||
            !m_spFont->FaceSize(12))
        {
            setError("Failed to load font.");
            throw 1;
        }
        m_spBoldFont.reset(new FTGLPixmapFont("data/base/fonts/liberation/LiberationSans-Bold.ttf"));
        if( !m_spBoldFont || 
             m_spBoldFont->Error()||
            !m_spBoldFont->FaceSize(12))
        {
            setError("Failed to load font (Bold).");
            throw 1;
        }

        m_spGUI.reset(new GUI(new GUI::DefaultStyle(m_spFont, m_spBoldFont)));
        if(!m_spGUI || m_spGUI->hasError())
            throw 1;

        loadScene();

        string filename;
        if((filename = Engine::get().getOption(Engine::OP_SCENE)).empty())
            filename = Engine::get().getArg(0);

        // temp
        if(filename.empty())
           filename = "data/ne/maps/Apartment/apartment_scene.obj";

        m_spScene.reset(new Scene());
        m_spScene->getTextureResources()->addPath("data/ne/materials");
        m_spScene->getMeshResources()->addPath("data/ne/assets");

        m_spScene->load("data/ne/maps/gothicTower/gothicTower.obj"); //temp

        if(!filename.empty())
            m_spScene->load(filename);
        if(!m_spScene || m_spScene->hasError())
        {
            Log::get().error(m_spScene->getError());
            setError("Failed to initiate editor state.");
            throw 1;
        }
        
        m_spScene->add(m_pPlayer = new Spectator(/*Engine::get().input()*/));
        m_vView.clear(Matrix::translation(*m_pPlayer->matrix()));

        //m_spScene->compile();
        Engine::get().input()->hideMouse(false);

        loadGUI();
        
        Renderer::get().lighting(Renderer::DISABLE_LIGHTING);
        Renderer::get().shaders(Renderer::DISABLE_SHADERS);
                
        Light* light;
        m_spScene->add(light = new Light());
        light->atten(glm::vec3(0.0f, 0.0f, 0.0f));
        light->diffuse(Color(1.0f, 1.0f, 1.0f));
        
        //Log::get().write("Starting Test...");
        //glm::vec3 point(1.0f, 2.0f, 3.0f);
        //MatrixStack ms;
        //ms.push(Matrix(Matrix::IDENTITY, glm::vec3(1.0f)));
        //point *= *ms.top();
        //Log::get().write("Test resulted in: ("
        //    + str(point.x) + ", "
        //    + str(point.y) + ", "
        //    + str(point.z) + ")");
        //Log::get().write("End Test.");
    }
    catch(int)
    {
        if(!hasError())
            setError("Unknown Editor State Construction Error");
    }
}

EditorState::~EditorState()
{
    Engine::get().input()->hideMouse(false);
}

void EditorState::loadScene()
{
    
}

void EditorState::loadGUI()
{
    float separator_size = 1.0f;
    unsigned int flags = GUI::F_AUTODEACTIVATE;

    vector<pair<string, function<void(GUI::ToggleButton*)>>> buttons = {
        { "File", bind(&EditorState::xFileMenu, this, _1) },
        { "Add", bind(&EditorState::xAddMenu, this, _1) },
        { "View", bind(&EditorState::xViewMenu, this, _1) }
    };

    m_spGUI->clear();
    for(unsigned int i=0; i<buttons.size(); i++)
    {
        m_spGUI->add(new GUI::ToggleButton(
            buttons[i].first,
            Freq::Timed<glm::vec2>(
                m_FadeTime,
                glm::vec2(
                    (m_vButtonSize.x + separator_size)*i*1.0f,
                    Renderer::get().size().y
                ),
                glm::vec2(
                    (m_vButtonSize.x + separator_size)*i*1.0f,
                    Renderer::get().size().y - m_vButtonSize.y
                )
            ),
            m_vButtonSize,
            buttons[i].second,
            flags
        ));
    }

    m_spFileMenu.reset(new GUI::Menu(
        list<GUI::Menu::Option>(
            {
                { "New", bind(&EditorState::xNew, this, _1, _2) },
                { "Open", bind(&EditorState::xOpen, this, _1, _2) },
                { "Save", bind(&EditorState::xSave, this, _1, _2) },
                { "Save As", bind(&EditorState::xTest, this, _1, _2) },
                { "Save+Test", bind(&EditorState::xTest, this, _1, _2) },
                { "Quit", bind(&EditorState::xQuit, this, _1, _2) }
            }
        ),
        Freq::Timed<glm::vec2>(
            m_FadeTime,
            glm::vec2(0.0f, Renderer::get().size().y - m_vButtonSize.y * 2.0f),
            glm::vec2(0.0f, Renderer::get().size().y + m_vButtonSize.y * 6.0f) // num items
        ),
        m_vButtonSize,
        GUI::F_HIDDEN | GUI::F_AUTOHIDE
    ));

    m_spAddMenu.reset(new GUI::Menu(
        list<GUI::Menu::Option>(
            {
                { "Environment", bind(&EditorState::xAddEnvironment, this, _1, _2) },
                { "Object", bind(&EditorState::xAddObject, this, _1, _2) },
                { "Particles", bind(&EditorState::xTest, this, _1, _2) },
                { "Light", bind(&EditorState::xTest, this, _1, _2) },
                { "Sound", bind(&EditorState::xTest, this, _1, _2) },
                { "Zone", bind(&EditorState::xTest, this, _1, _2) },
                { "Marker", bind(&EditorState::xTest, this, _1, _2) }
            }
        ),
        Freq::Timed<glm::vec2>(
            m_FadeTime,
            glm::vec2(m_vButtonSize.x, Renderer::get().size().y - m_vButtonSize.y * 2.0f),
            glm::vec2(m_vButtonSize.x, Renderer::get().size().y + m_vButtonSize.y * 7.0f) // num items
        ),
        m_vButtonSize,
        GUI::F_HIDDEN | GUI::F_AUTOHIDE
    ));

    m_spGUI->add(m_spFileMenu);
    m_spGUI->add(m_spAddMenu);
}

void EditorState::xFileMenu(GUI::ToggleButton* b)
{
    Log::get().write(b->caption() + " clicked.");
    m_spFileMenu->position().reverse();
    m_spFileMenu->hide(!b->activated());
}

void EditorState::xAddMenu(GUI::ToggleButton* b)
{
    Log::get().write(b->caption() + " clicked.");
    m_spAddMenu->position().reverse();
    m_spAddMenu->hide(!b->activated());
}

void EditorState::xViewMenu(GUI::ToggleButton* b)
{
    Log::get().write(b->caption() + " clicked.");
}

void EditorState::xTest(GUI::Menu* menu, GUI::Menu::Option* op)
{
    Log::get().write(op->caption() + " clicked.");
}

void EditorState::xNew(GUI::Menu* menu, GUI::Menu::Option* op)
{
    //m_spGUI->dialog(new GUI::Dialog(
    //));
    
    //m_spGUI->add(new GUI::Button(
    //    "Yes",
    //    Freq::Timed<glm::vec2>(
    //        Time::seconds(0.5f),
    //        glm::vec2(
    //            Renderer::get().size().x - m_vButtonSize.x,
    //            -Renderer::get().size().y
    //        ),
    //        glm::vec2(
    //            Renderer::get().size().x - m_vButtonSize.x,
    //            Renderer::get().size().y
    //        )
    //    ),
    //    m_vButtonSize,
    //    buttons[i].second,
    //));
    //m_spGUI->add(new GUI::Button(
    //    "No",
    //    Freq::Timed<glm::vec2>(
    //        Time::seconds(0.5f),
    //        glm::vec2(
    //            Renderer::get().size().x + m_vButtonSize.x,
    //            -Renderer::get().size().y
    //        ),
    //        glm::vec2(
    //            Renderer::get().size().x + m_vButtonSize.x,
    //            Renderer::get().size().y
    //        )
    //    ),
    //    m_vButtonSize,
    //    buttons[i].second,
    //));
}

void EditorState::xOpen(GUI::Menu* menu, GUI::Menu::Option* op)
{
    Log::get().write(op->caption() + " clicked.");
    if(!m_spScene->load("testmap.fml"))
        Log::get().error("Map failed to load.");
    m_spScene->add(m_pPlayer = new Spectator());
}

void EditorState::xSave(GUI::Menu* menu, GUI::Menu::Option* op)
{
    Log::get().write(op->caption() + " clicked.");
    m_spScene->info("Untitled Level", "0.0", "Unknown Author", "No Description");
    m_spScene->saveFML("testmap.fml");
}

void EditorState::xQuit(GUI::Menu* menu, GUI::Menu::Option* op)
{
    Log::get().write(op->caption() + " clicked.");
    Engine::get().quit();
}

void EditorState :: xAddEnvironment(GUI::Menu* menu, GUI::Menu::Option* op)
{
    Log::get().write(op->caption() + " clicked.");
    string folder_path = "data/ne/environment";
    
    list<GUI::Menu::Option> asset_option_list;
    size_t asset_option_list_sz = 0;
    
    try{
       boost::filesystem::directory_iterator end_itr;
       boost::filesystem::path asset_path(folder_path);
       for(boost::filesystem::directory_iterator dir_itr(asset_path);
           dir_itr != end_itr;
           ++dir_itr)
       {
           if(boost::filesystem::is_regular_file(dir_itr->status()))
           {
               //string fn = dir_itr->filename();
               string fn = dir_itr->path().filename().string();
               if(stringEndsWith(boost::to_lower_copy(fn), string(".obj")))
                   asset_option_list.push_back(GUI::Menu::Option(fn, bind(&EditorState::xAddByFilename, this, _1, _2, folder_path+"/"+fn)));
           }
       }
    }catch(const std::exception& ex){return;}
    
    asset_option_list_sz = asset_option_list.size();
    
    if(asset_option_list_sz)
    {
        m_spGUI->add(new GUI::Menu(
            asset_option_list,
            Freq::Timed<glm::vec2>(
                m_FadeTime,
                glm::vec2(0.0f, -m_vButtonSize.y),
                glm::vec2(0.0f, m_vButtonSize.y * (asset_option_list_sz*1.0f-1.0f))
            ),
            glm::vec2(256.0f, m_vButtonSize.y),
            GUI::F_AUTOREMOVE
        ));
    }
}

void EditorState :: xAddObject(GUI::Menu* menu, GUI::Menu::Option* op)
{
    Log::get().write(op->caption() + " clicked.");
    string folder_path = "data/ne/assets";
    
    list<GUI::Menu::Option> asset_option_list;
    size_t asset_option_list_sz = 0;
    
    try{
       boost::filesystem::directory_iterator end_itr;
       boost::filesystem::path asset_path(folder_path);
       for(boost::filesystem::directory_iterator dir_itr(asset_path);
           dir_itr != end_itr;
           ++dir_itr)
       {
           if(boost::filesystem::is_regular_file(dir_itr->status()))
           {
                //string fn = dir_itr->filename();
               string fn = dir_itr->path().filename().string();
               if(stringEndsWith(boost::to_lower_copy(fn), string(".obj")))
                   asset_option_list.push_back(GUI::Menu::Option(fn, bind(&EditorState::xAddByFilename, this, _1, _2, string(folder_path+"/"+fn))));
           }
       }
    }catch(const std::exception& ex){return;}
    
    asset_option_list_sz = asset_option_list.size();
    
    if(asset_option_list_sz)
    {
        m_spGUI->add(new GUI::Menu(
            asset_option_list,
            Freq::Timed<glm::vec2>(
                m_FadeTime,
                glm::vec2(0.0f, -m_vButtonSize.y),
                glm::vec2(0.0f, m_vButtonSize.y * (asset_option_list_sz*1.0f-1.0f))
            ),
            glm::vec2(256.0f, m_vButtonSize.y),
            GUI::F_AUTOREMOVE
        ));
    }
}

void EditorState :: xAddByFilename(GUI::Menu* menu, GUI::Menu::Option* op, std::string filename)
{
    Log::get().write(op->caption() + " clicked: " + filename);
    //m_spGUI->remove(menu);
    Node* n = m_spScene->loadModel(filename);
    m_Selection.clear();
    m_Selection.add(n);
    menu->flags(menu->flags() | GUI::F_REMOVE);
}



void EditorState::nullify()
{
    m_spScene = NULL;
    m_FadeTime = Freq::Time::seconds(0.25f);
    //m_fOverviewScale = 1.0f;
    m_OverviewScale.clear(25.0f);
    m_vView.clear(glm::vec3(0.0f));
    m_fEdgeScrollSpeed = 1000.0f;
    m_ZoomTime = 100;
    m_fZoomRatio = 1.5f;
    m_vButtonSize = glm::vec2(96.0f, 24.0f);

}

void EditorState::logic(unsigned int advance)
{
    float timestep = advance * 0.001f;
    Input* input = Engine::get().input();

    // is GUI "blocking" Input? if so do GUI logic and nothing else
    if(m_spGUI->blocking())
    {
        m_spGUI->logic(advance, input);
        return;
    }

    // perform logic for all objects in scene graph
    m_spScene->logic(advance);

    // toggle input modes (changes ortho/persp views in render())
    if(input->keyd(SDLK_TAB))
    {
        m_pPlayer->input(m_pPlayer->input() ? NULL : Engine::get().input());
        
        bool input_enabled = m_pPlayer->input();
        
        if(input_enabled) // in 3d mode
        {
            Renderer::get().lighting(Renderer::ENABLE_LIGHTING | Renderer::BIND_LIGHTING);
            Renderer::get().shaders(Renderer::ENABLE_SHADERS | Renderer::BIND_SHADERS);

            glm::vec3 mouse_world = mouseWorldSpace();
            //Log::get().write("Mouse World: " + str(mouse_world.x) + ", " + str(mouse_world.y));
            m_pPlayer->resetOrientation();
            Matrix::translation(*m_pPlayer->matrix(), glm::vec3(
               mouse_world.x,
               2.0f,
               mouse_world.y));
            //m_pPlayer->pendWorldMatrix();
            m_OverviewScale.finish();
            m_vView.clear(Matrix::translation(*m_pPlayer->matrix()));
        }
        else // in overview mode
        {
            Renderer::get().lighting(Renderer::DISABLE_LIGHTING);
            Renderer::get().shaders(Renderer::DISABLE_SHADERS);

            //m_pPlayer->resetOrientation();
            Matrix::translate(*m_pPlayer->matrix(), glm::vec3(
                -1.0f * Renderer::get().width() / 2.0f,
                0.0f,
                1.0f * Renderer::get().height() / 2.0f));
            //m_pPlayer->pendWorldMatrix();
            //m_OverviewScale.finish();
            m_vView.clear(Matrix::translation(*m_pPlayer->matrix()));
        }
        input->hideMouse(input_enabled);
    }

    if(m_pPlayer->input())
    {
        // 3D mode logic

        if(input->mouseMiddleClick() || input->mouseRightClick())
        {
            m_Selection.modify(glm::translate(glm::mat4(),
                glm::vec3(
                    0.0f,
                    ((input->mouseMiddleClick())?-1.0f:1.0f) *
                    ((input->key(SDLK_LSHIFT))?0.1f:1.0f) *
                    5.0f *
                    timestep,
                    0.0f
                )),
                Selection::MOD_WORLD
            );
        }
        return;
    }
     // did a GUI object claim focus (or respond to event) of the mouse?
    if(!m_spGUI->logic(advance, input))
    {
            
        if(input->mouseLeftClick())
        {
            // View Panning
            m_vView.shift(glm::vec3(
                -input->getMouseRelX(),
                0.0f,
                -input->getMouseRelY() 
            ));
            //m_vView.finish();
        }

        if(input->mouseRightRelease())
        {
            
            glm::vec3 clickpoint = mouseWorldSpaceXnZ();

            //Node* selection_node = NULL;
            //m_spScene->add(selection_node = new SelectorNode());
            //*selection_node->matrix() *= Matrix(Matrix::IDENTITY, clickpoint);
            
            //MatrixStack matrixstack;
            //matrixstack.identity();
            std::list<Node*> nodes;
            if(m_spScene->root()->vLineTest(nodes, clickpoint, Node::TEST_ONLY_VISIBLE | Node::USE_SUPERPARENT))
            {
                Log::get().write(str("Nodes clicked: ") + str(nodes.size()));
                nodes.sort(
                    [] (Node* a, Node* b) -> bool {
                        return a->box()->area() < b->box()->area();
                    }
                );
                
                Node* selected = nodes.back();
                //ASSERT(selected->superParent() == selected);
                
                if(!input->key(SDLK_LSHIFT) && !input->key(SDLK_RSHIFT))
                {
                    Log::get().write("New selection");
                    m_Selection.clear();
                    m_Selection.add(selected);
                }
                else
                {
                    Log::get().write("Toggled selection");
                    m_Selection.toggle(selected);
                }
                //selected->add(new SelectorNode());
            }
            else {
                // clear
                if(!input->key(SDLK_LSHIFT) && !input->key(SDLK_RSHIFT))
                    m_Selection.clear();
            }
        }
        else
        {
            // Screen Edge Scrolling
            //bool scrolled = true;

            //glm::vec3 mouse_local = mouseLocalSpace();
            //if(mouse_local.x < 1.0f || input->keyd(SDLK_s))
            //    m_pPlayer->matrix()->translate(glm::vec3(
            //        -m_fEdgeScrollSpeed*timestep,
            //        0.0f, 0.0f));
            //else if(mouse_local.x > Renderer::get().width()-1.0f || input->keyd(SDLK_f))
            //    m_pPlayer->matrix()->translate(glm::vec3(
            //        m_fEdgeScrollSpeed*timestep,
            //        0.0f, 0.0f));
            //else
            //    scrolled = false;

            //if(mouse_local.y <1.0f || input->keyd(SDLK_e))
            //    m_pPlayer->matrix()->translate(glm::vec3(
            //        0.0f,0.0f,
            //        m_fEdgeScrollSpeed*timestep));
            //else if(mouse_local.y > Renderer::get().height()-1.0f || input->keyd(SDLK_d))
            //    m_pPlayer->matrix()->translate(glm::vec3(GH
            //        0.0f,0.0f,
            //        -m_fEdgeScrollSpeed*timestep));
            //else
            //    scrolled = false;

            //if(scrolled)
            //{
            //    m_OverviewScale.finish();
            //    m_vView.clear(m_pPlayer->matrix()->translation());
            //}
        }
    }
    
    if(input->key(SDLK_r))
    {
        if(input->key(SDLK_LSHIFT) || input->key(SDLK_RSHIFT)) {
            m_Selection.identity();
        }
        else{
            float factor = 1.0f * fartherAbs(-input->getMouseRelX(), input->getMouseRelY());
            m_Selection.modify(glm::rotate(glm::mat4(), factor, Axis::Y));
            //else
                //m_Selection.modify(glm::rotate(glm::mat4(), -11.25f* m_pInput->getMouseRelX(), Axis::Y));
        }
    }
    if(input->key(SDLK_s))
    {
        float factor = 1.0f * fartherAbs(input->getMouseRelX(), input->getMouseRelY());
        if(!floatcmp(factor,0.0f)) {
            glm::mat4 m = glm::scale(glm::mat4(), (factor<0.0f)?glm::vec3(0.9f):glm::vec3(1.1f) * fabs(factor));
            //m = glm::scale(m, glm::vec3(fabs(factor)));
            //Log::get().write("matrix: " + Matrix::toString(m));
            Matrix::wScale(m,1.0f);
            m_Selection.modify(m);
        }
    }
    if(input->key(SDLK_g))
    {
        float scale = m_OverviewScale.get();
        if(!floatcmp(scale, 0.0f))
            m_Selection.modify(glm::translate(glm::mat4(),
                glm::vec3(
                    1.0f/scale * input->getMouseRelX(),
                    0.0f,
                    1.0f/scale * input->getMouseRelY()
                )),
                Selection::MOD_WORLD
            );
    }
    if(input->key(SDLK_x) || input->key(SDLK_DELETE))
        m_Selection.destroy();
    if(input->key(SDLK_a)) // instance
    {
        //m_Selection
    }

    // Mouse Wheel Zooming
    if(input->mouseWheelDownd())
    {
        //glm::vec3 mw = mouseWorldSpaceXZ();
        glm::vec3 m = mouseLocalSpaceXZ();
        //float old_scale = m_fOverviewScale;
        //m_fOverviewScale /= m_fZoomRatio;
        m_OverviewScale.set(Freq::Time(m_ZoomTime), m_OverviewScale.get(), m_OverviewScale.end() / m_fZoomRatio);
        m_vView.set(Freq::Time(m_ZoomTime), m_vView.get(), m_vView.end()/m_fZoomRatio + m*(1.0f/m_fZoomRatio - 1.0f));
        //m_OverviewScale.set(Freq::Time(max(m_ZoomTime, (unsigned int)m_OverviewScale.remainingTime())), m_OverviewScale.hasElapsed()?m_OverviewScale.end():m_OverviewScale.start(), m_OverviewScale.end() / m_fZoomRatio);
        //glm::vec3 trans =    m_pPlayer->matrix()->translation();
        //m_pPlayer->matrix()->setTranslation(trans/m_fZoomRatio + m*(1.0f/m_fZoomRatio - 1.0f));

        //m_pPlayer->matrix()->translate();
        //Log::get().write("m_fZoomRatio: " + str(1.0f/m_fOverviewScale));
        //Log::get().write("scale: " + str(m_fOverviewScale));
        //Log::get().write("m_local: " + str(m.x) + ", " + str(m.z));
    }
    else if(input->mouseWheelUpd())
    {
        //glm::vec3 mw = mouseWorldSpaceXZ();
        glm::vec3 m = mouseLocalSpaceXZ();
        //float old_scale = m_fOverviewScale;
        //m_fOverviewScale *= m_fZoomRatio;
        m_OverviewScale.set(Freq::Time(m_ZoomTime), m_OverviewScale.get(), m_OverviewScale.end() * m_fZoomRatio);
        m_vView.set(Freq::Time(m_ZoomTime), m_vView.get(), m_vView.end()*m_fZoomRatio + m*(m_fZoomRatio - 1.0f));
        //m_OverviewScale.set(Freq::Time(max(m_ZoomTime, (unsigned int)m_OverviewScale.remainingTime())), m_OverviewScale.hasElapsed()?m_OverviewScale.end():m_OverviewScale.start(), m_OverviewScale.end() * m_fZoomRatio);
        //glm::vec3 trans =    m_pPlayer->matrix()->translation();
        //m_pPlayer->matrix()->setTranslation(trans*m_fZoomRatio + m*(m_fZoomRatio-1.0f));

        //Log::get().write("m_fZoomRatio: " + str(m_fZoomRatio));
        //Log::get().write("scale: " + str(m_fOverviewScale));
        //Log::get().write("m_local: " + str(m.x) + ", " + str(m.z));
    }
    //m_pPlayer->matrix()->setTranslation(m_OverviewScale.hasElapsed()?m_OverviewScalem_OverviewScale.get());

    if(input->keyd(SDLK_RETURN))
    {
        m_pPlayer->resetOrientation();
        m_vView.clear(glm::vec3(0.0f));
    }

    if(input->keyd(SDLK_w))
    {
        Renderer::get().toggleWireframe();
        Renderer::get().toggleTextures();
    }

    m_pPlayer->position(m_vView.get());
    m_pPlayer->pendWorldMatrix();
    //m_spMusic->refresh();

    // set player as audio listener
    m_pPlayer->listen();
}

void EditorState::syncView() const
{
    m_pPlayer->position(m_vView.get());
}

void EditorState::render() const
{
    if(m_pPlayer->input())
    {
        Renderer::get().viewport(Renderer::VIEW_PERSPECTIVE, 0.0f, 0.0f, 1.0f, 1.0f);
        m_spScene->render(m_pPlayer);
    }
    else
    {
        syncView();

        Renderer::get().viewport(Renderer::VIEW_ORTHO);
        if(!m_pPlayer->input())
            m_spScene->renderOverview(
                Matrix::translation(*m_pPlayer->matrix_c()),
                m_OverviewScale.get()
            );

        glLoadIdentity();
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        m_spGUI->render();
        glEnable(GL_DEPTH_TEST);
    }

    //try{
    //    boost::filesystem::directory_iterator end_itr;
    //    boost::filesystem::path asset_path("data/assets");
    //    int spacing = 0;
    //    for(boost::filesystem::directory_iterator dir_itr(asset_path);
    //        dir_itr != end_itr;
    //        ++dir_itr)
    //    {
    //        if(boost::filesystem::is_regular_file(dir_itr->status()))
    //        {
    //            string fn = dir_itr->filename();
    //            if(stringEndsWith(boost::to_lower_copy(fn), string(".obj")))
    //            {
    //                m_spFont->Render(fn.c_str(), -1, FTPoint(0.0, spacing*1.0));
    //                spacing += 12;
    //            }
    //        }
    //    }
    //}catch(const std::exception& ex)
    //{
        
    //}

    //glColor4f(1.0f,1.0f,1.0f,1.0f);
    // debug info
    //string s = "mouse= " + str(Engine::get().input()->getMouseX()) + str(", ") +
    //    str(Renderer::get().height()-Engine::get().input()->getMouseY());
    //glm::vec3 mouse_world = mouseWorldSpace();
    //glm::vec3 v = m_pPlayer->matrix_c()->translation();
    //s+= str(" ||| view= ") + str(v.x) + string(", ") + str(v.z);
    //s+= str(" ||| mouse'= ") + str(mouse_world.x) + string(", ") + str(mouse_world.y);
    ////s+= str(" ||| view'= ") + str(v.x/m_fOverviewScale) + string(", ") + str(v.z/m_fOverviewScale);
    //m_spFont->Render(s.c_str());
}

glm::vec3 EditorState :: mouseLocalSpace() const
{
    return glm::vec3(float(Engine::get().input()->getMouseX()),
        float(Renderer::get().height()-Engine::get().input()->getMouseY()-1.0f),
        0.0f);
}

glm::vec3 EditorState :: mouseLocalSpaceXZ() const
{
    return glm::vec3(float(Engine::get().input()->getMouseX()),
        0.0f,
        -float(Renderer::get().height()-Engine::get().input()->getMouseY()-1.0f));
}

glm::vec3 EditorState :: mouseWorldSpace() const
{
    glm::vec3 v = Matrix::translation(*m_pPlayer->matrix_c());
    glm::vec3 m = mouseLocalSpace();
    float s = m_OverviewScale.get();
    return glm::vec3(
        (m.x + v.x)/s,
        -(m.y - v.z)/s,
        0.0f
    );
}

glm::vec3 EditorState :: mouseWorldSpaceXZ() const
{
    glm::vec3 mw = mouseWorldSpace();
    mw.z = -mw.y;
    mw.y = 0.0f;
    return mw;
}

glm::vec3 EditorState :: mouseWorldSpaceXnZ() const
{
    glm::vec3 mw = mouseWorldSpace();
    mw.z = mw.y;
    mw.y = 0.0f;
    return mw;
}

