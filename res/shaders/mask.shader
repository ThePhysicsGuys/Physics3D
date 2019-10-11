[common]

#version 330 core

//------------------------------------------------------------------------------//

[vertex]

layout(location = 0) in vec3 vposition;

uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

void main() {
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vposition, 1.0);
}

//------------------------------------------------------------------------------//

[fragment]

out vec4 outColor;

uniform vec4 color = vec4(1, 1, 1, 1);

void main() {
	outColor = color;
}