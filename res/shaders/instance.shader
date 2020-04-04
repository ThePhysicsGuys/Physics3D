[common]
#version 330 core

[vertex]
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;
layout(location = 3) in mat4 vModelMatrix;

out vec3 fColor;

uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

void main() {
	modelMatrix;
    gl_Position = projectionMatrix * viewMatrix * vModelMatrix * vec4(vPosition, 1.0);
}  

[fragment]
out vec4 outColor;

void main() {
    outColor = vec4(1, 0, 0, 1);
}
