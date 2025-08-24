#include "Shader.h"


std::string get_file_contents(const char* filename)
{
	std::fstream in(filename, std::stream::ios);

	if (in)
	{

	}
}


Shader::Shader(const char* vert, const char* frag)
{

}
void Shader::Use()
{
	glUseProgram(ID);
}
void Shader::Destroy()
{
	glDeleteProgram(ID);
}
Shader::~Shader()
{
	Destroy();
}