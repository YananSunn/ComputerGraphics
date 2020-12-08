#version 410 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in float pointOrd;

out vec2 TexCoord;
out vec3 RealPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform int xAxis;
uniform int zAxis;
uniform vec4 yAxis;

void main()
{
	float tmpY = 30.0;
	if(pointOrd == 0.0){
		tmpY = yAxis.x;
	}
	else if(pointOrd == 1.0){
		tmpY = yAxis.y;
	}
	else if(pointOrd == 2.0){
		tmpY = yAxis.z;
	}
	else if(pointOrd == 3.0){
		tmpY = yAxis.w;
	}
	RealPosition = vec3(position.x + float(xAxis), tmpY, position.z + float(zAxis));
    gl_Position = projection * view * model * vec4(RealPosition, 1.0f);
	TexCoord = vec2(texCoord.x + (float(xAxis)/255.0), 1.0f - (float(zAxis)/255.0) + texCoord.y);
}
