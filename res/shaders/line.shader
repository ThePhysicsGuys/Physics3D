[common]

#version 330 core

//------------------------------------------------------------------------------//

[vertex]

layout (location = 0) in vec3 vposition;
layout (location = 1) in vec4 vcolor;

out vec4 fcolor;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {
	gl_Position = projectionMatrix * viewMatrix * vec4(vposition, 1.0);
	fcolor = vcolor;
}

//------------------------------------------------------------------------------//

[fragment]

in vec4 fcolor;

out vec4 outColor;

void main() {
	outColor = fcolor;
}