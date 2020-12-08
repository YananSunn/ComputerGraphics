#version 410 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float waveShift;
uniform float waveSize1;
uniform float waveSize2;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
	float x = texCoord.x + waveShift;
	float y = texCoord.y + waveShift;
	TexCoord = vec2(x, y);
}
