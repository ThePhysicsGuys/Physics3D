#shader vertex
#version 330 core

layout(location = 0) in vec4 positions;

uniform mat4 projectionMatrix;

out vec2 ftextureUV;

void main() {
	gl_Position = projectionMatrix * vec4(positions.x, positions.y, 0.0, 1.0);
	ftextureUV = positions.zw;
}

////////////////////////////////////////////////////////////

#shader fragment
#version 330 core

out vec4 outColor;

in vec2 ftextureUV;

uniform bool textured;
uniform sampler2D textureSampler;
uniform vec4 color;

void main() {
	outColor = (textured)? texture(textureSampler, ftextureUV) : color;
}