#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED

#include "GfxAPI.h"
#include "Texture.h"
#include "Shader.h"
#include "IStaticInstance.h"
#include <vector>
#include <string>
#include <memory>
#include "math/common.h"
//#include "math/vector2.h"
//#include "math/vector3.h"
#include "Util.h"
#include "Shader.h"
#include "Graphics.h"

struct VideoResolution_t{
    int w, h, bpp;
};


class Renderer : public IStaticInstance<Renderer>
{
    public:

        class VideoInfo
        {
            public:
                VideoInfo():
                    w(1024),
                    h(768),
                    bpp(32),
                    fullscreen(false),
                    
                    caption(std::string("")) {}
                    
                VideoInfo(int iw,int ih,int ibpp,bool bfullscreen,std::string scaption):
                    w(iw),
                    h(ih),
                    bpp(ibpp),
                    fullscreen(bfullscreen),
                    caption(scaption) {}
                    
            
                int w,h,bpp;
                bool fullscreen;
                std::string caption;
                
                void set(VideoInfo *info)
                {
                    w=info->w;
                    h=info->h;
                    bpp=info->bpp;
                    fullscreen=info->fullscreen;
                    caption=info->caption;
                }

                void set(int _w, int _h, int _b, bool _fs, std::string _caption)
                {
                    w=_w; h=_h; bpp=_b; fullscreen=_fs; caption=_caption;
                }
                
                static VideoInfo newVideoInfo(int iw, int ih, int ibpp, bool bfullscreen, std::string scaption)
                {
                    VideoInfo info;
                    
                    info.w = iw;
                    info.h = ih;
                    info.bpp = ibpp;
                    info.fullscreen = bfullscreen;
                    info.caption = scaption;
                    
                    return info;
                }
        };
        
        enum eView
        {
            VIEW_ORTHO,
            VIEW_PERSPECTIVE
        };
    
    private:
        
        VideoInfo m_VideoInfo;
        bool m_bError;
        bool m_bWireframe;
        bool m_bTextures;
        bool m_bLighting;
        bool m_bLightingState;
        bool m_bShaders;

        float m_fFOV;
        
        bool setDisplayMode();

        std::unique_ptr<Program> m_spProgram;
        std::vector<Texture*> m_TextureSlots;
        std::vector<Program::UniformID> m_TextureUniform;
        Program::UniformID m_ViewMatrixUniform;

        struct LightData
        {
            LightData():
                vec(0),
                atten(0),
                color(0)
            {}
            //glm::vec4 vec;
            //glm::vec3 atten;
            //Color color;
            Program::UniformID vec;
            Program::UniformID atten;
            Program::UniformID color;
        };
        std::vector<LightData> m_LightUniform;
        glm::mat4 m_ViewMatrix;

        unsigned int m_Flags;
        
    public:
        
        Renderer();
        Renderer(int w, int h, int b, bool fs, std::string caption);

        //static Renderer* newVideo();
        //static Renderer* newVideo(int w, int h, int b, bool fs, std::string caption);
        
        virtual ~Renderer();
        void nullify();
        
        bool error() const { return m_bError; }
        
        void draw();
        
        void changeSettings(VideoInfo* info) { m_VideoInfo.set(info); }
        
        bool isResolutionSupported(int w, int h, int b){
            unsigned int r;
            r = SDL_VideoModeOK(w, h, b, SDL_OPENGL);
            if(r)
                return true;
            return false;
        }
        
        VideoInfo *getVideoInfo() { return &m_VideoInfo; }
        
        int width() const { return m_VideoInfo.w; }
        int height() const { return m_VideoInfo.h; }
        glm::vec2 size() const {
            return glm::vec2(
                1.0f*m_VideoInfo.w,
                1.0f*m_VideoInfo.h
            );
        }

        void setFOV(float f) { m_fFOV = f; }
        float getFOV() { return m_fFOV; }
        
        static bool startIL();
        bool startGL();
        void endGL();
        void clear();
        void viewport(eView vm, float x = 0.0f, float y = 0.0f, float w = 1.0f, float h = 1.0f);

        void perspective(float aspect);

        void wireframe(bool t);
        bool wireframe() const { return m_bWireframe; }
        void toggleWireframe();

        void textures(bool t);
        bool textures() const { return m_bTextures; }
        void toggleTextures();
        
        bool screenshot() const;

