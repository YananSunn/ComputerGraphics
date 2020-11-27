#include <iostream>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "Shader.h"

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// The MAIN function, from here we start the application and run the game loop
int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "C1", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);

	// Initialize GLAD to setup the OpenGL Function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize glad" << std::endl;
		return -1;
	};

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);


	// Build and compile our shader program
	Shader ourShader("main.vert.glsl", "main.frag.glsl");
	Shader ourShader2("main.vert2.glsl", "main.frag2.glsl");
	Shader ourShader3("main.vert3.glsl", "main.frag3.glsl");


	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] = {
		// Positions         // Colors
		0.1f, -0.4f, 0.0f, 1.0f, 0.0f, 0.0f,  // Bottom Right
		-0.8f, -0.3f, 0.0f, 0.0f, 1.0f, 0.0f,  // Bottom Left
		-0.4f, 0.3f, 0.0f, 0.0f, 0.0f, 1.0f,   // Top 

		0.8f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom right
		0.2f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // bottom left
		0.8f, -0.1f, 0.0f, 0.0f, 0.0f, 1.0f,   // top right
		0.2f, -0.1f, 0.0f, 0.5f, 0.5f, 0.5f,   // top left

		0.8f, 0.1f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom right
		0.2f, 0.1f, 0.0f, 0.0f, 1.0f, 1.0f,  // bottom left
		0.8f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f,   // top right
		0.2f, 0.5f, 0.0f, 0.5f, 0.5f, 0.5f,   // top left
	};
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO


	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Create transformations
		glm::mat4 model(1);
		glm::mat4 view(1);
		glm::mat4 projection(1);

		view = glm::translate(view, glm::vec3(-0.7f, 0.3f, -3.0f));
		model = glm::rotate(model, (GLfloat)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		// Note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		projection = glm::perspective(glm::radians(45.0f), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		// Get their uniform location
		GLint modelLoc = glGetUniformLocation(ourShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(ourShader.Program, "view");
		GLint projLoc = glGetUniformLocation(ourShader.Program, "projection");
		// Pass them to the shaders
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Draw the triangle
		ourShader.Use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		// Create transformations
		glm::mat4 model2(1);
		glm::mat4 view2(1);
		glm::mat4 projection2(1);

		view2 = glm::translate(view2, glm::vec3(0.5f, -0.5f, -3.0f));
		model2 = glm::rotate(model2, (GLfloat)glfwGetTime() * glm::radians(-50.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		
		// Note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		projection2 = glm::perspective(glm::radians(45.0f), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		// Get their uniform location
		GLint modelLoc2 = glGetUniformLocation(ourShader2.Program, "model2");
		GLint viewLoc2 = glGetUniformLocation(ourShader2.Program, "view2");
		GLint projLoc2 = glGetUniformLocation(ourShader2.Program, "projection2");
		// Pass them to the shaders
		glUniformMatrix4fv(modelLoc2, 1, GL_FALSE, glm::value_ptr(model2));
		glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, glm::value_ptr(view2));
		glUniformMatrix4fv(projLoc2, 1, GL_FALSE, glm::value_ptr(projection2));

		ourShader2.Use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 3, 4);
		glBindVertexArray(0);


		// Create transformations
		glm::mat4 model3(1);
		glm::mat4 view3(1);
		glm::mat4 projection3(1);

		view3 = glm::translate(view3, glm::vec3(-0.5f, -0.3f, -3.0f));
		model3 = glm::rotate(model3, (GLfloat)glfwGetTime() * glm::radians(80.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		// Note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		projection3 = glm::perspective(glm::radians(45.0f), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
		// Get their uniform location
		GLint modelLoc3 = glGetUniformLocation(ourShader3.Program, "model3");
		GLint viewLoc3 = glGetUniformLocation(ourShader3.Program, "view3");
		GLint projLoc3 = glGetUniformLocation(ourShader3.Program, "projection3");
		// Pass them to the shaders
		glUniformMatrix4fv(modelLoc3, 1, GL_FALSE, glm::value_ptr(model3));
		glUniformMatrix4fv(viewLoc3, 1, GL_FALSE, glm::value_ptr(view3));
		glUniformMatrix4fv(projLoc3, 1, GL_FALSE, glm::value_ptr(projection3));

		ourShader3.Use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 7, 4);
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

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}