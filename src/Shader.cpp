#include <fstream>
using namespace std;

#include "Shader.h"
#include "Util.h"
#include "Log.h"

Shader :: Shader()
{
	nullify();
}

Shader :: Shader(string fn, eType type, unsigned int flags)
{
	load(fn,type,flags);
}


Shader :: ~Shader()
{
	unload();
}

void Shader :: nullify()
{
	m_ID = 0;
	m_Type = NO_TYPE;
}

bool Shader :: load(string fn, eType type, unsigned int flags)
{
	int status;

	unload();

	if(! m_ID)
	{
		m_Type = type;
		GLenum gltype;
		switch(m_Type)
		{
			case FRAGMENT:
				gltype = GL_FRAGMENT_SHADER;
				break;
			case VERTEX:
				gltype = GL_VERTEX_SHADER;
				break;
			case GEOMETRY:
				gltype = GL_GEOMETRY_SHADER;
				break;
			default:
				return 0;
		}
		m_ID = glCreateShader(gltype);
	}

	ifstream file;
	vector<string> file_lines;
	string line;
	file.open(fn);
	while(getline(file, line))
		file_lines.push_back(line+"\n");
	file.close();

	if(file_lines.empty())
		return false;

	//const char* c = &file_lines[0];
	vector<const char*> cstring_ptrs;
	cstring_ptrs.reserve(file_lines.size());
	vector<GLint> cstring_lens;
	cstring_lens.reserve(file_lines.size());
	
	transform(file_lines.begin(), file_lines.end(), back_inserter(cstring_ptrs), mem_fn(&string::c_str));
	transform(file_lines.begin(), file_lines.end(), back_inserter(cstring_lens), mem_fn(&string::size));
	glShaderSource(m_ID, file_lines.size(), &cstring_ptrs[0], &cstring_lens[0]);

	glCompileShader(m_ID);
	glGetShaderiv(m_ID, GL_COMPILE_STATUS, &status);
	if(! status)
	{
		unload();
		return false;
	}
	Log::get().write("Shader @ id = " + str(m_ID));
	return m_ID!=0;
}

//bool Shader :: compile()
//{
//    return true;
//}

void Shader:: unload()
{
	if(m_ID)
		glDeleteShader(m_ID);

	nullify();
}

Program :: Program()
{
	nullify();
	m_ID = glCreateProgram();
	if(glGetError() != GL_NO_ERROR)
		Log::get().error("OGL Error Code: " + str(glGetError()));
}

Program :: ~Program()
{
	unload();
}

void Program :: nullify()
{
	m_ID = 0;
}

void Program :: unload()
{
	if(m_ID)
		glDeleteProgram(m_ID);
	nullify();
}

bool Program :: attach(shared_ptr<Shader>& shader)
{
	ASSERT(shader);
	if(!shader || !shader->good())
		return false;
	glAttachShader(m_ID, shader->id());
	m_Shaders.push_back(shader);
	return true;
}

bool Program :: link()
{
	if(!m_ID)
		return false;
	int r;
	glLinkProgram(m_ID);
	glGetProgramiv(m_ID, GL_LINK_STATUS, &r);
	if(glGetError() != GL_NO_ERROR)
		Log::get().error("OGL Error Code: " + str(glGetError()));
	return r!=0;
}

bool Program :: use()
{
	if(!m_ID)
		return false;
	glUseProgram(m_ID);
	return true;
}

Program::UniformID Program :: uniform(string n)
{
	return (UniformID)glGetUniformLocation(m_ID, n.c_str());
}

void Program :: uniform(UniformID uid, float v){
	if(!isValidUniformID(uid)) return;
	glUniform1f((GLint)uid, v);
}
void Program :: uniform(UniformID uid, float v, float v2){
	if(!isValidUniformID(uid)) return;
	glUniform2f((GLint)uid, v, v2);
}
void Program :: uniform(UniformID uid, float v, float v2, float v3){
	if(!isValidUniformID(uid)) return;
	glUniform3f((GLint)uid, v, v2, v3);
}
void Program :: uniform(UniformID uid, float v, float v2, float v3, float v4) {
	if(!isValidUniformID(uid)) return;
	glUniform4f((GLint)uid, v, v2, v3, v4);
}
void Program :: uniform(UniformID uid, int v){
	if(!isValidUniformID(uid)) return;
	glUniform1i((GLint)uid, v);
}
void Program :: uniform(UniformID uid, int v, int v2){
	if(!isValidUniformID(uid)) return;
	glUniform2i((GLint)uid, v, v2);
}
void Program :: uniform(UniformID uid, int v, int v2, int v3){
	if(!isValidUniformID(uid)) return;
	glUniform3i((GLint)uid, v, v2, v3);
}
void Program :: uniform(UniformID uid, int v, int v2, int v3, int v4){
	if(!isValidUniformID(uid)) return;
	glUniform4i((GLint)uid, v, v2, v3, v4);
}
void Program :: uniform(UniformID uid, glm::mat4& matrix){
	if(!isValidUniformID(uid)) return;
	glUniformMatrix4fv((GLint)(uid), 1, false, glm::value_ptr(matrix));
}
void Program :: uniform(UniformID uid, glm::vec3& vec){
	if(!isValidUniformID(uid)) return;
	glUniform3fv((GLint)(uid), 1, glm::value_ptr(vec));
}
void Program :: uniform(UniformID uid, glm::vec4& vec){
	if(!isValidUniformID(uid)) return;
	glUniform4fv((GLint)(uid), 1, glm::value_ptr(vec));
}
//void Program :: uniform(UniformID uid, unsigned int size, unsigned int count, const int* v) {
//    if(!isValidUniformID(uid)) return;
//    // todo: add
//}
//void Program :: uniform(UniformID uid, unsigned int size, unsigned int count, const float* v){
//    if(!isValidUniformID(uid)) return;
//    // todo: add
//}

