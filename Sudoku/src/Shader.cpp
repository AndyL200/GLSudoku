#include "Shader.h"

std::string get_file_contents(std::string filename)
{

	std::ifstream in(filename, std::ios::binary);
	std::cout << "Opening : [" << filename << "] " << std::endl;
	std::string contents;
	if (in)
	{
		in.seekg(0, std::ios::end);
		size_t size = in.tellg();
		in.seekg(0, std::ios::beg);
		contents.resize(size);
		in.read(&contents[0], size);

		return contents;
	}
	fprintf(stderr, "Cannot read file\n");
	return "";
}


Shader::Shader(std::string vert, std::string frag)
{

	std::string vertex_txt = get_file_contents(vert);
	const char* vertex = vertex_txt.c_str();
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertex, NULL);
	glCompileShader(vertexShader);

	std::string frag_txt = get_file_contents(frag);
	const char* fragment = frag_txt.c_str();
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fragment, NULL);
	glCompileShader(fragShader);


	ID = glCreateProgram();

	GLint success;
	GLchar infoLog[512];

	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragShader);


	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "Vertex Shader Error:\n" << infoLog << std::endl;
	}

	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
		std::cout << "Fragment Shader Error:\n" << infoLog << std::endl;
	}


	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "Shader Linking Error:\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);

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