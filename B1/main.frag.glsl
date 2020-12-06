#version 410 core

//in vec3 ourColor;
in vec2 TexCoord;

out vec4 color;

uniform sampler2D ourTexture;
uniform vec4 ourColor;

void main()
{
    
	float tmp = 0.0;
	if (pow((TexCoord.x - 0.5), 2) + pow((TexCoord.y - 0.5), 2) > 0.09){
		tmp = 0.0;
	}
	else{
		tmp = 1.0 - ((pow((TexCoord.x - 0.5), 2)+pow((TexCoord.y - 0.5), 2)))/0.09;
	}
	vec4 texColor = texture(ourTexture, TexCoord) * vec4(ourColor.x, ourColor.y, ourColor.z, tmp);
	color = texColor;
}