#include "Shader.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <filesystem>
#include <time.h>
#include <random>
#include <unordered_set>


enum SudokuState
{
	SUDOKU_SOLVED = 1,
	SUDOKU_NEUTRAL = 0,
	SUDOKU_INVALID = -1
};
//use mutex
std::pair<double, double> CURSOR_POS = { -1, -1 };
//use mutex
bool TO_CHECK = false;

struct Character {
	unsigned int TextureID;
	unsigned int Size[2];
	int Bearing[2];
	int Advance[2];
};

struct GameParameters
{
	std::vector<GLfloat> vertices;
	std::vector<GLfloat> selections;
	std::vector<GLfloat> gaps_x;
	std::vector<GLfloat> gaps_y;
	std::vector<GLuint> indicies_bottom;
	std::vector<GLuint> indicies;
	float left;
	float right;
	float top;
	float bottom;
	SudokuState state;
};
struct GLParameters
{
	GLuint vao_grid, vao_txt, vbo_grid, vbo_txt, ebo_grid;
	GLFWwindow* window;
	Shader* txtShader;
	Shader* gridShader;
	FT_Library lib;
	FT_Face face;
};

class SudokuGame
{
public:
	unsigned int lives = 3;
	unsigned int matrix[9][9];
	Character numericOptions[10];
	GLParameters glP;
	GameParameters GP;

	~SudokuGame() {
		FT_Done_Face(glP.face);
		FT_Done_FreeType(glP.lib);
		glDeleteProgram(glP.gridShader->ID);
		glDeleteProgram(glP.txtShader->ID);
		glDeleteVertexArrays(1, &glP.vao_grid);
		glDeleteVertexArrays(1, &glP.vao_txt);
		glDeleteBuffers(1, &glP.vbo_grid);
		glDeleteBuffers(1, &glP.ebo_grid);
		glDeleteBuffers(1, &glP.vbo_txt);
	}

	SudokuGame(int& width, int& height, Shader& txt, Shader& g, GLFWwindow* window)
	{
		//should change with resize
		glP.txtShader = &txt;
		glP.gridShader = &g;
		
		glP.window = window;
	
		for (int i = 0; i < 9; ++i) {
			for (int j = 0; j < 9; ++j) {
				this->matrix[i][j] = 0;
			}
		}


		//grab shader uniforms
		
		GLint utexcol = glGetUniformLocation(txt.ID, "text_color");
		GLint ugridcol = glGetUniformLocation(g.ID, "col");

		//center game region on the window
		float padding_x = width * 0.1f;
		float padding_y = height * 0.1f;
		float left = padding_x;
		GP.left = left;
		float right = width - padding_x;
		GP.right = right;

		//Coordinate system increases downwards
		float bottom_y = height - padding_y;
		GP.bottom = bottom_y;
		
		float top_y = padding_y;
		GP.top = top_y;

		float gapx = right - left;
		float gapy = bottom_y - top_y;

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
			left, bottom_y,				
			left, top_y,			
			left + gaps_x[0], bottom_y,	
			left + gaps_x[0], top_y,
			left + gaps_x[1], bottom_y,	
			left + gaps_x[1], top_y,
			left + gaps_x[2], bottom_y,	
			left + gaps_x[2], top_y,
			left + gaps_x[3], bottom_y,	
			left + gaps_x[3], top_y,
			left + gaps_x[4], bottom_y,	
			left + gaps_x[4], top_y,
			left + gaps_x[5], bottom_y,	
			left + gaps_x[5], top_y,
			left + gaps_x[6], bottom_y,	
			left + gaps_x[6], top_y,
			left + gaps_x[7], bottom_y,	
			left + gaps_x[7], top_y,
			right, bottom_y,				
			right, top_y,			

			left, top_y,			
			right, top_y,			
			left, top_y + gaps_y[0],
			right, top_y + gaps_y[0],
			left, top_y + gaps_y[1],
			right, top_y + gaps_y[1],
			left, top_y + gaps_y[2],
			right, top_y + gaps_y[2],
			left, top_y + gaps_y[3],
			right, top_y + gaps_y[3],
			left, top_y + gaps_y[4],
			right, top_y + gaps_y[4],
			left, top_y + gaps_y[5],
			right, top_y + gaps_y[5],
			left, top_y + gaps_y[6],
			right, top_y + gaps_y[6],
			left, top_y + gaps_y[7],
			right, top_y + gaps_y[7],
			left, bottom_y,				
			right, bottom_y,				

		};
		//notice how some values are reused
		float selection_menu_bottom = height * 0.95f;
		float selection_menu_top = height * 0.85f;
		GP.selections =
		{
			left, selection_menu_top,
			right, selection_menu_top,
			left, selection_menu_bottom,
			right, selection_menu_bottom,
		};

