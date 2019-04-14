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

const float offset = 1.0 / 300.0;

void main() {
	vec2 offsets[9] = vec2[] (
		vec2(-offset,  offset ), 
		vec2( 0.0f,    offset ), 
		vec2( offset,  offset ), 
		vec2(-offset,  0.0f   ), 
		vec2( 0.0f,    0.0f   ), 
		vec2( offset,  0.0f   ), 
		vec2(-offset, -offset ), 
		vec2( 0.0f,   -offset ), 
		vec2( offset, -offset )  
	);

	/*float kernel[9] = float[] (
		1,  1,  1,
		1, -8,  1,
		1,  1,  1
	);*/

	float kernel[9] = float[] (
		0, 0, 0,
		0, 1, 0,
		0, 0, 0
	);

	vec2 tex = ftextureUV;
	vec3 sampleTexture[9];
	for (int i = 0; i < 9; i++) {
		sampleTexture[i] = vec3(texture(textureSampler, tex.st + offsets[i]));
	}

	vec3 color = vec3(0.0);
	for (int i = 0; i < 9; i++)
		color += sampleTexture[i] * kernel[i];

	outColor = vec4(color, 1.0);
}