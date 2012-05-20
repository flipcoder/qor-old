#include "ParticleSystem.h"
#include "math/common.h"

#include "Util.h"
#include "Texture.h"
#include "Renderer.h"

#include <cstdlib>
#include <ctime>
#include <iostream>
using namespace std;

void ParticleSystem::Particle :: zero()
{
    //psystem = NULL;

    flags = 0;
    life = 0.0f;

    //pos.zero();
    //vel.zero();

    color.white();

    srand((unsigned int)time(0));
}

void ParticleSystem::Particle :: update(unsigned int advance)
{
    float speed = 0.001f * advance;
    pos += speed * vel;
    life -= speed;
    if(life<=0.0f)
        BIT_UNSET(flags, ACTIVE); // flag for removal
}


void ParticleSystem :: nullify()
{
    //m_bTextureOwnership = false;
    //m_pTexture = NULL;
    m_spTexture.reset();
    m_fFreq = 0.0f;
    m_fLife = 0.0f;
    m_fSpawnAccum = 0.0f;
    m_BlendType = BLEND_NONE;
    m_Flags = 0;    
}

ParticleSystem :: ParticleSystem()
{
    nullify();
}

//ParticleSystem :: ParticleSystem(std::string texture_fn)
//{
//    init(new Texture(texture_fn, Texture::MIPMAPPED));
//}

ParticleSystem :: ~ParticleSystem()
{
    //if(m_bTextureOwnership)
        //delete m_pTexture;
    foreach(Particle* p, m_Particles)
        delete p;
}


ParticleSystem :: ParticleSystem(
    shared_ptr<Texture> texture,
    const Particle& particle,
    eBlendType type,
    float variance,
    float freq,
    unsigned int flags
    //const glm::vec3& velocity,
    //const glm::vec3& variance
    //float freq,
    //float lif
)
{
    nullify();

    m_spTexture = texture;
    m_ReferenceParticle = particle;
    //m_ReferenceParticle.psystem = this;
    m_fVariance = variance;
    m_fFreq = freq;
    m_BlendType = type;
}

ParticleSystem :: ParticleSystem(
    Texture* texture,
    const Particle& particle,
    eBlendType type,
    float variance,
    float freq,
    unsigned int flags
    //const glm::vec3& velocity,
    //const glm::vec3& variance
    //float freq,
    //float lif
)
{
    nullify();

    m_spTexture.reset(texture);
    m_ReferenceParticle = particle;
    //m_ReferenceParticle.psystem = this;
    m_fVariance = variance;
    m_fFreq = freq;
    m_BlendType = type;
}

void ParticleSystem :: logicSelf(unsigned int advance)
{
    unsigned int particle_count = 0;
    // Particle logic, removal if necessary
    for(std::list<Particle*>::iterator itr = m_Particles.begin();
        itr != m_Particles.end();)
    {
        Particle* p = *itr;
        p->update(advance);
        if(! BIT_ISSET(p->flags, Particle::ACTIVE))
        {
            delete p;
            itr = m_Particles.erase(itr);
            continue;
        }
        ++particle_count;
        ++itr;
    }

    if(!(m_Flags & F_LIFE) || m_fLife >= 0.0f)
    {
        if(m_Flags & F_LIFE)
        {
            m_fLife -= advance * 0.001f;
            if(m_fLife < 0.0f)
                m_fLife = 0.0f;
        }
            
        // ParticleSystem spawn logic
        m_fSpawnAccum += (advance * 0.001f);
        while(!floatcmp(m_fFreq,0.0f) && 1.0f/m_fFreq <= m_fSpawnAccum)
        {
            m_fSpawnAccum -= 1.0f/m_fFreq;
            //m_fSpawnAccum = 0.0f;
            Particle* p = new Particle(m_ReferenceParticle);
            BIT_SET(p->flags, Particle::ACTIVE);
            float mag = glm::length(p->vel);
            p->pos = m_SpawnPos;
            p->pos = glm::vec3(
                p->pos.x + m_SpawnVariance.x * mag * (rand() % 1000) * 0.001f * (rand() % 2 ? 1.0f : -1.0f),
                p->pos.y + m_SpawnVariance.y * mag * (rand() % 1000) * 0.001f * (rand() % 2 ? 1.0f : -1.0f),
                p->pos.z + m_SpawnVariance.z * mag * (rand() % 1000) * 0.001f * (rand() % 2 ? 1.0f : -1.0f));
            p->vel = glm::vec3(
                p->vel.x + m_fVariance * mag * (rand() % 1000) * 0.001f * (rand() % 2 ? 1.0f : -1.0f),
                p->vel.y + m_fVariance * mag * (rand() % 1000) * 0.001f * (rand() % 2 ? 1.0f : -1.0f),
                p->vel.z + m_fVariance * mag * (rand() % 1000) * 0.001f * (rand() % 2 ? 1.0f : -1.0f));
            p->vel = glm::normalize(p->vel) * mag;
            m_Particles.push_back(p);
        }
    }

    if((m_Flags & F_LIFE) && m_fLife <= 0.0f && !particle_count)
        remove();
}

void ParticleSystem :: renderSelf(IPartitioner* partitioner, unsigned int flags) const
{
    Renderer::get().shaders(Renderer::UNBIND_SHADERS);
    Renderer::get().lighting(Renderer::UNBIND_LIGHTING);
    
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    m_spTexture->bind();

    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);

    switch(m_BlendType)
    {
        case BLEND_DARKEN:
            //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR); // gray darken
            //glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
            glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
            break;
        case BLEND_COLOR:
            glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE); // screen?
            break;
        case BLEND_ADDITIVE:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            break;
        default:
            glBlendFunc(GL_ONE, GL_ONE);
    };
    
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
    glPointSize(m_ReferenceParticle.size);
    glBegin(GL_POINTS);

    //glVertex3f(0.0f, 0.0f, 0.0f);
    foreach(const Particle* p, m_Particles){
        float lv = p->life * m_ReferenceParticle.life;
        m_ReferenceParticle.color.glColor(lv);
        glVertex3fv(glm::value_ptr(p->pos));
    }
    
    glEnd();

    //glDisable(GL_POINT_SPRITE);
    glDisable(GL_BLEND);

    glDepthMask(GL_TRUE);
    
    Renderer::get().lighting(Renderer::BIND_LIGHTING);
    Renderer::get().shaders(Renderer::BIND_SHADERS);
}