		GP.indicies_bottom =
		{
			1, 2,
			3, 4,
			1, 3,
			2, 4
		};

		GP.indicies =
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


			20, 21,
			22, 23,
			24, 25,
			26, 27,
			28, 29,
			30, 31,
			32, 33,
			34, 35,
			36, 37,
			38, 39

		};

		glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f);

		//grid VAO, VBO, and EBO
		glGenVertexArrays(1, &glP.vao_grid);
		glGenBuffers(1, &glP.vbo_grid);
		glBindVertexArray(glP.vao_grid);
		glGenBuffers(1, &glP.ebo_grid);
		glBindBuffer(GL_ARRAY_BUFFER, glP.vbo_grid);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glP.ebo_grid);
		glBufferData(GL_ARRAY_BUFFER, GP.vertices.size() * sizeof(GLfloat), GP.vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, GP.indicies.size() * sizeof(GLuint), GP.indicies.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		//text VAO and VBO
		glGenVertexArrays(1, &glP.vao_txt);
		glGenBuffers(1, &glP.vbo_txt);
		glBindVertexArray(glP.vao_txt);
		glBindBuffer(GL_ARRAY_BUFFER, glP.vbo_txt);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		
		glUseProgram(glP.txtShader->ID);
		glUniform3f(utexcol, 0.1098f, 1.0f, 0.80784f);
		glUniformMatrix4fv(glGetUniformLocation(glP.txtShader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		GLint utex = glGetUniformLocation(glP.txtShader->ID, "tex");
		//problem line
		glUniform1i(utex, 0);


		glUseProgram(glP.gridShader->ID);
		glUniform3f(ugridcol, 0.1098f, 0.333333f, 1.0f);
		glUniformMatrix4fv(glGetUniformLocation(glP.gridShader->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		FT_Library ft;
		if (FT_Init_FreeType(&ft))
		{
			fprintf(stderr, "Could not init freetype");
			return;
		}
		FT_Face facade;
		if (FT_New_Face(ft, "fonts/Cairopixel.ttf", 0, &facade))
		{
			fprintf(stderr, "Could not create face");
			return;
		}

		FT_Set_Pixel_Sizes(facade, 0, 48);

		if (FT_Load_Char(facade, 'X', FT_LOAD_RENDER))
		{
			fprintf(stderr, "Could not Load Character");
			return;
		}
		if (FT_Render_Glyph(facade->glyph, FT_RENDER_MODE_LCD))
		{
			fprintf(stderr, "Could not Render Character");
			return;
		}
		

		this->glP.lib = ft;
		this->glP.face = facade;
		digitTexturePreload();
		sudoku_setup();

	}

	void drawLines()
	{
		glBindVertexArray(glP.vao_grid);
		glBindBuffer(GL_ARRAY_BUFFER, glP.vbo_grid);
		//EBO here
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glP.ebo_grid);

		glBufferData(GL_ARRAY_BUFFER, GP.vertices.size() * sizeof(GLfloat), GP.vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, GP.indicies.size() * sizeof(GLuint), GP.indicies.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

		glP.gridShader->Use();
		

		glDrawElements(GL_LINES, GP.indicies.size(), GL_UNSIGNED_INT, 0);
		//unbind
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);


	}
	void sudoku_setup() {
	std::mt19937 mt(time(nullptr));
    std::unordered_set<int> used;
    int generated = 0;
    int target = 17 + std::uniform_int_distribution<int>(0, 7)(mt);  // 17-23 clues
    int attempts = 0;
    const int MAX_ATTEMPTS = 1000;  // Prevent infinite loop

    while (generated < target && attempts < MAX_ATTEMPTS) {
        attempts++;
        
        int rand_row = std::uniform_int_distribution<int>(0, 8)(mt);
        int rand_col = std::uniform_int_distribution<int>(0, 8)(mt);
        int cell_id = rand_row * 9 + rand_col;

        // Skip if already filled
        if (used.find(cell_id) != used.end()) {
            continue;
        }
        used.insert(cell_id);

		int rand_val = std::uniform_int_distribution<int>(1, 9)(mt);
        matrix[rand_row][rand_col] = rand_val;

        // Check if board is still valid
        if (sudokuValid() != SUDOKU_INVALID) {
            generated++;
        } else {
            // Placement violated rules, undo it and try again
            matrix[rand_row][rand_col] = 0;
            used.erase(cell_id);
        }
    }

    if (attempts >= MAX_ATTEMPTS) {
        std::cout << "WARNING: Could only generate " << generated << " valid clues (target was " << target << ")" << std::endl;
    }

    std::cout << "\t\t\t BOARD (" << generated << " clues)\t\t\t" << std::endl;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            std::cout << " [" << matrix[i][j] << "] ";
        }
        std::cout << std::endl;
    }
}
	void sudoku_solved()
	{

	}
	std::pair<float, float> gridToScreen(int row, int col) {

		if (col < 0 || row < 0 || col > 9 || row > 9) {
			return { -1,-1 };
		}

		float cell_w = (GP.right - GP.left) / 9.0f;
		float cell_h = (GP.bottom - GP.top) / 9.0f;

		float xpos, ypos;
		
		if (col == 0) {
			xpos = (GP.left + GP.gaps_x[0])/2;
		}
		else if (col < 8) {
			xpos = (GP.left + GP.gaps_x[col - 1] + GP.gaps_x[col]) / 2;
		}
		else {
			xpos = (GP.gaps_x[7] + GP.right) / 2;
		}

		if (row == 0) {
			ypos = (GP.top + GP.gaps_y[0]) / 2;
		}
		else if (row < 8) {
			//average position between two lines
			ypos = (GP.top + GP.gaps_y[row - 1] + GP.gaps_y[row])/2;
		}
		else {
			ypos = (GP.gaps_y[7] + GP.bottom) / 2;
		}

		return { xpos + cell_w/2, ypos + cell_h/2 };
	}

	void digitTexturePreload() {
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);//nobyte-alignmentrestriction


		for (int i = 0; i < 10; i++)
		{
			char d = i + '0';
			//loadcharacterglyph
			if (FT_Load_Char(glP.face, d, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: FailedtoloadGlyph" << std::endl;
				continue;
			}
			//generatetexture
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				glP.face->glyph->bitmap.width,
				glP.face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				glP.face->glyph->bitmap.buffer
			);
			//settextureoptions
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//nowstorecharacterforlateruse
			numericOptions[i] = {
			texture,
			{glP.face->glyph->bitmap.width,glP.face->glyph->bitmap.rows},
			{glP.face->glyph->bitmap_left,glP.face->glyph->bitmap_top},
			glP.face->glyph->advance.x
			};
			
		}
	}

	void renderDigit(int digit, int row, int col)
	{
		
		std::pair<float, float> screen_pos = gridToScreen(row, col);
		float x = screen_pos.first;
		float y = screen_pos.second;
		//std::cout << "x:" << x << " " << "y:" << y << std::endl;
		if (x == -1 || y == -1) { return; }
		Character glyph = numericOptions[digit];
		glP.txtShader->Use();
		

		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(glP.vao_txt);
		glBindTexture(GL_TEXTURE_2D, glyph.TextureID);
		
		
		//add scale later
		float x2 = x + glyph.Bearing[0];
		//increase y as you go down
		float y2 = y + (glyph.Size[1] - glyph.Bearing[1]);
		float w = glyph.Size[0];
		float h = glyph.Size[1];

		GLfloat vertices[6][4] = {
			{x2,     y2,	 0.0f, 0.0f},
			{x2,	 y2 + h, 0.0f, 1.0f},
			{x2 + w, y2 + h, 1.0f, 1.0f},
			{x2,	 y2,	 0.0f, 0.0f},
			{x2 + w, y2 + h, 1.0f, 1.0f},
			{x2 + w, y2,	 1.0f, 0.0f}
		};
		
		glBindBuffer(GL_ARRAY_BUFFER, glP.vbo_txt);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		// UNBIND??
		// glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		/*x += (g->advance.x / 64);
		y += (g->advance.y / 64);
		only rendering one digit so no real reason to advance*/

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		return;
	}
	SudokuState sudokuValid()
	{
		
		uint16_t count = 0;

		for (int i = 0; i < 9; ++i)
		{
			uint16_t row = 0, col = 0, box = 0;
			for (int j = 0; j < 9; ++j)
			{
				int digit = matrix[i][j]-1;
				if ((1 << digit) & row)
				{
					return SUDOKU_INVALID;
				}
				else if (digit >= 0) {
					row |= 1 << digit;
				}

				digit = matrix[j][i] - 1;
				if ((1 << digit) & col)
				{
					return SUDOKU_INVALID;
				}
				else if (digit >= 0)
				{
					col |= 1 << digit;
				}

				digit = matrix[(3 * (i / 3)) + j / 3][3 * (i % 3) + j % 3] - 1;
				if ((1 << digit) & box)
				{
					return SUDOKU_INVALID;
				}
				else if (digit >= 0)
				{
					box |= 1 << digit;
				}
			}
			uint16_t max_solve = (1u << 9) - 1;
			if (row == max_solve && col == max_solve && box == max_solve)
			{
				count++;
			}
		}
		if (count == 9)
		{
			return SUDOKU_SOLVED;
		}
		
		return SUDOKU_NEUTRAL;
	}

	void initialRenderMatrix() {
		for (int i = 0; i < 9; ++i) {
			for (int j = 0; j < 9; ++j) {
				renderDigit(matrix[i][j], i, j);
			}
		}
	}

	void renderMatrix() {
		for(int i = 0; i < 9; ++i) {
			for(int j = 0; j < 9; ++j) {
				renderDigit(matrix[i][j], i, j);
				//std::cout << " [" << matrix[i][j] << "] ";
			}
			//std::cout << std::endl;
		}
	}
	void process_input(int digit, int row, int col)
	{
		//change matrix to reflect new digit
		//every change needs validation
		matrix[row][col] = digit;


		this->GP.state = sudokuValid();
		if (this->GP.state == SUDOKU_SOLVED)
		{
			sudoku_solved();
			return;
		}
		if (this->GP.state == SUDOKU_INVALID)
		{
			lives -= 1;
			std::cout << "Life lost; lives: " << lives << std::endl;
			matrix[row][col] = 0;
		}
		
	}

	void setCurrentCursorPos(int& x, int& y) {
		x = CURSOR_POS.first;
		y = CURSOR_POS.second;
	}

	//convert cursor pos to grid indicies
	std::pair<int, int> insideGrid()
	{
		double xpos = CURSOR_POS.first, ypos = CURSOR_POS.second;
		//convert to indices
		if (xpos < GP.left || xpos > GP.right || ypos < GP.top || ypos > GP.bottom)
		{
			return { -1, -1 };
		}

		// Compute cell width and height
		float cell_w = (GP.right - GP.left) / 9.0f;
		float cell_h = (GP.bottom - GP.top) / 9.0f;

		// Find which cell the cursor is in
		int col = (int)((xpos - GP.left) / cell_w);
		int row = (int)((ypos - GP.top) / cell_h);

		// Clamp to [0, 8]
		col = (col < 0) ? 0 : (col > 8) ? 8 : col;
		row = (row < 0) ? 0 : (row > 8) ? 8 : row;

		return { col, row };
	}

	void changeGridVal(int val, std::pair<int, int>& pos)
	{
		
		int x_i = pos.first;
		int y_i = pos.second;

		if (x_i == -1 || y_i == -1)
		{
			return;
		}

		
		process_input(val, y_i, x_i);
	}

	void waitForUpdate()
	{
		//method broken
		glBindVertexArray(glP.vao_grid);
		glBindBuffer(GL_ARRAY_BUFFER, glP.vbo_grid);
		//EBO here
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glP.ebo_grid);

		glP.gridShader->Use();

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(0);
		glBufferData(GL_ARRAY_BUFFER, GP.selections.size() * sizeof(GLfloat), GP.selections.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, GP.indicies_bottom.size() * sizeof(GLuint), GP.indicies_bottom.data(), GL_STATIC_DRAW);

		//buffers need to reset to original grid values
		glDrawElements(GL_LINES, GP.indicies_bottom.size(), GL_UNSIGNED_INT, 0);

		//unbind
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

	}
};


