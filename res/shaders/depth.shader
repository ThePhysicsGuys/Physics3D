[common]

#version 330 core

//------------------------------------------------------------------------------//

[vertex]

layout(location = 0) in vec3 vPosition; 

uniform mat4 modelMatrix;
uniform mat4 lightMatrix;

void main() {
	gl_Position = lightMatrix * modelMatrix * vec4(vPosition, 1.0);
}

//------------------------------------------------------------------------------//

[fragment]

void main() {}