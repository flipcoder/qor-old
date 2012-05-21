#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "Texture.h"
#include "Graphics.h"
#include "Renderer.h"
#include "ResourceCache.h"
#include "IFallible.h"

#include <string>
using namespace std;

class Material
{
public:
    /*enum eLayers {
        L_DIFFUSE,
        L_BUMP
    };*/

private:

    std::shared_ptr<Texture> m_spTexture; // diffuse
    std::shared_ptr<Texture> m_spBump; // bump
    std::shared_ptr<Texture> m_spDisp; // displacement
    std::shared_ptr<Texture> m_spSpec; // specular/gloss map
    std::shared_ptr<Texture> m_spOcc; // occlusion map
    Color m_Specular;
    Color m_Shininess;
    Color m_Color;

    static std::shared_ptr<Texture> allocateTex(std::string filename, std::string ext, ResourceCache<Texture>& rmap);
    
public:
    Material(
        std::shared_ptr<Texture>& tex
    ):
        m_spTexture(tex)
    {}

    Material(
        std::shared_ptr<Texture>& tex,
        std::shared_ptr<Texture>& bump,
        std::shared_ptr<Texture>& disp,
        std::shared_ptr<Texture>& spec,
        std::shared_ptr<Texture>& occ
    ):
        m_spTexture(tex),
        m_spBump(bump),
        m_spDisp(disp),
        m_spSpec(spec),
        m_spOcc(occ)
    {}
    
    virtual ~Material();
    bool good() { return m_spTexture?m_spTexture->good():false; }
    Texture* getTexture() { return m_spTexture.get(); }
    //Texture* getBump() { return m_spBump.get(); }
    //bool hasBump() { return m_spBump.get(); }
    unsigned int bind(unsigned int layer = 0) {
        glEnable(GL_COLOR_MATERIAL);
        glm::vec4 full(1.0f);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_Specular.array());
        m_Shininess.set(0.1f, 1.0f);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, m_Shininess.array());
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glm::value_ptr(full));
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, glm::value_ptr(full));
        unsigned offset = 0;
        glColor4fv(m_Color.array());
        m_spTexture->bind(layer + offset++);
        if(Renderer::get().shadersBound())
        {
            if(m_spBump)
                m_spBump->bind(layer + offset++);
            if(m_spDisp)
                m_spDisp->bind(layer + offset++);
            if(m_spSpec)
                m_spSpec->bind(layer + offset++);
            if(m_spOcc)
                m_spOcc->bind(layer + offset++);
        }
        return offset;
    }
    static std::shared_ptr<Material> allocate(std::string fn, ResourceCache<Texture>& rmap);
};

#endif

