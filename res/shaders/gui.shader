#shader vertex
#version 330 core

layout(location = 0) in vec2 vposition;
layout(location = 1) in vec2 vuv;
layout(location = 2) in vec4 vcolor;

uniform mat4 projectionMatrix;

out vec4 fcolor;
out vec2 fuv;

void main() {
	gl_Position = projectionMatrix * vec4(vposition, 0.0, 1.0);
	fcolor = vcolor;
	fuv = vuv;
}

////////////////////////////////////////////////////////////

#shader fragment
#version 330 core

out vec4 outColor;

in vec2 fuv;
in vec4 fcolor;

uniform sampler2D textureSampler;
uniform bool textured = false;

void main() {
	outColor = (textured) ? texture(textureSampler, fuv) * fcolor : fcolor;
}