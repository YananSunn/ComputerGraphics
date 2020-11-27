#version 330 core
layout (location = 0) in vec3 position3;
layout (location = 1) in vec3 color3;

out vec3 ourColor3;

uniform mat4 model3;
uniform mat4 view3;
uniform mat4 projection3;

void main()
{
    gl_Position = projection3 * view3 * model3 * vec4(position3, 1.0f);
    ourColor3 = color3;
}