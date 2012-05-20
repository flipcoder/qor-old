#ifndef _RENDERBUFFER_H
#define _RENDERBUFFER_H

#include "GfxAPI.h"
#include "Texture.h"

class RenderBuffer
{
private:
    unsigned int m_FrameBufferID;
    unsigned int m_DepthBufferID;
    Texture m_Texture;
    unsigned int m_Status;
    unsigned int width;
    unsigned int height;

public:
    RenderBuffer(int _width, int _height){
        m_FrameBufferID = 0;
        m_DepthBufferID = 0;
        m_Status = 0;
        m_Texture.unload();
        width = _width;
        height = _height;

        glGenFramebuffers(1, &m_FrameBufferID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
        glGenRenderbuffers(1, &m_DepthBufferID);
        glBindRenderbuffer(GL_RENDERBUFFER, m_DepthBufferID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthBufferID);
        glGenTextures(1, &m_Texture.id);
        glBindTexture(GL_TEXTURE_2D, m_Texture.id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        // glTexParameters, glGenerateMipmap
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture.id, 0);

        status();
        ASSERT(m_Status == GL_FRAMEBUFFER_COMPLETE);
    }
    virtual ~RenderBuffer(){
        if(m_FrameBufferID)
            glDeleteFramebuffers(1, &m_FrameBufferID);
        if(m_DepthBufferID)
            glDeleteRenderbuffers(1, &m_DepthBufferID);
    }
    void bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
    }
    static void unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void pushViewport(int x, int y, int w, int h){
        bind();
        glPushAttrib(GL_VIEWPORT_BIT);
        glViewport(x,y,w,h);
    }
    void pushViewport(int x = 0, int y = 0) {
        pushViewport(x,y,width,height);
    }
    static void popViewport() {
        glPopAttrib();
        unbind();
    }

    bool status() {
        m_Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        return m_Status == GL_FRAMEBUFFER_COMPLETE;
    }
    bool good() {
        return m_FrameBufferID &&
               m_DepthBufferID &&
               m_Texture.good() &&
               m_Status==GL_FRAMEBUFFER_COMPLETE;
    }
    bool bad() { return !good(); }
    Texture* texture() { return &m_Texture; }
};

#endif

