#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <iostream>
#include <fstream>


class Shader
{
public:
	GLuint ID;
	Shader(const char* vert, const char* frag);
	~Shader();
	void Use();
	void Destroy();
};