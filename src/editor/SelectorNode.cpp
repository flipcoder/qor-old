#include "../GfxAPI.h"
#include "../Log.h"
#include "SelectorNode.h"
#include "../Renderer.h"

void SelectorNode :: logicSelf(unsigned int advance)
{
    //glm::vec3 trans = m_pParent->matrix_c()->translation();
    //Log::get().write("parent_trans: " + str(trans.x) + ", " + str(trans.z));
}


void SelectorNode :: renderSelf(Frustum* view, unsigned int flags) const
{
    if(!(flags & RENDER_AMBIENT_PASS) || !(flags & RENDER_INDICATORS))
        return;

    glDisable(GL_DEPTH_TEST);
    
    glm::vec3 o(0.0f);
    glm::vec3 v(1.0f);
    glBegin(GL_LINES);
        glColor3fv(glm::value_ptr(glm::vec3(v.x,0.0f,0.0f)));
        glVertex3fv(glm::value_ptr(o));
        glVertex3fv(glm::value_ptr(glm::vec3(v.x,0.0f,0.0f)));
        glColor3fv(glm::value_ptr(glm::vec3(0.0f,v.y,0.0f)));
        glVertex3fv(glm::value_ptr(o));
        glVertex3fv(glm::value_ptr(glm::vec3(0.0f,v.y,0.0f)));
        glColor3fv(glm::value_ptr(glm::vec3(0.0f,0.0f,v.z)));
        glVertex3fv(glm::value_ptr(o));
        glVertex3fv(glm::value_ptr(glm::vec3(0.0f,0.0f,v.z)));
    glEnd();
    
    //glColor3f(1.0f, 1.0f, 1.0f);
    //if(getParent_c() && getParent_c()->box())
    //    Renderer::get().drawLineBox(getParent_c()->box()->min, getParent_c()->box()->max);
    
    glEnable(GL_DEPTH_TEST);
}

