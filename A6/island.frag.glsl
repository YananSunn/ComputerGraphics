#version 410 core

in vec2 TexCoord;
in vec3 RealPosition;

out vec4 color;

uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;

void main()
{
	if(RealPosition.y < 0.0)
        discard;
	
	vec4 texColor = vec4(texture(ourTexture, TexCoord).x+texture(ourTexture2, vec2(TexCoord.x*10.0, TexCoord.y*10.0)).x-0.5, texture(ourTexture, TexCoord).y+texture(ourTexture2, vec2(TexCoord.x*10.0, TexCoord.y*10.0)).y-0.5, texture(ourTexture, TexCoord).z+texture(ourTexture2, vec2(TexCoord.x*10.0, TexCoord.y*10.0)).z-0.5, 1.0);
	if(texColor.x < 0.0){
		texColor.x = 0.0;
	}
	if(texColor.y < 0.0){
		texColor.y = 0.0;
	}
	if(texColor.z < 0.0){
		texColor.z = 0.0;
	}
    color = texColor;
}