#include <iostream>
#include <cmath>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include <SOIL.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "shader.h"


// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
GLfloat getRandomColor();

// Window dimensions
GLuint WIDTH = 800, HEIGHT = 600;
const GLint maxStarSize = 60;

GLfloat a = 0.0f, b = 0.05f, generateRate = 0.2f;

// The MAIN function, from here we start the application and run the game loop
int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "B1", NULL, NULL);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);

	// Initialize GLAD to setup the OpenGL Function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// Build and compile our shader program
	Shader ourShader("main.vert.glsl", "main.frag.glsl");


	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] = {
		// x     y     z    texture coordinates
		-0.2f,  -0.2f,   0.0f,    0.0f, 0.0f,    1.0f,  1.0f,  1.0f,
		 0.2f,  -0.2f,   0.0f,    1.0f, 0.0f,    1.0f,  0.0f,  0.0f,
		 0.2f,   0.2f,   0.0f,    1.0f, 1.0f,    1.0f,  1.0f,  0.0f,
		 0.2f,   0.2f,   0.0f,    1.0f, 1.0f,    1.0f,  1.0f,  0.0f,
		-0.2f,   0.2f,   0.0f,    0.0f, 1.0f,    0.0f,  1.0f,  1.0f,
		-0.2f,  -0.2f,   0.0f,    0.0f, 0.0f,    1.0f,  1.0f,  1.0f,
	};
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Texture attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Color attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO


	// Load and create a texture
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
	// Set our texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    // Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load, create texture and generate mipmaps
	int width, height;
	unsigned char* image = SOIL_load_image("Star.bmp", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	GLint starSize = 1;
	GLint starIndex = 0;
	GLfloat timeRange = 0;
	glm::vec4 starColor[maxStarSize];

	for (GLuint i = 0; i < maxStarSize; i++) {
		starColor[i] = glm::vec4(getRandomColor(), getRandomColor(), getRandomColor(), 1.0f);
	}


	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Activate shader
		ourShader.Use();

		// Bind Textures using texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(ourShader.Program, "ourTexture"), 0);

		starIndex = floor((glfwGetTime() - timeRange * (GLfloat(maxStarSize) * generateRate)) / generateRate);

		int vertexColorLocation = glGetUniformLocation(ourShader.Program, "ourColor");

		if ( glfwGetTime() < GLfloat(maxStarSize) * generateRate) {
			starSize = GLint(floor(glfwGetTime()/generateRate));
		}

		if (glfwGetTime() - timeRange * (GLfloat(maxStarSize) * generateRate) > GLfloat(maxStarSize)* generateRate) {
			timeRange = timeRange + 1.0f;
		}

		

		// Camera/View transformation
		glm::mat4 view(1);
		GLfloat radius = 10.0f;
		GLfloat camX = sin(glfwGetTime()) * radius; // set the camera to rotate on a circle
		GLfloat camZ = cos(glfwGetTime()) * radius;
		view = glm::lookAt(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		// Projection
		glm::mat4 projection(1);
		projection = glm::perspective(glm::radians(45.0f), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
		GLint projLoc = glGetUniformLocation(ourShader.Program, "projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAO);

		for (GLuint i = 0; i < starSize; i++)// draw 10 cubes, each at a different location
		{
			GLint index = starIndex - i;
			if (index < 0) {
				index = index + maxStarSize;
			}
			glUniform4f(vertexColorLocation, starColor[index].x, starColor[index].y, starColor[index].z, starColor[index].w);

			// Calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model(1);
			model = glm::translate(model, glm::vec3((a + b * ((glfwGetTime() / generateRate) - floor((glfwGetTime() / generateRate)) + i)) * cos((((glfwGetTime() / generateRate) - floor((glfwGetTime() / generateRate)) + i) / 2.0f)), (a + b * ((glfwGetTime() / generateRate) - floor((glfwGetTime() / generateRate)) + i)) * sin((((glfwGetTime() / generateRate) - floor((glfwGetTime() / generateRate)) + i) / 2.0f)), 0.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

GLfloat getRandomColor() {
	GLfloat color = 0.0f;
	while (color < 0.5f){
		color = GLfloat(rand() % 255) / 255.0f;
	}
	return color;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	WIDTH = width;
	HEIGHT = height;
	glViewport(0, 0, width, height);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}