static void mouseCallBack(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		if (TO_CHECK == false)
		{
			glfwGetCursorPos(window, &CURSOR_POS.first, &CURSOR_POS.second);
			std::cout << "cursor x" << CURSOR_POS.first << "cursor y" << CURSOR_POS.second << std::endl;
			TO_CHECK = true;
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
	glfwMakeContextCurrent(main);

	if (!gladLoaderLoadGL())
	{
		fprintf(stderr, "could not load GL");
		return -1;
	}

	glfwSwapInterval(1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	Shader textShader("shaders/txt.vert", "shaders/txt.frag");
	Shader gridShader("shaders/grid.vert", "shaders/grid.frag");
	
	int fbw, fbh;
	glfwGetFramebufferSize(main, &fbw, &fbh);
	glViewport(0, 0, fbw, fbh);

	
	SudokuGame game(fbw, fbh, textShader, gridShader, main);

	glfwSetMouseButtonCallback(main, mouseCallBack);
	//#1DB1DE
	glClearColor(14/255, 89/255, 111/255, 44/255);

	game.initialRenderMatrix();
	while (!glfwWindowShouldClose(main))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	
		//consider thread safety
		if (TO_CHECK)
		{
			unsigned int INPUT_DIGIT = 0;
			//wait for input digit
			printf("ENTER DIGIT: ");
			//blocks
			scanf_s("%d", &INPUT_DIGIT);
			if(INPUT_DIGIT < 1 || INPUT_DIGIT > 9)
			{
				INPUT_DIGIT = 0;
			}
			std::pair<int, int> pos = game.insideGrid();
			std::cout << "first x: " << pos.first << "second y" << pos.second << std::endl;
			std::pair<double, double> p2 = game.gridToScreen(pos.second, pos.first);
			std::cout << "xpos x: " << p2.first << "second y" << p2.second << std::endl;
			game.changeGridVal(INPUT_DIGIT, pos);
			TO_CHECK = false;
		}
		game.renderMatrix();
		game.drawLines();
		//game.waitForUpdate();
		glfwSwapBuffers(main);
		glfwPollEvents();
	}
	return 0;
}