#include "Renderer.h"
#include "GfxAPI.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include "math/common.h"
#include "Shader.h"
#include "Log.h"
#include "Util.h"
#include "Light.h"

#include <iostream>
using namespace std;

const unsigned int MAX_TEXTURE_SLOTS = 8;
const unsigned int MAX_LIGHT_PER_PASS = 1;

// Renderer Class

Renderer :: Renderer()
{
    nullify();
    setDisplayMode();
}

Renderer :: Renderer(int w,int h,int b,bool fs,std::string caption)
{
    nullify();
    m_VideoInfo.set(w,h,b,fs,caption);
    setDisplayMode();
}

Renderer :: ~Renderer()
{
    endGL();
    SDL_Quit();
}

bool Renderer :: setDisplayMode()
{
    unsigned int flags = SDL_OPENGL;

    if(m_VideoInfo.fullscreen)
        flags |= SDL_FULLSCREEN;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        m_bError = true;
        return false;
    }

    //SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    //SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    //SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    //SDL_GL_SetSwapInterval(0); // vsync
    //SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);  // Force VSync
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // AA x4
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // AA x4

    SDL_Surface* screen = SDL_SetVideoMode(m_VideoInfo.w, m_VideoInfo.h, m_VideoInfo.bpp, flags);
    if (!screen)
    {
        m_bError = true;
        return false;
    }

    SDL_WM_SetCaption(m_VideoInfo.caption.c_str(),NULL);

    return true;
}

bool Renderer :: startIL()
{
    if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
        return false;
    ilInit();
    iluInit();
    return !ilGetError();
}

void Renderer :: draw()
{
    //glFinish();
    SDL_GL_SwapBuffers();
    SDL_Delay(1);
    clear();
}


bool Renderer :: startGL()
{
    if(glewInit()!=GLEW_OK)
        return false;

    glShadeModel(GL_SMOOTH);
    //glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glEnable(GL_POINT_SPRITE);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    //glEnable(GL_POLYGON_SMOOTH); //!
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LEQUAL);
    //glEnable(GL_MULTISAMPLE);
    glLineWidth(8.0f);
    
    // point sprite settings
    glm::vec4 quadratic(0.0f, 0.0f, 0.01f, 0.0f);
    //glm::vec3 min(0.01f);
    //glm::vec3 max(1000.0f);
    glPointParameterf(GL_POINT_SIZE_MIN, 0.0f);
    glPointParameterf(GL_POINT_SIZE_MAX, 1000.0f);
    glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, glm::value_ptr(quadratic));
    // shaders
    
    m_TextureSlots.resize(MAX_TEXTURE_SLOTS, 0); // fill blank
    m_TextureUniform.resize(MAX_TEXTURE_SLOTS, 0); // fill blank
    m_LightUniform.resize(MAX_LIGHT_PER_PASS);
    
    // Load Shader Pairs
    if(!loadShaderPair(m_spDefaultProgram, "bump"))
        return false;
    if(!loadShaderPair(m_spBaseProgram, "depth"))
        return false;
    m_pProgram = m_spDefaultProgram.get();
        
    m_TextureUniform[0] = m_pProgram->uniform("tex");
    m_TextureUniform[1] = m_pProgram->uniform("nmap");
    m_TextureUniform[2] = m_pProgram->uniform("disp");
    m_TextureUniform[3] = m_pProgram->uniform("spec");
    //m_TextureUniform[4] = m_pProgram->uniform("occ");
    
    m_ViewMatrixUniform = m_pProgram->uniform("ViewMatrix");

    m_LightUniform[0].vec = m_pProgram->uniform("Light");
    m_LightUniform[0].atten = m_pProgram->uniform("LightAtten");
    m_LightUniform[0].color = m_pProgram->uniform("LightColor");

    shaders(ENABLE_SHADERS | BIND_SHADERS);
    
    m_Flags |= TEX_BUMP;
    
    return true;
}

bool Renderer :: loadShaderPair(std::unique_ptr<Program>& program, const std::string& shader_name) {
    std::unique_ptr<Shader> vertShader(new Shader());
    if(!vertShader->load("data/base/shaders/"+shader_name+".vp", Shader::VERTEX)){
        Log::get().error("Failed to load vertex shader.");
        return false;
    }
    std::unique_ptr<Shader> fragShader(new Shader());
    if(!fragShader->load("data/base/shaders/"+shader_name+".fp", Shader::FRAGMENT)){
        Log::get().error("Failed to load fragment shader.");
        return false;
    }
    program.reset(new Program());
    program->attach(std::move(vertShader));
    program->attach(std::move(fragShader));
    
    program->attribute(1, "tangent");
    program->attribute(6, "bitangent");
    
    if(!program->link())
    {
        Log::get().error("Failed to link shader program.");
        return false;
    }
    if(!program->use())
    {
        Log::get().error("Failed to bind shader program.");
        return false;
    }
    return true;
}

