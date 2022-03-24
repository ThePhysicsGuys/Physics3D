[common]

#version 330 core

//------------------------------------------------------------------------------//

[vertex]

layout(location = 0) in vec3 vPosition;

uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;

void main() {
	gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(vPosition, 1.0);
}

//------------------------------------------------------------------------------//

[fragment]

out vec4 outColor;

uniform vec4 color = vec4(1, 1, 1, 1);

void main() {
	outColor = color;
}