#include "Shader.h"
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H



struct GameParameters
{
	std::vector<GLfloat> vertices;
	std::vector<GLfloat> numPositions;
	std::vector<GLfloat> gaps_x;
	std::vector<GLfloat> gaps_y;
	float left;
	float right;
	float top;
	float bottom;

};
struct GLParameters
{
	GLuint VAO, VBO, tex;
	GLFWwindow* window;
	Shader* txtShader;
	Shader* gridShader;
	GLint uniform_tex, uniform_col;
	FT_Library lib;
	FT_Face face;
};

class SudokuGame
{
public:
	unsigned int lives = 3;
	unsigned int matrix[9][9] = { 0 };
	int sx, sy;
	GLParameters glP;
	GameParameters GP;

	SudokuGame(int& width, int& height, GLuint& vao, GLuint& vbo, Shader& txt, Shader& g, GLFWwindow* window)
	{

		//should change with resize
		sx = 2.0f/width;
		sy = 2.0f/height;
		glP.txtShader = &txt;
		glP.gridShader = &g;
		glP.VAO = vao;
		glP.VBO = vbo;
		glP.window = window;

		//grab shader uniforms
		glP.uniform_tex = glGetUniformLocation(txt.ID, "tex");
		glP.uniform_col = glGetUniformLocation(txt.ID, "color");

		//center game region on the window
		float padding_x = width * 0.2f;
		float padding_y = height * 0.2f;
		float left = padding_x;
		GP.left = left;
		float right = width - padding_x;
		GP.right = right;
		float top = height - padding_y;
		GP.top = top;
		float bottom = padding_y;
		GP.bottom = bottom;

		float gapx = right - left;
		float gapy = top - bottom;

		std::vector<GLfloat> gaps_x;
		std::vector<GLfloat> gaps_y;
		for (int i = 1; i < 9; ++i)
		{
			gaps_x.push_back(i*gapx/9.0f);
			gaps_y.push_back(i*gapy/9.0f);
		}
		GP.gaps_x = gaps_x;
		GP.gaps_y = gaps_y;

		GP.vertices =
		{
			//Coords // Color //
			left, top,					0.0f, 0.0f, 0.0f,
			left, bottom,				0.0f, 0.0f, 0.0f,
			left + gaps_x[0], top,		0.0f, 0.0f, 0.0f,
			left + gaps_x[0], bottom,	0.0f, 0.0f, 0.0f,
			left + gaps_x[1], top,		0.0f, 0.0f, 0.0f,
			left + gaps_x[1], bottom,	0.0f, 0.0f, 0.0f,
			left + gaps_x[2], top,		0.0f, 0.0f, 0.0f,
			left + gaps_x[2], bottom,	0.0f, 0.0f, 0.0f,
			left + gaps_x[3], top,		0.0f, 0.0f, 0.0f,
			left + gaps_x[3], bottom,	0.0f, 0.0f, 0.0f,
			left + gaps_x[4], top,		0.0f, 0.0f, 0.0f,
			left + gaps_x[4], bottom,	0.0f, 0.0f, 0.0f,
			left + gaps_x[5], top,		0.0f, 0.0f, 0.0f,
			left + gaps_x[5], bottom,	0.0f, 0.0f, 0.0f,
			left + gaps_x[6], top,		0.0f, 0.0f, 0.0f,
			left + gaps_x[6], bottom,	0.0f, 0.0f, 0.0f,
			left + gaps_x[7], top,		0.0f, 0.0f, 0.0f,
			left + gaps_x[7], bottom,	0.0f, 0.0f, 0.0f,
			right, top,					0.0f, 0.0f, 0.0f,
			right, bottom,				0.0f, 0.0f, 0.0f,

			bottom, left,				0.0f, 0.0f, 0.0f,
			bottom, right,				0.0f, 0.0f, 0.0f,
			bottom + gaps_y[0], left,	0.0f, 0.0f, 0.0f,
			bottom + gaps_y[0], right,	0.0f, 0.0f, 0.0f,
			bottom + gaps_y[1], left,	0.0f, 0.0f, 0.0f,
			bottom + gaps_y[1], right,	0.0f, 0.0f, 0.0f,
			bottom + gaps_y[2], left,	0.0f, 0.0f, 0.0f,
			bottom + gaps_y[2], right,	0.0f, 0.0f, 0.0f,
			bottom + gaps_y[3], left,	0.0f, 0.0f, 0.0f,
			bottom + gaps_y[3], right,	0.0f, 0.0f, 0.0f,
			bottom + gaps_y[4], left,	0.0f, 0.0f, 0.0f,
			bottom + gaps_y[4], right,	0.0f, 0.0f, 0.0f,
			bottom + gaps_y[5], left,	0.0f, 0.0f, 0.0f,
			bottom + gaps_y[5], right,	0.0f, 0.0f, 0.0f,
			bottom + gaps_y[6], left,	0.0f, 0.0f, 0.0f,
			bottom + gaps_y[6], right,	0.0f, 0.0f, 0.0f,
			bottom + gaps_y[7], left,	0.0f, 0.0f, 0.0f,
			bottom + gaps_y[7], right,	0.0f, 0.0f, 0.0f,
			top, left,					0.0f, 0.0f, 0.0f,
			top, right,					0.0f, 0.0f, 0.0f,


		};

		GLint indicies[] =
		{
			0, 1,
			2, 3,
			4, 5,
			6, 7,
			//Say that again?
			8, 9,
			10, 11,
			12, 13,
			14, 15,
			16, 17,
			18, 19,
			20, 21

		};
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glUniform1i(uniform_tex, 0);
		glUniform3f(uniform_col, 1.0f, 1.0f, 1.0f);

		FT_Library ft;
		if (FT_Init_FreeType(&ft))
		{
			fprintf(stderr, "Could not init freetype");
			return;
		}
		FT_Face facade;
		if (FT_New_Face(ft, "fonts/UltronRegular-ARwBM.ttf", 0, &facade))
		{
			fprintf(stderr, "Could not create face");
			return;
		}
		if (FT_Load_Char(facade, 'X', FT_LOAD_RENDER))
		{
			fprintf(stderr, "Could not Load Character");
			return;
		}

		this->glP.lib = ft;
		this->glP.face = facade;
		    

	}

