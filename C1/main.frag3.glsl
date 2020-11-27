#version 330 core
in vec3 ourColor3;

out vec4 color3;

void main()
{
    color3 = vec4(ourColor3, 1.0f);
}