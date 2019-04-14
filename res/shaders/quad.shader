#shader vertex
#version 330 core

layout(location = 0) in vec4 positions;

out vec2 ftextureUV;

void main() {
	gl_Position = vec4(positions.xy, 0.0, 1.0);
	ftextureUV = positions.zw;
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