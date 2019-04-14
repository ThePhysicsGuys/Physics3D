#shader vertex
#version 330 core

layout(location = 0) in vec2 positions;

uniform mat4 projectionMatrix;

void main() {
	gl_Position = projectionMatrix * vec4(positions.x, positions.y, 0.0, 1.0);
}

////////////////////////////////////////////////////////////

#shader fragment
#version 330 core

out vec4 outColor;

uniform vec4 color;

void main() {
	outColor = color;
}