void Renderer :: endGL()
{
    m_spDefaultProgram.reset();
    m_spBaseProgram.reset();
    m_pProgram = NULL;
}

void Renderer :: viewport(eView vm, float x, float y, float w, float h)
{
    //glClearColor(0.5f, 0.5f, 0.8f, 1.0f);
    //glClearDepth(1.0f);
    glViewport((GLsizei)(m_VideoInfo.w*x),
            (GLsizei)(m_VideoInfo.h*y),
            (GLsizei)(m_VideoInfo.w*w),
            (GLsizei)(m_VideoInfo.h*h));

    float viewport_width = m_VideoInfo.w*w;
    float viewport_height = m_VideoInfo.h*h;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    switch(vm)
    {
        case VIEW_ORTHO:
        {
            glOrtho(0,viewport_width,
                0,viewport_height,
                -1000.0, 1000.0f); //1.0f
            break;
        }
        case VIEW_PERSPECTIVE:
        {
            perspective(viewport_width / viewport_height);
            break;
        }
        default:
            break;
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Renderer :: perspective(float aspect)
{
    //Matrix m;
    //m.perspective(m_fFOV,
    //        (float)m_VideoInfo.w/(float)m_VideoInfo.h,
    //        DIST_NEAR_PLANE,
    //        DIST_FAR_PLANE);
    //m.glMultMatrix();
    gluPerspective(m_fFOV,
            aspect,
            DIST_NEAR_PLANE,
            DIST_FAR_PLANE);
}

void Renderer :: clear()
{
    glClear(GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT |
            GL_STENCIL_BUFFER_BIT);
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glLoadIdentity(); // important for logic calls invoking GL
}

bool Renderer :: screenshot() const
{
    // TODO:...
    return false;
}

void Renderer :: wireframe(bool t)
{
    //static bool light_state = false;
    //static bool shader_state = false;
    if(t) {
        //light_state = pauseLighting();
        //shader_state = pauseShaders();
        m_bWireframe = true;
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else{
        //resumeLighting(light_state);
        //resumeShaders(shader_state);
        m_bWireframe = false;
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void Renderer :: toggleWireframe()
{
    wireframe(!m_bWireframe);
}

void Renderer :: textures(bool t)
{
    m_bTextures = t;
    if(!m_bTextures)
        glDisable(GL_TEXTURE_2D);
}

void Renderer :: toggleTextures()
{
    textures(!m_bTextures);
}

void Renderer :: bindLight(glm::vec4& vec, glm::vec3& atten, Color& c, unsigned int layer)
{
    if(!m_pProgram)
        return;
    ASSERT(layer < MAX_LIGHT_PER_PASS);

    glm::vec3 color_vec(c.r, c.g, c.b);

    m_pProgram->uniform(m_LightUniform[layer].vec, vec);
    m_pProgram->uniform(m_LightUniform[layer].atten, atten);
    m_pProgram->uniform(m_LightUniform[layer].color, color_vec);
}

void Renderer :: unbindLights()
{
    // pointless until we add multiple lights
}

void Renderer :: bindTexture(Texture* t, unsigned int layer)
{
    if(m_bTextures)
    {
        ASSERT(t);
        ASSERT(layer >= 0 || layer < m_TextureSlots.size());
        //ASSERT(layer >= 0 || layer < 2);
        glActiveTexture(GL_TEXTURE0 + layer);
        m_TextureSlots[layer] = t;
        glBindTexture(GL_TEXTURE_2D, t->id);
        if(m_pProgram)
            if((int)m_TextureUniform[layer] >= 0)
                m_pProgram->uniform(m_TextureUniform[layer], (int)layer);
        glEnable(GL_TEXTURE_2D);
    }
}

void Renderer :: unbindTextures()
{
    if(m_bTextures)
    {
        for(unsigned int i=0; i < m_TextureSlots.size(); ++i) {
            if(m_TextureSlots[i])
            {
                glActiveTexture(GL_TEXTURE0 + i);
                m_TextureSlots[i] = NULL;
                glBindTexture(GL_TEXTURE_2D, 0);
                if(m_pProgram)
                    m_pProgram->uniform(m_TextureUniform[i], 0);
                glDisable(GL_TEXTURE_2D);
            }
        }
        glActiveTexture(GL_TEXTURE0);
    }
}

