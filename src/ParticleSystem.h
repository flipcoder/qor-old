#ifndef _PARTICLESYSTEM_H
#define _PARTICLESYSTEM_H

#include "Node.h"
#include "Texture.h"
#include "math/common.h"
#include "Graphics.h"

class ParticleSystem : public Node
{
	public:

		class Particle {
		public:
			Particle():
				life(1.0f),
				size(1.0f),
				flags(0)
			{
				color.white();
			}
			virtual ~Particle() {}

			void zero();
			void update(unsigned int advance);

			glm::vec3 pos;
			glm::vec3 vel;
			float life;
			float size;
			Color color;

			enum {
				ACTIVE = BIT(1)
			};
			unsigned int flags;
		};
		
		enum eBlendType {
			BLEND_NONE,
			BLEND_ADDITIVE,
			BLEND_DARKEN,
			BLEND_COLOR,
		};

		enum eFlags {
			F_LIFE = BIT(1)
		};

	protected:

		Particle m_ReferenceParticle;
		std::shared_ptr<Texture> m_spTexture;
		//Texture* m_pTexture;
		//bool m_bTextureOwnership;
		float m_fFreq;
		float m_fLife;
		float m_fSpawnAccum;
		float m_fVariance;
		glm::vec3 m_SpawnPos;
		glm::vec3 m_SpawnVariance;
		glm::vec3 m_PosVariance;
		std::list<Particle*> m_Particles;

		eBlendType m_BlendType;
		
		unsigned int m_Flags;
		
	public:

		ParticleSystem();
		virtual ~ParticleSystem();
		
		ParticleSystem(std::shared_ptr<Texture> texture,
			const Particle& particle,
			eBlendType type,
			float variance,
			float freq,
			unsigned int flags = 0);

		ParticleSystem(Texture* texture,
			const Particle& particle,
			eBlendType type,
			float variance,
			float freq,
			unsigned int flags = 0);
		
		//void setTextureOwnership(bool b) { m_bTextureOwnership = b; }
		//bool getTextureOwnership() { return m_bTextureOwnership; }

		virtual void logicSelf(unsigned int advance);
		virtual void renderSelf(Partitioner* partitioner = NULL, unsigned int flags = 0) const;

		// offsets particle spawn point without changing matrix
		void offset(const glm::vec3& pos){
			m_SpawnPos = pos;
		}
		
		unsigned int* flags() { return &m_Flags; }
		
		void nullify();
		
		virtual SCOPED_ENUM_TYPE(NodeType) nodeType() const { return NodeType::PARTICLE_SYSTEM; }
		virtual std::string nodeTypeString() const { return "particle_system"; }
};

#endif

