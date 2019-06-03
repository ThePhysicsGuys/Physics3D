#shader vertex
#version 330 core

layout (location = 0) in vec4 positions;

out vec2 fUV;

uniform mat4 projectionMatrix;

void main() {
	gl_Position = projectionMatrix * vec4(positions.xy, 0.0, 1.0);
	fUV = positions.zw;
}

////////////////////////////////////////////////////////////

#shader fragment
#version 330 core

in vec2 fUV;

out vec4 outColor;

uniform bool textured;
uniform sampler2D textureSampler;
uniform vec4 color;

void main() {
	outColor = (textured)? texture(textureSampler, fUV) : color;
}