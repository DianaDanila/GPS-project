#version 410 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoords;

out vec3 normal;
out vec4 fragPosEye;
out vec2 fragTexCoords;
out vec3 fragNormal;
out vec3 fragPosition;
out vec4 fragPosLightSpace;
out vec3 fragFog;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main() {

	fragTexCoords = vTexCoords;
	fragNormal = vNormal;
	fragPosition = vPosition;
	fragPosLightSpace = lightSpaceMatrix * model * vec4(vPosition, 1.0f);
	fragPosEye = view * model * vec4(vPosition, 1.0f);
	fragFog = vec3(fragPosEye);
	gl_Position = projection * view * model * vec4(vPosition, 1.0f);
}
