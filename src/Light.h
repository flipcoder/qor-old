#ifndef _LIGHTNODE_H
#define _LIGHTNODE_H

#include "Graphics.h"
#include "Node.h"
#include "math/common.h"
#include "Renderer.h"
#include "Indicator.h"

class Light : public Indicator
{
    public:

        enum Type
        {
            NO_TYPE = 0, // don't change this
            POINT,
            DIRECTIONAL,
            SPOT
        };
        
        Light():
            m_Type(POINT),
            m_Atten(glm::vec3(1.0f, 0.0f, 0.0f)),
            m_LightFlags(0)
        {
            // TODO: Use light cutoffs for AABB, instead of temp:
            m_Box.min = glm::vec3(-0.5f);
            m_Box.max = glm::vec3(0.5f);
        }
        virtual ~Light() {}
        
        // bind: to be called only by Scene during a render
        //  id is generated at time of render and may change!
        bool bind(unsigned int id)
        {
            if(visible() && lightType())
            {
                // bind this light to shader uniform here (using id given)
                
                // determine "vec" to be position or direction based on light type
                glm::vec4 vec;
                Type light_type = lightType();
                if(light_type == POINT || light_type == SPOT)
                {
                    vec = glm::vec4(position(Space::WORLD), 1.0);
                }
                else if (light_type == DIRECTIONAL)
                {
                    // direcitonal lights can use local
                    vec = glm::vec4(position(), 1.0);
                }
                else
                    return false;

                Renderer::get().bindLight(vec, m_Atten, m_Diffuse, id); // use diffuse light color only right now
                return true;
            }
            return false;
        }

        virtual void logicSelf(unsigned int advance) {
            /*static float offset = 0.0f;*/
            //static float shift = 1.0f;
            //if(offset > 5.0f)
               //shift = -1.0f;
            //else if(offset < 5.0f)
               //shift = 1.0f;
            //float timestep = advance * 0.001f;
            //Node::move(glm::vec3(shift*timestep*10.0f, 0.0f, 0.0f));
            /*offset += shift*timestep*10.0f;*/
        }
        virtual void renderSelf(IPartitioner* partitioner, unsigned int flags) const;
        
        Color ambient() const { return m_Ambient; }
        Color diffuse() const { return m_Diffuse; }
        Color specular() const { return m_Specular; }
        glm::vec3 attenuation() const { return m_Atten; }
        Type lightType() const { return m_Type; }

        void ambient(const Color& c) { m_Ambient = c; }
        void diffuse(const Color& c) { m_Diffuse = c; }
        void specular(const Color& c) { m_Specular = c; }
        void atten(const glm::vec3& atten) { m_Atten = atten; }
        void lightType(Type t) { m_Type = t; }
        
        virtual SCOPED_ENUM_TYPE(NodeType) nodeType() const { return NodeType::LIGHT; }
        virtual std::string nodeTypeString() const { return "light"; }

        virtual const AABB* box(unsigned int flags = 0) const { return &m_Box; }
        virtual AABB* box(unsigned int flags = 0) { return &m_Box; }

        void lightFlags(unsigned int flags) {
            m_LightFlags = flags;
        }
        unsigned int lightFlags() const {
            return m_LightFlags;
        }
    
    private:
        
        Color m_Ambient;
        Color m_Diffuse;
        Color m_Specular;
        glm::vec3 m_Atten; // c, l, q
        Type m_Type;
        enum eLightFlags {
            F_CAST_SHADOWS = BIT(0)
        };
        unsigned int m_LightFlags;

        AABB m_Box;
};

#endif

