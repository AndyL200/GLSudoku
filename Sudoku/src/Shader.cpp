#include "Shader.h"

std::string get_file_contents(const char* filename)
{
	std::fstream in(filename, std::ios::binary);
	std::string contents;
	if (in)
	{
		in.seekg(0);
		int count = 0;
		in.tellg();
		contents.resize(count);
		in.read(contents, count);
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