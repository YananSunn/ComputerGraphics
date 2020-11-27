#version 330 core
flat in vec3 ourColor2;

out vec4 color2;

void main()
{
    color2 = vec4(ourColor2, 1.0f);
}