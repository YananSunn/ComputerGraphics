#version 330 core
layout (location = 0) in vec3 position2;
layout (location = 1) in vec3 color2;

flat out vec3 ourColor2;

uniform mat4 model2;
uniform mat4 view2;
uniform mat4 projection2;

void main()
{
    gl_Position = projection2 * view2 * model2 * vec4(position2, 1.0f);
    ourColor2 = color2;
}