	~SudokuGame()
	{
		glDeleteProgram(gridShader->ID);
		glDeleteProgram(txtShader->ID);
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}
	void drawLines()
	{
		glBindVertexArray(glP.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, glP.VBO);

		glP.gridShader->Use();

		glVertexAttribPointer(0, sizeof(GLfloat) * 2, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, sizeof(GLfloat) * 3, GL_FLOAT, GL_FALSE, 0, (void*)2);
		glBufferData(GL_ARRAY_BUFFER, GP.vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

		glDrawArrays(GL_LINE, 0, 36);

	}
	enum SudokuState
	{
		SUDOKU_SOLVED = 1,
		SUDOKU_NEUTRAL = 0,
		SUDOKU_INVALID = -1
	};
	void sudoku_solved()
	{

	}

	void renderDigit(int digit, int x, int y)
	{

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glVertexAttribPointer(0, sizeof(GLfloat) * 4, GL_FLOAT, GL_FALSE, 0, 0);

		glP.txtShader->Use();
		char d = digit + 32;


		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


		if (FT_Load_Char(glP.face, d, FT_LOAD_RENDER))
		{
			fprintf(stderr, "Could not load character");
			return;
		}
		FT_GlyphSlot g = glP.face->glyph;

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			g->bitmap.width,
			g->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			g->bitmap.buffer
		);

		float x2 = x + g->bitmap_left * sx;
		float y2 = -y - g->bitmap_top * sy;
		float w = g->bitmap.width * sx;
		float h = g->bitmap.rows * sy;

		GLfloat box[4][4] = {
			{x2,     -y2    , 0, 0},
			{x2 + w, -y2    , 1, 0},
			{x2,     -y2 - h, 0, 1},
			{x2 + w, -y2 - h, 1, 1},
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		x += (g->advance.x / 64) * sx;
		y += (g->advance.y / 64) * sy;

		return;
	}
	SudokuState sudokuValid()
	{
		unsigned int row = 0, col = 0, box = 0;

		for (int i = 0; i < 9; ++i)
		{
			if (1 << i & row)
			{
				return SUDOKU_INVALID;
			}
		}

		if (row == 511, col == 511, box == 511)
		{
			return SUDOKU_SOLVED;
		}
	}
	void process_input(int digit, int x, int y)
	{
		//change matrix to reflect new digit
		SudokuState state = sudokuValid();
		if (state == SUDOKU_SOLVED)
		{
			sudoku_solved();
			return;
		}
		if (state == SUDOKU_INVALID)
		{
			lives -= 1;
			matrix[x][y] = 0;
		}
		renderDigit(matrix[x][y], x, y);
	}
};

std::pair<int, int> insideGrid(SudokuGame& game)
{
	double xpos, ypos;
	glfwGetCursorPos(game.glP.window, &xpos, &ypos);
	if (xpos > game.GP.left and xpos <  game.GP.right and ypos > game.GP.top and ypos < game.GP.bottom)
	{
		int x_idx = -1;
		int y_idx = -1;
		for (int i = 0; i < 8; ++i)
		{
			if (xpos > game.GP.gaps_x[i])
			{
				x_idx = i;
			}
			if (ypos > game.GP.gaps_y[i])
			{
				y_idx = i;
			}
		}
		return { x_idx, y_idx };
	}
	return { -1, -1 };
}
static void mouseCallBack(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		std::pair<int, int> xy = insideGrid();
		if (xy.first != -1 and xy.second != -1)
		{

		}
	}
}


int main()
{
	if (!glfwInit())
	{
		fprintf(stderr, "Could not init GLFW");
		return -1;
	}
	GLFWwindow* main = glfwCreateWindow(500, 500, "SudokuGame", NULL, NULL);
	if (!main)
	{
		fprintf(stderr, "Could not create window");
		return -1;
	}
	if (!gladLoadGL())
	{
		fprintf(stderr, "could not load GL");
		return -1;
	}

	glfwMakeContextCurrent(main);
	glfwSwapInterval(1);
	glEnable(GL_BLEND);
	glBlendFunc();
	

	Shader textShader("txt.vert", "txt.grid");
	Shader gridShader("grid.vert", "grid.frag");
	GLuint VAO, VBO;
	int fbw, fbh;
	glfwGetWindowSize(main, &fbw, &fbh);
	glViewport(0, 0, fbw, fbh);
	
	SudokuGame game(fbw, fbh, VAO, VBO, textShader, gridShader, main);

	glfwSetMouseButtonCallback(main, );

	while (!glfwWindowShouldClose(main))
	{
		game.process_input();
		game.drawLines();
		glfwSwapBuffers(main);
		glfwPollEvents();
	}
	return 0;
}