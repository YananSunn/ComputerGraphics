#include <iostream>
#include <cmath>
#include <string>
#include <map>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include <SOIL.h>

// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

// Other includes
#include "shader.h"


// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void RenderText(Shader& shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
void change_speed();

// Window dimensions
GLuint WIDTH = 900, HEIGHT = 900;

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
GLuint characterVAO, characterVBO;

struct Planet {
	GLfloat size;
	GLfloat revoPeriod;
	GLfloat ellipseA;
	GLfloat ellipseB;
	GLfloat ellipseE;
	GLfloat nearTheta;
	std::string name;
	GLfloat tmpTheta;
	glm::vec3 color;
	GLfloat fontSize;

	Planet(GLfloat s, GLfloat r, GLfloat a, GLfloat b, GLfloat e, GLfloat n, std::string na, GLfloat t, glm::vec3 c, GLfloat f) {
		size = s;
		revoPeriod = r;
		ellipseA = a;
		ellipseB = b;
		ellipseE = e;
		nearTheta = n;
		name = na;
		tmpTheta = t;
		color = c;
		fontSize = f;
	}
};

Planet planets[5] = {
	Planet(4880.0f, 0.24f, 57.9f, 56.6f, 0.206f, 76.8f, "Mercury", 0.0f, glm::vec3(162.0f/255.0f, 147.0f/255.0f, 250.0f/255.0f), 0.7f),
	Planet(12742.0f, 1.0f, 149.6f, 149.6f,0.017f, 102.3f, "Earth", 0.0f, glm::vec3(128.0f / 255.0f, 228.0f / 255.0f, 254.0f / 255.0f), 0.8f),
	Planet(6794.0f, 1.88f, 227.9f, 226.9f, 0.093f, 335.3f, "Mars", 0.0f, glm::vec3(234.0f / 255.0f, 82.0f / 255.0f, 0.0f / 255.0f), 0.9f),
	Planet(142984.0f, 12.0f, 778.3f, 777.4f, 0.048f, 13.7f, "Jupiter", 0.0f, glm::vec3(236.0f / 255.0f, 230.0f / 255.0f, 172.0f / 255.0f), 3.0f),
	Planet(150536.0f, 29.0f, 1427.0f, 1424.8f, 0.056f, 92.3f, "Saturn", 0.0f, glm::vec3(218.0f / 255.0f, 154.0f / 255.0f, 144.0f / 255.0f), 2.0f),
};

GLfloat speed = 5.0f;
GLfloat scaleSize = 1400.0f;
GLfloat deltaScale = 50.0f;
GLfloat deltaT = 0.01f;
GLfloat unit = 8.0f;

// Key_callback
bool keys[1024];

// Deltatime
GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f;      // Time of last frame


