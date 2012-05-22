
#include "Indicator.h"
#include "Renderer.h"

void Indicator :: renderSelf(IPartitioner* partitioner, unsigned int flags) const
{
    if(!(flags & RENDER_AMBIENT_PASS))
        return;

    Renderer::get().shaders(Renderer::UNBIND_SHADERS);
    Renderer::get().lighting(Renderer::UNBIND_LIGHTING);
    
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    m_spTexture->bind();

    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    //glBlendFunc(GL_SRC_ALPHA_SATURATE, GL_ONE);
    //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
    //glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
    //glBlendFunc (GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
    //glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, true);
    //glm::vec3 quadratic(0.0f, 0.0f, 0.01f);
    //glm::vec3 min(0.01f);
    //glm::vec3 max(1000.0f);
    //glPointParameterfv(GL_POINT_SIZE_MIN, min.array());
    //glPointParameterfv(GL_POINT_SIZE_MAX, max.array());
    //glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, quadratic.array());

    //glEnable(GL_POINT_SPRITE);
    glPointSize(24.0f);
    glBegin(GL_POINTS);
        glVertex3f(0.0f, 0.0f, 0.0f);
    glEnd();

    //glDisable(GL_POINT_SPRITE);
    glDisable(GL_BLEND);

    glDepthMask(GL_TRUE);

    Renderer::get().lighting(Renderer::BIND_LIGHTING);
    Renderer::get().shaders(Renderer::BIND_SHADERS);
}