        enum {
            UNBIND_LIGHTING = BIT(0),
            BIND_LIGHTING = BIT(1),
            DISABLE_LIGHTING = BIT(2),
            ENABLE_LIGHTING = BIT(3)
        };
        void lighting(unsigned int flags) {
            
            if(flags & ENABLE_LIGHTING)
            {
                glDisable(GL_LIGHTING); //yes I typed DISABLE on purpose
                m_bLighting = true;
            }
            else if(flags & DISABLE_LIGHTING)
            {
                glDisable(GL_LIGHTING);
                m_bLighting = false;
            }
            
            if(m_bLighting)
            {
                if(flags & BIND_LIGHTING)
                {
                    glEnable(GL_LIGHTING);
                    m_bLightingState = true;
                }
                else if(flags & UNBIND_LIGHTING)
                {
                    glDisable(GL_LIGHTING);
                    m_bLightingState = false;
                }
            }
        }
        enum {
            UNBIND_SHADERS = BIT(0),
            BIND_SHADERS = BIT(1),
            DISABLE_SHADERS = BIT(2),
            ENABLE_SHADERS = BIT(3)
        };
        void shaders(unsigned int flags) {
            
            if(flags & ENABLE_SHADERS)
            {
                if(m_spProgram)
                {
                    m_bShaders = true;
                    m_spProgram->use();
                }
            }
            else if(flags & DISABLE_SHADERS)
            {
                Program::unuseAll();
                m_bShaders = false;
            }
            
            if(m_bShaders && m_spProgram)
            {
                if(flags & BIND_SHADERS)
                {
                    m_spProgram->use();
                }
                else if(flags & UNBIND_SHADERS)
                {
                    Program::unuseAll();
                }
            }
        }
        bool shaders() const {
            return m_bShaders && m_spProgram;
        }
        
        void setViewUniform() {
            if(m_spProgram)
            {
                glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(m_ViewMatrix));
                m_spProgram->uniform(m_ViewMatrixUniform, m_ViewMatrix);
            }
        }
        
        //glm::mat4 getModelView() {
        //    glm::mat4 mat;
        //    glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(mat));
        //}
        
        //void getModelView(glm::mat4& mat) {
        //    glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(mat));
        //}

        const glm::mat4* getViewMatrix() const {
            return &m_ViewMatrix;
        }

        void bindLight(glm::vec4& vec, glm::vec3& atten, Color& color, unsigned int layer = 0);
        void unbindLights();

        void bindTexture(Texture* t, unsigned int layer = 0);
        //void unbindTextures(unsigned int nlayers = 1);
        void unbindTextures();

        enum {
            TEX_BUMP = BIT(0)
        };
        unsigned int enabled(unsigned int flags) const {
            return m_Flags & flags;
        }
        
        void drawLine(const glm::vec3& a, const glm::vec3& b) const
        {
            glBegin(GL_LINES);
                glVertex2fv(glm::value_ptr(a));
                glVertex2fv(glm::value_ptr(b));
            glEnd();
        }
        
        void drawLineBox(const glm::vec3& min, const glm::vec3& max) const
        {
            glBegin(GL_LINES);
               //glNormal3f( 0.0f, 0.0f,max.z);
               glVertex3f(min.x, min.y, max.z);
               glVertex3f(max.x, min.y, max.z);
               glVertex3f(max.x, max.y, max.z);
               glVertex3f(min.x, max.y, max.z);
               // Back Face
               //glNormal3f( 0.0f, 0.0f,min.z);
               glVertex3f(min.x, min.y, min.z);
               glVertex3f(min.x, max.y, min.z);
               glVertex3f(max.x, max.y, min.z);
               glVertex3f(max.x, min.y, min.z);
               // Top Face
               //glNormal3f( 0.0f,max.y, 0.0f);
               glVertex3f(min.x, max.y, min.z);
               glVertex3f(min.x, max.y, max.z);
               glVertex3f(max.x, max.y, max.z);
               glVertex3f(max.x, max.y, min.z);
               // Bottom Face
               //glNormal3f( 0.0f,min.y, 0.0f);
               glVertex3f(min.x, min.y, min.z);
               glVertex3f(max.x, min.y, min.z);
               glVertex3f(max.x, min.y, max.z);
               glVertex3f(min.x, min.y, max.z);
               // Right face
               //glNormal3f(max.y, 0.0f, 0.0f);
               glVertex3f(max.x, min.y, min.z);
               glVertex3f(max.x, max.y, min.z);
               glVertex3f(max.x, max.y, max.z);
               glVertex3f(max.x, min.y, max.z);
               // Left Face
               //glNormal3f(min.y, 0.0f, 0.0f);
               glVertex3f(min.x, min.y, min.z);
               glVertex3f(min.x, min.y, max.z);
               glVertex3f(min.x, max.y, max.z);
               glVertex3f(min.x, max.y, min.z);
            glEnd();
        }
};


#endif

