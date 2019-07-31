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

const float offset = 1.0 / 600.0;

float rbgToLin(float channel) {
	if (channel <= 0.04045) {
		return channel / 12.92;
	} else {
		return pow((channel + 0.055) / 1.055, 2.4);
	}
}

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

	float gx[9] = float[] (
		-1, 0, 1,
		-1, 0, 1,
		-1, 0, 1
	);

	float gy[9] = float[](
		-1, -1, -1,
		 0,  0,  0,
		 1,  1,  1
	);

	float kernel[9] = float[] (
		0, 0, 0,
		0, 1, 0,
		0, 0, 0
	);

	/*vec3 sampleGrayTexture[9];
	vec3 sampleColorTexture[9];
	for (int i = 0; i < 9; i++) {
		vec3 rgb = vec3(texture(textureSampler, ftextureUV.st + offsets[i]));

		float y = 0.2126 * rbgToLin(rgb.x) + 0.7152 * rbgToLin(rgb.y) + 0.0722 * rbgToLin(rgb.z);
		
		//sampleGrayTexture[i] = vec3(y);
		sampleColorTexture[i] = rgb;
	}

	vec3 color = vec3(0.0);
	//vec3 sx = vec3(0.0);
	//vec3 sy = vec3(0.0);
	for (int i = 0; i < 9; i++) {
		color += sampleColorTexture[i] * kernel[i];
		//sx += sampleGrayTexture[i] * gx[i];
		//sy += sampleGrayTexture[i] * gy[i];
	}
	//vec3 s = vec3(1.0) - sqrt(sx*sx + sy*sy);*/

	//outColor = vec4(color, 1.0);
	outColor = texture(textureSampler, ftextureUV.st);
}