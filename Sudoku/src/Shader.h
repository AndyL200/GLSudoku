#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <iostream>

class Shader
{
	GLuint ID;
	Shader(const char* vert, const char* frag);
	~Shader();
	void Use();
	void Destroy();
};