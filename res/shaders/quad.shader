#shader vertex
#version 330 core

layout(location = 0) in vec4 positions;

uniform mat4 projectionMatrix;

out vec2 fUV;

void main() {
	gl_Position = projectionMatrix * vec4(positions.xy, 0.0, 1.0);
	fUV = positions.zw;
}

////////////////////////////////////////////////////////////

#shader fragment
#version 330 core

out vec4 outColor;

in vec2 fUV;

uniform bool textured;
uniform sampler2D textureSampler;
uniform vec4 color = vec4(1, 1, 1, 1);

void main() {
	outColor = (textured) ? texture(textureSampler, fUV) * color : color;
}