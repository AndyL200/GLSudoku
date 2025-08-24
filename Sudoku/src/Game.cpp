#include "Shader.h"
#include <vector>

#include <ft2build.h>
#include ""

class SudokuGame
{
public:
	unsigned int lives = 3;
	unsigned int matrix[9][9] = { 0 };
	unsigned int dimX, dimY;
	std::vector<GLfloat> vertices;
	GLuint VAO, VBO;
	Shader* txtShader;
	Shader* gridShader;

	SudokuGame(unsigned int& width, unsigned int& height, GLuint& vao, GLuint& vbo, Shader& txt, Shader& g)
	{
		//should change with resize
		dimX = width;
		dimY = height;
		txtShader = &txt;
		gridShader = &g;
		VAO = vao;
		VBO = vbo;

		float padding_x = width * 0.2f;
		float padding_y = height * 0.2f;
		float left = padding_x;
		float right = width - padding_x;
		float top = height - padding_y;
		float bottom = padding_y;

		float gapx = right - left;
		float gapy = top - bottom;

		std::vector<GLfloat> gaps_x;
		std::vector<GLfloat> gaps_y;
		for (int i = 0; i < 9; ++i)
		{

		}


		vertices = 
		{
			//Coords // Color //
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,

			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
			left, right, 0.0f, 0.0f, 0.0f,
		}

	}
};
int main()
{
	Shader textShader("txt.vert", "txt.grid");
	GLuint VAO, VBO;
	
	return 0;
}