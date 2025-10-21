
#ifndef SUDO_SHADE
#define SUDO_SHADE

#include <iostream>
#include <fstream>
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>



class Shader
{
public:
	GLuint ID;
	Shader(const char* vert, const char* frag);
	~Shader();
	void Use();
	void Destroy();
};

#endif