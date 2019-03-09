#shader vertex
#version 330 core

layout(location = 0) in vec2 positions;
layout(location = 1) in vec2 textureUV;

out vec2 ftextureUV;

void main() {
	gl_Position = vec4(positions.x, positions.y, 0.0, 1.0);
	ftextureUV = textureUV;
}

////////////////////////////////////////////////////////////

#shader fragment
#version 330 core

in vec2 ftextureUV;
out vec4 outColor;

uniform sampler2D textureSampler;

void main() {
	outColor = texture(textureSampler, ftextureUV);
}