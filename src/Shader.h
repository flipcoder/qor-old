#ifndef _SHADER_H
#define _SHADER_H

#include <string>
#include <list>
#include <memory>
#include "math/common.h"
#include "GfxAPI.h"

class Shader
{
	public:

		enum eType {
			NO_TYPE = 0,
			FRAGMENT,
			VERTEX,
			GEOMETRY
		};

	protected:
		
		unsigned int m_ID;
		eType m_Type;

	public:
		Shader();
		Shader(std::string fn, eType type, unsigned int flags = 0);
		virtual ~Shader();

		void nullify();

		//enum {
		//    L_COMPILE = BIT(1)
		//};

		bool load(std::string fn, eType type, unsigned int flags = 0);
		//bool compile();
		void unload();

		bool good() const { return m_ID!=0; }
		bool bad() const { return m_ID==0; }

		unsigned int id() const { return m_ID; }
};

class Program
{
	public:

		typedef int UniformID;
		static bool isValidUniformID(UniformID u) { return (int)u >= 0; }

	protected:

		unsigned int m_ID;
		std::list<std::shared_ptr<Shader>> m_Shaders;

	public:

		Program();
		virtual	~Program();
		bool attach(std::shared_ptr<Shader>& shader);
		bool link();
		bool use();
		void unload();
		
		bool good() const { return m_ID!=0; }
		bool bad() const { return m_ID==0; }

		void nullify();
		unsigned int id() const { return m_ID; }
		
		void attribute(unsigned int index, std::string name) {
			glBindAttribLocation(m_ID, index, name.c_str());
		}

		UniformID uniform(std::string n);
		static void uniform(UniformID uid, float v);
		static void uniform(UniformID uid, float v, float v2);
		static void uniform(UniformID uid, float v, float v2, float v3);
		static void uniform(UniformID uid, float v, float v2, float v3, float v4);
		static void uniform(UniformID uid, int v);
		static void uniform(UniformID uid, int v, int v2);
		static void uniform(UniformID uid, int v, int v2, int v3);
		static void uniform(UniformID uid, int v, int v2, int v3, int v4);
		static void uniform(UniformID uid, glm::mat4& matrix);
		static void uniform(UniformID uid, glm::vec3& vec);
		static void uniform(UniformID uid, glm::vec4& vec);

		// glUniform{size}{f|i}({uid},{count},{v})
		static void uniform(UniformID uid, unsigned int size, unsigned int count, const int* v);
		static void uniform(UniformID uid, unsigned int size, unsigned int count, const float* v);
		
		static void unuseAll() { glUseProgram(0); }
};

#endif

