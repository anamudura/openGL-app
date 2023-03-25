#version 410 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoords;

out vec3 fPosition;
out vec3 fNormal;
out vec2 fTexCoords;
out vec4 fragmentPosEyeSpace;
out vec4 fPosEye;
out vec3 vPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform	mat3 normalMatrix;
uniform mat4 lightSpaceTrMatrix;
uniform int fogen;

void main() 
{

	fragmentPosEyeSpace = lightSpaceTrMatrix * model * vec4(vPosition, 1.0f);
	fPosEye = view * model * vec4(vPosition, 1.0f);
	fPosition = vPosition;
	fNormal = normalize(normalMatrix * vNormal);
	fTexCoords = vTexCoords;
	gl_Position = projection * view * model * vec4(vPosition, 1.0f);
	vPos=vPosition;
}