// The MAIN function, from here we start the application and run the game loop
int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "B2", NULL, NULL);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Initialize GLAD to setup the OpenGL Function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);

	// Setup OpenGL options
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Build and compile our shader program
	Shader ourShader("main.vert.glsl", "main.frag.glsl");
	Shader ourShaderCh("text.vert.glsl", "text.frag.glsl");


	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] = {
		// Positions                          // Texture Coords
		-8.0f, -8.0f, 0.0f,  0.0f, 0.0f,
		 8.0f, -8.0f, 0.0f,  1.0f, 0.0f,
		 8.0f,  8.0f, 0.0f,  1.0f, 1.0f,
		 8.0f,  8.0f, 0.0f,  1.0f, 1.0f,
		-8.0f,  8.0f, 0.0f,  0.0f, 1.0f,
		-8.0f, -8.0f, 0.0f,  0.0f, 0.0f,
	};

	GLuint planetVBO, planetVAO;
	glGenVertexArrays(1, &planetVAO);
	glGenBuffers(1, &planetVBO);
	glBindVertexArray(planetVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planetVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// TexCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	// Load and create textures
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	GLuint texture[7];
	int width, height;
	unsigned char* image = SOIL_load_image("1.png", &width, &height, 0, SOIL_LOAD_RGBA);
	for (int i = 0; i < 7; i++)
	{
		glGenTextures(1, &texture[i]);
		glBindTexture(GL_TEXTURE_2D, texture[i]);
		// Set our texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Load, create texture and generate mipmaps
		switch (i)
		{
		case 0:
			image = SOIL_load_image("1.png", &width, &height, 0, SOIL_LOAD_RGBA);
			break;
		case 1:
			image = SOIL_load_image("3.png", &width, &height, 0, SOIL_LOAD_RGBA);
			break;
		case 2:
			image = SOIL_load_image("4.png", &width, &height, 0, SOIL_LOAD_RGBA);
			break;
		case 3:
			image = SOIL_load_image("5.png", &width, &height, 0, SOIL_LOAD_RGBA);
			break;
		case 4:
			image = SOIL_load_image("6.png", &width, &height, 0, SOIL_LOAD_RGBA);
			break;
		case 5:
			image = SOIL_load_image("0.png", &width, &height, 0, SOIL_LOAD_RGBA);
			break;
		case 6:
			image = SOIL_load_image("sky.jpg", &width, &height, 0, SOIL_LOAD_RGBA);
			break;
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);
	}


	// FreeType
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	// Load font as face
	FT_Face face;
	if (FT_New_Face(ft, "arial.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, 48);

	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Load first 128 characters of ASCII set
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint textureCh;
		glGenTextures(1, &textureCh);
		glBindTexture(GL_TEXTURE_2D, textureCh);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			textureCh,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			GLuint(face->glyph->advance.x)
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	// Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &characterVAO);
	glGenBuffers(1, &characterVBO);
	glBindVertexArray(characterVAO);
	glBindBuffer(GL_ARRAY_BUFFER, characterVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);




	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		change_speed();

		// Render
		// Clear the colorbuffer
		glClearColor(31.0f / 255.0f, 28.0f / 255.0f, 99.0f / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
		glm::mat4 model(1);

		// Draw the sky
		glBindVertexArray(planetVAO);
		ourShader.Use();
		glBindTexture(GL_TEXTURE_2D, texture[6]);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
		model = glm::scale(model, glm::vec3(0.14f, 0.14f, 0.14f));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);


		// Draw the sun
		glBindVertexArray(planetVAO);
		ourShader.Use();
		glBindTexture(GL_TEXTURE_2D, texture[5]);

		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(5.0f/scaleSize, 5.0f / scaleSize, 5.0f / scaleSize));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		RenderText(ourShaderCh, "Sun", -3.0f * unit / scaleSize, 3.0f * unit / scaleSize, 1.0f / scaleSize, glm::vec3(236.0f / 255.0f, 134.0f / 255.0f, 109.0f / 255.0f));

		// Draw planets
		for (int i = 0; i < 5; i++)
		{
			//std::cout << planets[i].name << std::endl;
			GLfloat theta = 360.0f / (planets[i].revoPeriod * speed);
			planets[i].tmpTheta = planets[i].tmpTheta + theta * deltaTime;
			if (planets[i].tmpTheta >= 360.0f)
				planets[i].tmpTheta = planets[i].tmpTheta - 360.0f;
			GLfloat a = (planets[i].ellipseA + planets[i].ellipseB) / 2.0f;
			GLfloat r = (a * (1.0f - pow(planets[i].ellipseE, 2.0f))) / (1.0f + planets[i].ellipseE * cos(glm::radians(planets[i].tmpTheta)));
			GLfloat x = r * cos(glm::radians(planets[i].tmpTheta + planets[i].nearTheta));
			GLfloat y = r * sin(glm::radians(planets[i].tmpTheta + planets[i].nearTheta));

			model = glm::mat4(1);
			//std::cout << (x / scaleSize) << " " << (y / scaleSize) << std::endl;
			model = glm::translate(model, glm::vec3(x / scaleSize, y / scaleSize, 0.0f));
			model = glm::scale(model, glm::vec3(planets[i].size / planets[0].size / scaleSize, planets[i].size / planets[0].size / scaleSize, planets[i].size / planets[0].size / scaleSize));

			glBindVertexArray(planetVAO);
			ourShader.Use();
			glBindTexture(GL_TEXTURE_2D, texture[i]);

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);

			RenderText(ourShaderCh, planets[i].name, (x - planets[i].size / planets[0].size * unit) / scaleSize, (y + planets[i].size / planets[0].size * unit) / scaleSize, planets[i].fontSize / scaleSize, planets[i].color);
		}

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &planetVAO);
	glDeleteBuffers(1, &planetVBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}


void RenderText(Shader& shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	// Activate corresponding render state
	glBindVertexArray(characterVAO);
	shader.Use();
	glUniform3f(glGetUniformLocation(shader.Program, "textColor"), color.x, color.y, color.z);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;

		// Update VBO for each character
		GLfloat verticesCh[6][4] = {
			{ xpos,     ypos + h,   0.0, 1.0 },
			{ xpos,     ypos,       0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 0.0 },

			{ xpos,     ypos + h,   0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 0.0 },
			{ xpos + w, ypos + h,   1.0, 1.0 }
		};

		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, characterVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verticesCh), verticesCh); // Be sure to use glBufferSubData and not glBufferData
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}


// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void change_speed()
{
	if (keys[GLFW_KEY_UP])
	{
		if (speed - deltaT * speed > 5.0f)
			speed -= deltaT * speed;
	}
	if (keys[GLFW_KEY_DOWN])
	{
		if (speed + deltaT * speed < 3000.0f)
			speed += deltaT * speed;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	scaleSize = scaleSize - yoffset * deltaScale;
	//std::cout << scaleSize << std::endl;
	if (scaleSize <= 0.0f) {
		scaleSize = 50.0f;
	}
	else if (scaleSize >= 2500.0f) {
		scaleSize = 2500.0f;
	}
}