#pragma warning(402 : 4996)

#include <iostream>
#include <cmath>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "shader.h"
#include "camera.h"

// Other Libs
#include <SOIL.h>

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();

// Window dimensions
GLuint WIDTH = 800, HEIGHT = 600;

// Camera
Camera camera(glm::vec3(0.0f, 50.0f, 0.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

// Deltatime
GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f;      // Time of last frame

GLfloat boxSize1 = 400.0f;
GLfloat boxSize2 = 800.0f;
GLfloat boxSize3 = 800.0f;

GLfloat waveSize1 = 10.0f;
GLfloat waveSize2 = 10.0f;

GLfloat waveSpeed = 0.1f;

GLfloat mountHeight = 0.15f;

GLfloat range = 1.0f / 255.0f;
GLfloat xShift = -2.0f;
GLfloat yShift = -14.0f * (1.0f + mountHeight);
GLfloat zShift = -20.0f;

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
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "A6", NULL, NULL);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Initialize GLAD to setup the OpenGL Function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// Build and compile our shader program
	Shader skyShader("sky.vert.glsl", "sky.frag.glsl");
	Shader waveShader("wave.vert.glsl", "wave.frag.glsl");
	Shader islandShader("island.vert.glsl", "island.frag.glsl");
	Shader island2Shader("island2.vert.glsl", "island2.frag.glsl");

	// Load height bmp
	int imgWidth, imgHeight;
	unsigned char* heightImage = SOIL_load_image("heightmap.bmp",
		&imgWidth,
		&imgHeight,
		0,
		SOIL_LOAD_L);


	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] = {
		// x     y     z    texture coordinates

		// 0
		-boxSize1,  boxSize2,  -boxSize3,  0.0f, 1.0f,
		 boxSize1,  boxSize2,  -boxSize3,  1.0f, 1.0f,
		 boxSize1, 0.0f,  -boxSize3,  1.0f, 0.0f,
		 boxSize1, 0.0f,  -boxSize3,  1.0f, 0.0f,
		-boxSize1, 0.0f,  -boxSize3,  0.0f, 0.0f,
		-boxSize1,  boxSize2,  -boxSize3,  0.0f, 1.0f,

		// 1
		boxSize1,  boxSize2,  boxSize3,  1.0f, 1.0f,
		boxSize1,  boxSize2, -boxSize3,  0.0f, 1.0f,
		boxSize1, 0.0f, -boxSize3,  0.0f, 0.0f,
		boxSize1, 0.0f, -boxSize3,  0.0f, 0.0f,
		boxSize1, 0.0f,  boxSize3,  1.0f, 0.0f,
		boxSize1,  boxSize2,  boxSize3,  1.0f, 1.0f,

		// 2
		-boxSize1, 0.0f,  boxSize3,  1.0f, 0.0f,
		 boxSize1, 0.0f,  boxSize3,  0.0f, 0.0f,
		 boxSize1,  boxSize2,  boxSize3,  0.0f, 1.0f,
		 boxSize1,  boxSize2,  boxSize3,  0.0f, 1.0f,
		-boxSize1,  boxSize2,  boxSize3,  1.0f, 1.0f,
		-boxSize1, 0.0f,  boxSize3,  1.0f, 0.0f,

		// 3
		-boxSize1,  boxSize2,  boxSize3,  0.0f, 1.0f,
		-boxSize1,  boxSize2, -boxSize3,  1.0f, 1.0f,
		-boxSize1, 0.0f, -boxSize3,  1.0f, 0.0f,
		-boxSize1, 0.0f, -boxSize3,  1.0f, 0.0f,
		-boxSize1, 0.0f,  boxSize3,  0.0f, 0.0f,
		-boxSize1,  boxSize2,  boxSize3,  0.0f, 1.0f,

		// 4
		-boxSize1,  boxSize2,  boxSize3,  0.0f, 1.0f,
		 boxSize1,  boxSize2,  boxSize3,  1.0f, 1.0f,
		 boxSize1,  boxSize2, -boxSize3,  1.0f, 0.0f,
		 boxSize1,  boxSize2, -boxSize3,  1.0f, 0.0f,
		-boxSize1,  boxSize2, -boxSize3,  0.0f, 0.0f,
		-boxSize1,  boxSize2,  boxSize3,  0.0f, 1.0f,

		// Reversed sky
		// 0
		-boxSize1,  -boxSize2,  -boxSize3,  0.0f, 1.0f,
		 boxSize1,  -boxSize2,  -boxSize3,  1.0f, 1.0f,
		 boxSize1, 0.0f,  -boxSize3,  1.0f, 0.0f,
		 boxSize1, 0.0f,  -boxSize3,  1.0f, 0.0f,
		-boxSize1, 0.0f,  -boxSize3,  0.0f, 0.0f,
		-boxSize1,  -boxSize2,  -boxSize3,  0.0f, 1.0f,

		// 1
		boxSize1,  -boxSize2,  boxSize3,  1.0f, 1.0f,
		boxSize1,  -boxSize2, -boxSize3,  0.0f, 1.0f,
		boxSize1, 0.0f, -boxSize3,  0.0f, 0.0f,
		boxSize1, 0.0f, -boxSize3,  0.0f, 0.0f,
		boxSize1, 0.0f,  boxSize3,  1.0f, 0.0f,
		boxSize1,  -boxSize2,  boxSize3,  1.0f, 1.0f,

		// 2
		-boxSize1, 0.0f,  boxSize3,  1.0f, 0.0f,
		 boxSize1, 0.0f,  boxSize3,  0.0f, 0.0f,
		 boxSize1,  -boxSize2,  boxSize3,  0.0f, 1.0f,
		 boxSize1,  -boxSize2,  boxSize3,  0.0f, 1.0f,
		-boxSize1,  -boxSize2,  boxSize3,  1.0f, 1.0f,
		-boxSize1, 0.0f,  boxSize3,  1.0f, 0.0f,

		// 3
		-boxSize1,  -boxSize2,  boxSize3,  0.0f, 1.0f,
		-boxSize1,  -boxSize2, -boxSize3,  1.0f, 1.0f,
		-boxSize1, 0.0f, -boxSize3,  1.0f, 0.0f,
		-boxSize1, 0.0f, -boxSize3,  1.0f, 0.0f,
		-boxSize1, 0.0f,  boxSize3,  0.0f, 0.0f,
		-boxSize1,  -boxSize2,  boxSize3,  0.0f, 1.0f,

		// 4
		-boxSize1,  -boxSize2,  boxSize3,  0.0f, 1.0f,
		 boxSize1,  -boxSize2,  boxSize3,  1.0f, 1.0f,
		 boxSize1,  -boxSize2, -boxSize3,  1.0f, 0.0f,
		 boxSize1,  -boxSize2, -boxSize3,  1.0f, 0.0f,
		-boxSize1,  -boxSize2, -boxSize3,  0.0f, 0.0f,
		-boxSize1,  -boxSize2,  boxSize3,  0.0f, 1.0f,


		// wave
		-boxSize1, 0.0f, -boxSize3, 0.0f,  waveSize2,
		 boxSize1, 0.0f, -boxSize3, waveSize1, waveSize2,
		 boxSize1, 0.0f,  boxSize3, waveSize1, 0.0f,
		 boxSize1, 0.0f,  boxSize3, waveSize1, 0.0f,
		-boxSize1, 0.0f,  boxSize3, 0.0f,  0.0f,
		-boxSize1, 0.0f, -boxSize3, 0.0f,  waveSize2,
	};

	GLfloat islandVertices[] = {
		// island
		// x     y     z    texture coordinates  point 
		0.0f, 0.0f, 0.0f,		0.0f,				1.0f / 255.0f,	0.0f,
		1.0f, 0.0f, 0.0f,		1.0f / 255.0f,      1.0f / 255.0f,	1.0f,
		1.0f, 0.0f, 1.0f,		1.0f / 255.0f,      0.0f,			2.0f,
		1.0f, 0.0f, 1.0f,		1.0f / 255.0f,      0.0f,			2.0f,
		0.0f, 0.0f, 1.0f,		0.0f,				0.0f,			3.0f,
		0.0f, 0.0f, 0.0f,		0.0f,				1.0f / 255.0f,	0.0f,
	};
	GLuint islandVBO, islandVAO;
	glGenVertexArrays(1, &islandVAO);
	glGenBuffers(1, &islandVBO);
	glBindVertexArray(islandVAO);
	glBindBuffer(GL_ARRAY_BUFFER, islandVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(islandVertices), islandVertices, GL_STATIC_DRAW);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Texture attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Point attribute
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// TexCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO



	// Load and create a texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	GLuint skyTexture[5];
	int width, height;
	unsigned char* image = SOIL_load_image("SkyBox0.bmp", &width, &height, 0, SOIL_LOAD_RGBA);
	for (int i = 0; i < 5; i++)
	{
		glGenTextures(1, &skyTexture[i]);
		glBindTexture(GL_TEXTURE_2D, skyTexture[i]);
		// Set our texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		// Load, create texture and generate mipmaps
		switch (i)
		{
		case 0:
			image = SOIL_load_image("SkyBox0.bmp", &width, &height, 0, SOIL_LOAD_RGBA);
			break;
		case 1:
			image = SOIL_load_image("SkyBox1.bmp", &width, &height, 0, SOIL_LOAD_RGBA);
			break;
		case 2:
			image = SOIL_load_image("SkyBox2.bmp", &width, &height, 0, SOIL_LOAD_RGBA);
			break;
		case 3:
			image = SOIL_load_image("SkyBox3.bmp", &width, &height, 0, SOIL_LOAD_RGBA);
			break;
		case 4:
			image = SOIL_load_image("SkyBox4.bmp", &width, &height, 0, SOIL_LOAD_RGBA);
			break;
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLuint waveTexture;
	glGenTextures(1, &waveTexture);
	glBindTexture(GL_TEXTURE_2D, waveTexture); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
	// Set our texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    // Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	// Load, create texture and generate mipmaps
	image = SOIL_load_image("SkyBox5.bmp", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	GLuint islandTexture;
	glGenTextures(1, &islandTexture);
	glBindTexture(GL_TEXTURE_2D, islandTexture); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
	// Set our texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    // Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	// Load, create texture and generate mipmaps
	image = SOIL_load_image("terrain-texture3.bmp", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.


	GLuint detailTexture;
	glGenTextures(1, &detailTexture);
	glBindTexture(GL_TEXTURE_2D, detailTexture); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
	// Set our texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    // Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	// Load, create texture and generate mipmaps
	image = SOIL_load_image("detail.bmp", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.


	waveShader.Use();
	glUniform1f(glGetUniformLocation(waveShader.Program, "waveSize1"), waveSize1);
	glUniform1f(glGetUniformLocation(waveShader.Program, "waveSize2"), waveSize2);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		do_movement();

		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Activate shader
		skyShader.Use();

		// Camera/View transformation
		glm::mat4 view(1);
		view = camera.GetViewMatrix();
		// Projection
		glm::mat4 projection(1);
		projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 2000.0f);
		// Get the uniform locations
		GLint modelLoc1 = glGetUniformLocation(skyShader.Program, "model");
		GLint viewLoc1 = glGetUniformLocation(skyShader.Program, "view");
		GLint projLoc1 = glGetUniformLocation(skyShader.Program, "projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc1, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc1, 1, GL_FALSE, glm::value_ptr(projection));

		// Activate shader
		waveShader.Use();
		// Get the uniform locations
		GLint modelLoc2 = glGetUniformLocation(waveShader.Program, "model");
		GLint viewLoc2 = glGetUniformLocation(waveShader.Program, "view");
		GLint projLoc2 = glGetUniformLocation(waveShader.Program, "projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc2, 1, GL_FALSE, glm::value_ptr(projection));


		// Activate shader
		islandShader.Use();
		// Get the uniform locations
		GLint modelLoc3 = glGetUniformLocation(islandShader.Program, "model");
		GLint viewLoc3 = glGetUniformLocation(islandShader.Program, "view");
		GLint projLoc3 = glGetUniformLocation(islandShader.Program, "projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc3, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc3, 1, GL_FALSE, glm::value_ptr(projection));

		// Activate shader
		island2Shader.Use();
		// Get the uniform locations
		GLint modelLoc4 = glGetUniformLocation(island2Shader.Program, "model");
		GLint viewLoc4 = glGetUniformLocation(island2Shader.Program, "view");
		GLint projLoc4 = glGetUniformLocation(island2Shader.Program, "projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc4, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc4, 1, GL_FALSE, glm::value_ptr(projection));


		// Draw sky
		skyShader.Use();
		glBindVertexArray(VAO);
		glm::mat4 model(1);
		glUniformMatrix4fv(modelLoc1, 1, GL_FALSE, glm::value_ptr(model));

		for (GLint i = 0; i < 5; i++) {
			// Bind Textures using texture units
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, skyTexture[i]);
			glUniform1i(glGetUniformLocation(skyShader.Program, "ourTexture"), 0);
			glDrawArrays(GL_TRIANGLES, i*6, 6);
		}

		for (GLint i = 0; i < 5; i++) {
			// Bind Textures using texture units
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, skyTexture[i]);
			glUniform1i(glGetUniformLocation(skyShader.Program, "ourTexture"), 0);
			glDrawArrays(GL_TRIANGLES, 30 + i * 6, 6);
		}
		glBindVertexArray(0);


		


		// Draw island
		islandShader.Use();
		glBindVertexArray(islandVAO);
		glm::mat4 model3(1);
		glUniformMatrix4fv(modelLoc3, 1, GL_FALSE, glm::value_ptr(model3));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, islandTexture);
		glUniform1i(glGetUniformLocation(islandShader.Program, "ourTexture"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, detailTexture);
		glUniform1i(glGetUniformLocation(islandShader.Program, "ourTexture2"), 1);

		for (GLint i = 0; i < 255; i++) {
			// z axis
			for (GLint j = 0; j < 255; j++) {
				// x axis
				glUniform1i(glGetUniformLocation(islandShader.Program, "xAxis"), j);
				glUniform1i(glGetUniformLocation(islandShader.Program, "zAxis"), i);
				GLfloat yAxis[4] = { 0.0f };
				yAxis[0] = heightImage[i * 256 + j] * mountHeight + yShift;
				yAxis[1] = heightImage[i * 256 + j + 1] * mountHeight + yShift;
				yAxis[2] = heightImage[(i + 1) * 256 + j + 1] * mountHeight + yShift;
				yAxis[3] = heightImage[(i + 1) * 256 + j] * mountHeight + yShift;
				glUniform4f(glGetUniformLocation(islandShader.Program, "yAxis"), yAxis[0], yAxis[1], yAxis[2], yAxis[3]);
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
		}
		glBindVertexArray(0);


		// Draw island inverse
		island2Shader.Use();
		glBindVertexArray(islandVAO);
		glm::mat4 model4(1);
		glUniformMatrix4fv(modelLoc4, 1, GL_FALSE, glm::value_ptr(model4));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, islandTexture);
		glUniform1i(glGetUniformLocation(island2Shader.Program, "ourTexture"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, detailTexture);
		glUniform1i(glGetUniformLocation(island2Shader.Program, "ourTexture2"), 1);

		for (GLint i = 0; i < 255; i++) {
			// z axis
			for (GLint j = 0; j < 255; j++) {
				// x axis
				glUniform1i(glGetUniformLocation(island2Shader.Program, "xAxis"), j);
				glUniform1i(glGetUniformLocation(island2Shader.Program, "zAxis"), i);
				GLfloat yAxis[4] = { 0.0f };
				yAxis[0] = heightImage[i * 256 + j] * mountHeight + yShift;
				yAxis[1] = heightImage[i * 256 + j + 1] * mountHeight + yShift;
				yAxis[2] = heightImage[(i + 1) * 256 + j + 1] * mountHeight + yShift;
				yAxis[3] = heightImage[(i + 1) * 256 + j] * mountHeight + yShift;
				glUniform4f(glGetUniformLocation(island2Shader.Program, "yAxis"), -yAxis[0], -yAxis[1], -yAxis[2], -yAxis[3]);

				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
		}

		glBindVertexArray(0);

		// Draw wave
		waveShader.Use();
		glBindVertexArray(VAO);
		glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(model));

		glUniform1f(glGetUniformLocation(waveShader.Program, "waveShift"), float(glfwGetTime())* waveSpeed);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, waveTexture);
		glUniform1i(glGetUniformLocation(waveShader.Program, "ourTexture"), 0);
		glDrawArrays(GL_TRIANGLES, 60, 6);
		

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
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void do_movement()
{
	// Camera controls
	GLfloat cameraSpeed = 5.0f * deltaTime;
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, 5.0f * deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, 5.0f * deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, 5.0f * deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, 5.0f * deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to left
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
