#include <iostream>
#include <cmath>
#include<algorithm>
#include<cstdio>

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


// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();
GLuint do_select();
GLfloat do_change();

// Window dimensions
GLuint WIDTH = 800, HEIGHT = 600;

// Camera
Camera  camera(glm::vec3(-1.0f, 1.3f, 1.2f), glm::vec3(0.0f, 1.0f, 0.0f), -57.0f, -22.0f);
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool    keys[1024];
bool firstMouse = true;
GLuint changeType = 0;
GLfloat direction = 0.0f;

// Light attributes
glm::vec3 lightPos(2.8f, 3.0f, 2.2f);

// Deltatime
GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f;      // Time of last frame

// The MAIN function, from here we start the application and run the game loop
int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Light2", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize GLAD to setup the OpenGL Function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);


	// Build and compile our shader program
	Shader lightingShader("material.vert.glsl", "material.frag.glsl");
	Shader lampShader("lamp.vert.glsl", "lamp.frag.glsl");

	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] = {
		0.0f,  2.0f,   2.0f,  0.0f, -1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
		2.0f,  2.0f,   2.0f,  0.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
		0.0f,  2.0f,   4.0f,  0.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		2.0f,  2.0f,   2.0f,  0.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
		2.0f,  2.0f,   4.0f,  0.0f, -1.0f, 0.0f,   1.0f, 1.0f, 1.0f,
		0.0f,  2.0f,   4.0f,  0.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,

		0.0f,  2.0f,   2.0f,   0.0f, 1.414f, -1.0f,  1.0f, 0.0f, 0.0f,
		1.0f,  3.414f, 3.0f,   0.0f, 1.414f, -1.0f,  0.0f, 1.0f, 1.0f,
		2.0f,  2.0f,   2.0f,   0.0f, 1.414f, -1.0f,  0.0f, 1.0f, 0.0f,

		2.0f,  2.0f,   2.0f,   1.0f, 1.414f, 0.0f,  0.0f, 1.0f, 0.0f,
		1.0f,  3.414f, 3.0f,   1.0f, 1.414f, 0.0f,  0.0f, 1.0f, 1.0f,
		2.0f,  2.0f,   4.0f,   1.0f, 1.414f, 0.0f,  1.0f, 1.0f, 1.0f,

		2.0f,  2.0f,   4.0f,  0.0f, 1.414f, 1.0f,   1.0f, 1.0f, 1.0f,
		1.0f,  3.414f, 3.0f,  0.0f, 1.414f, 1.0f,   0.0f, 1.0f, 1.0f,
		0.0f,  2.0f,   4.0f,  0.0f, 1.414f, 1.0f,   0.0f, 0.0f, 1.0f,

		0.0f,  2.0f,   4.0f,   -1.0f, 1.414f, 0.0f,  0.0f, 0.0f, 1.0f,
		1.0f,  3.414f, 3.0f,   -1.0f, 1.414f, 0.0f,  0.0f, 1.0f, 1.0f,
		0.0f,  2.0f,   2.0f,   -1.0f, 1.414f, 0.0f,  1.0f, 0.0f, 0.0f,
	};
	// First, set the container's VAO (and VBO)
	GLuint VBO, containerVAO;
	glGenVertexArrays(1, &containerVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(containerVAO);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	// Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Set the vertex attributes (only position data for the lamp))
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0); // Note that we skip over the normal vectors
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);


	glm::vec3 ambientColor = glm::vec3(0.2f, 0.2f, 0.2f);
	glm::vec3 diffuseColor = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 matColor = glm::vec3(0.5f, 0.5f, 0.5f);
	GLfloat matShine = 32.0f;


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
		GLint tmp = do_select();
		if (tmp != 0 && tmp != changeType) {
			changeType = tmp;
		}
		GLfloat tmp2 = do_change();
		if (tmp2 != 0.0) {
			std::cout << direction << " " << tmp2 << std::endl;
			direction = tmp2;
			
		}
		 

		// Clear the colorbuffer

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);


		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();
		GLint lightPosLoc = glGetUniformLocation(lightingShader.Program, "light.position");
		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

		// light properties

		GLint lightAmbientLoc = glGetUniformLocation(lightingShader.Program, "light.ambient");
		GLint lightDiffuseLoc = glGetUniformLocation(lightingShader.Program, "light.diffuse");
		GLint lightSpecularLoc = glGetUniformLocation(lightingShader.Program, "light.specular");
		glUniform3f(lightAmbientLoc, ambientColor.x, ambientColor.y, ambientColor.z);
		glUniform3f(lightDiffuseLoc, diffuseColor.x, diffuseColor.y, diffuseColor.z);
		glUniform3f(lightSpecularLoc, specularColor.x, specularColor.y, specularColor.z);


		//glm::vec3 lightColor;
		//lightColor.x = sin(glfwGetTime() * 2.0f);
		//lightColor.y = sin(glfwGetTime() * 0.7f);
		//lightColor.z = sin(glfwGetTime() * 1.3f);
		//glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
		//glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
		//GLint lightAmbientLoc = glGetUniformLocation(lightingShader.Program, "light.ambient");
		//GLint lightDiffuseLoc = glGetUniformLocation(lightingShader.Program, "light.diffuse");
		//GLint lightSpecularLoc = glGetUniformLocation(lightingShader.Program, "light.specular");
		//glUniform3f(lightAmbientLoc, ambientColor.x, ambientColor.y, ambientColor.z);
		//glUniform3f(lightDiffuseLoc, diffuseColor.x, diffuseColor.y, diffuseColor.z);
		//glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);

		// material properties
		//GLint matAmbientLoc = glGetUniformLocation(lightingShader.Program, "material.ambient");
		//GLint matDiffuseLoc = glGetUniformLocation(lightingShader.Program, "material.diffuse");
		GLint matSpecularLoc = glGetUniformLocation(lightingShader.Program, "material.specular");
		GLint matShineLoc = glGetUniformLocation(lightingShader.Program, "material.shininess");

		//glUniform3f(matAmbientLoc, 1.0f, 0.5f, 0.31f);
		//glUniform3f(matDiffuseLoc, 1.0f, 0.5f, 0.31f);
		glUniform3f(matSpecularLoc, matColor.x, matColor.y, matColor.z);
		glUniform1f(matShineLoc, matShine);

		if (changeType == 1 && direction != 0.0f) {
			GLfloat tmp = ambientColor.x + 0.1f * direction;
			std::cout << direction<<" " <<tmp << std::endl;
			if (tmp > 1.0f) {
				tmp = 1.0f;
			}
			else if (tmp < 0.1f) {
				tmp = 0.1f;
			}
			ambientColor = glm::vec3(tmp, tmp, tmp);
			direction = 0;
			keys[GLFW_KEY_UP] = false;
			keys[GLFW_KEY_DOWN] = false;
		}
		else if (changeType == 2 && direction != 0.0f) {
			GLfloat tmp = diffuseColor.x + 0.1f * direction;
			if (tmp > 1.0f) {
				tmp = 1.0f;
			}
			else if (tmp < 0.1f) {
				tmp = 0.1f;
			}
			diffuseColor = glm::vec3(tmp, tmp, tmp);
			direction = 0;
			keys[GLFW_KEY_UP] = false;
			keys[GLFW_KEY_DOWN] = false;
		}
		else if (changeType == 3 && direction != 0.0f) {
			GLfloat tmp = specularColor.x + 0.1f * direction;
			if (tmp > 1.0f) {
				tmp = 1.0f;
			}
			else if (tmp < 0.1f) {
				tmp = 0.1f;
			}
			specularColor = glm::vec3(tmp, tmp, tmp);
			direction = 0;
			keys[GLFW_KEY_UP] = false;
			keys[GLFW_KEY_DOWN] = false;
		}
		else if (changeType == 4 && direction != 0.0f) {
			GLfloat tmp = matColor.x + 0.1f * direction;
			if (tmp > 1.0f) {
				tmp = 1.0f;
			}
			else if (tmp < 0.1f) {
				tmp = 0.1f;
			}
			matColor = glm::vec3(tmp, tmp, tmp);
			direction = 0;
			keys[GLFW_KEY_UP] = false;
			keys[GLFW_KEY_DOWN] = false;
		}
		else if (changeType == 5 && direction != 0.0f) {
			GLfloat tmp = matShine + 5.0f * direction;
			if (tmp  > 256.0f) {
				tmp = 256.0f;
			}
			else if (tmp < 2.0f) {
				tmp = 2.0f;
			}
			matShine = tmp;
			direction = 0;
			keys[GLFW_KEY_UP] = false;
			keys[GLFW_KEY_DOWN] = false;
		}


		// Create camera transformations
		glm::mat4 view(1);
		view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Draw the container (using container's vertex attributes)
		glBindVertexArray(containerVAO);
		glm::mat4 model(1);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 18);
		glBindVertexArray(0);

		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");
		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		model = glm::mat4(1);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		// Draw the light object (using light's vertex attributes)
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 18);
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

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
		std::cout << key << std::endl;
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void do_movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (keys[GLFW_KEY_R])
		camera.ProcessKeyboard(UP, deltaTime);
	if (keys[GLFW_KEY_F])
		camera.ProcessKeyboard(DOWN, deltaTime);
}

GLuint do_select() {
	if (keys[GLFW_KEY_1]) {
		return 1;
	}
	else if (keys[GLFW_KEY_2]) {
		return 2;
	}
	else if (keys[GLFW_KEY_3]) {
		return 3;
	}
	else if (keys[GLFW_KEY_4]) {
		return 4;
	}
	else if (keys[GLFW_KEY_5]) {
		return 5;
	}
	return 0;
}

GLfloat do_change() {
	if (keys[GLFW_KEY_UP]) {
		return 1.0f;
	}
	else if (keys[GLFW_KEY_DOWN]) {
		return -1.0f;
	}
	return 0.0f;
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
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
