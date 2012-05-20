#include "Light.h"

void Light :: renderSelf(IPartitioner* partitioner, unsigned int flags = 0) const {
    if(!(flags & RENDER_AMBIENT_PASS))
        return;

    Renderer::get().shaders(Renderer::UNBIND_SHADERS);
    Renderer::get().lighting(Renderer::UNBIND_LIGHTING);
    
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    //glDepthMask(GL_FALSE);
    //glEnable(GL_BLEND);
    
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

    Renderer::get().lighting(Renderer::BIND_LIGHTING);
    Renderer::get().shaders(Renderer::BIND_SHADERS);    
    //glColor3f(1.0f, 1.0f, 1.0f);
    //if(getParent_c() && getParent_c()->box())
    //    Renderer::get().drawLineBox(getParent_c()->box()->min, getParent_c()->box()->max);
    
    //glEnable(GL_DEPTH_TEST);            
    //if(id)
    //{
    //    glPushMatrix();
    //    glLoadIdentity();
    //        GLenum gl_light_idx = GL_LIGHT0 + id - 1;
    //        glLightNodefv(gl_light_idx, GL_AMBIENT, ambient.array());
    //        glLightNodefv(gl_light_idx, GL_DIFFUSE, diffuse.array());
    //        glLightNodefv(gl_light_idx, GL_SPECULAR, specular.array());
    //        //glLightNodefv(gl_light_idx, GL_SPOT_DIRECTION, glm::value_ptr(Axis::nZ));
            
    //        glm::vec4 pos = glm::vec4(Matrix::translation(*matrix_c()), 1.0);
    //        //glm::vec4 pos(1.0f);
    //        glLightNodefv(gl_light_idx, GL_POSITION, glm::value_ptr(pos));
    //        //glLightNodef(gl_light_idx, GL_CONSTANT_ATTENUATION, 2.0);
    //        //glLightNodef(gl_light_idx, GL_LINEAR_ATTENUATION, 1.0);
    //        //glLightNodef(gl_light_idx, GL_QUADRATIC_ATTENUATION, 0.2);
    //        glEnable(gl_light_idx);
    //    glPopMatrix();
    //}
}
