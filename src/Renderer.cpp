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

#define DIST_NEAR_PLANE 0.01f
//#define DIST_FAR_PLANE 1000.0f
#define DIST_FAR_PLANE -1.0f

#define DEFAULT_FOV 60.0f //70

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

void Renderer :: nullify()
{
    m_fFOV = DEFAULT_FOV;
    m_bError = false;
    m_bWireframe = false;
    m_bTextures = true;
    m_bShaders = false;
    m_bLighting = true;
    m_ViewMatrixUniform = 0;
}

bool Renderer :: setDisplayMode()
{
    unsigned int flags = SDL_OPENGL;

    if(m_VideoInfo.fullscreen)
        flags |= SDL_FULLSCREEN | SDL_HWSURFACE;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        m_bError = true;
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    //SDL_GL_SetSwapInterval(0); // vsync
    //SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);  // Force VSync
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // AA x4
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // AA x4

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
    glFlush();
    SDL_GL_SwapBuffers();
    clear();
}


bool Renderer :: startGL()
{
    if(glewInit()!=GLEW_OK)
        return false;

    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    //glEnable(GL_POINT_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glEnable(GL_POINT_SPRITE);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glCullFace(GL_BACK);
    glDepthFunc(GL_LEQUAL);
    //glEnable(GL_MULTISAMPLE);
    
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

    // Load Shader Pair
    std::shared_ptr<Shader> vertShader(new Shader());
    if(!vertShader->load("data/base/shaders/bump.vp", Shader::VERTEX)){
        Log::get().error("Failed to load vertex shader.");
        return false;
    }
    std::shared_ptr<Shader> fragShader(new Shader());
    if(!fragShader->load("data/base/shaders/bump.fp", Shader::FRAGMENT)){
        Log::get().error("Failed to load fragment shader.");
        return false;
    }
    m_spProgram.reset(new Program());
    m_spProgram->attach(vertShader);
    m_spProgram->attach(fragShader);

    m_spProgram->attribute(1, "tangent");
    m_spProgram->attribute(6, "bitangent");
    
    if(!m_spProgram->link())
    {
        Log::get().error("Failed to link shader program.");
        return false;
    }
    if(!m_spProgram->use())
    {
        Log::get().error("Failed to bind shader program.");
        return false;
    }
    
    m_TextureUniform[0] = m_spProgram->uniform("tex");
    m_TextureUniform[1] = m_spProgram->uniform("nmap");
    m_TextureUniform[2] = m_spProgram->uniform("disp");
    m_TextureUniform[3] = m_spProgram->uniform("spec");
    m_TextureUniform[4] = m_spProgram->uniform("occ");
    
    m_ViewMatrixUniform = m_spProgram->uniform("ViewMatrix");

    m_LightUniform[0].vec = m_spProgram->uniform("Light");
    m_LightUniform[0].atten = m_spProgram->uniform("LightAtten");
    m_LightUniform[0].color = m_spProgram->uniform("LightColor");

    shaders(ENABLE_SHADERS | BIND_SHADERS);
    
    m_Flags |= TEX_BUMP;
    
    return true;
}

void Renderer :: endGL()
{
    m_spProgram.reset();
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
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glLoadIdentity(); // important for logic calls invoking GL
}

bool Renderer :: screenshot() const
{
    // TODO:...
    return false;
}

void Renderer :: wireframe(bool t)
{
    if(t) {
        m_bWireframe = true;
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else{
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
    if(!m_spProgram)
        return;
    ASSERT(layer < MAX_LIGHT_PER_PASS);

    glm::vec3 color_vec(c.r, c.g, c.b);

    m_spProgram->uniform(m_LightUniform[layer].vec, vec);
    m_spProgram->uniform(m_LightUniform[layer].atten, atten);
    m_spProgram->uniform(m_LightUniform[layer].color, color_vec);
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
        if(m_spProgram)
            if((int)m_TextureUniform[layer] > -1)
                m_spProgram->uniform(m_TextureUniform[layer], (int)layer);
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
                if(m_spProgram)
                    m_spProgram->uniform(m_TextureUniform[i], 0);
                glDisable(GL_TEXTURE_2D);
            }
        }
        glActiveTexture(GL_TEXTURE0);
    }
}

