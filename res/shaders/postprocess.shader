[common]

#version 330 core

//------------------------------------------------------------------------------//

[vertex]

layout(location = 0) in vec4 positions;

out vec2 fUV;

void main() {
	gl_Position = vec4(positions.xy, 0.0, 1.0);
	fUV = positions.zw;
}

//------------------------------------------------------------------------------//

[fragment]

in vec2 fUV;
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

vec4 kernelEffect() {
	vec2 offsets[9] = vec2[](
		vec2(-offset, offset),
		vec2(0.0f, offset),
		vec2(offset, offset),
		vec2(-offset, 0.0f),
		vec2(0.0f, 0.0f),
		vec2(offset, 0.0f),
		vec2(-offset, -offset),
		vec2(0.0f, -offset),
		vec2(offset, -offset)
		);

	float gx[9] = float[](
		-1, 0, 1,
		-1, 0, 1,
		-1, 0, 1
		);

	float gy[9] = float[](
		-1, -1, -1,
		0, 0, 0,
		1, 1, 1
		);

	float kernel[9] = float[](
		0, 0, 0,
		0, 1, 0,
		0, 0, 0
		);

	vec3 sampleGrayTexture[9];
	vec3 sampleColorTexture[9];
	vec3 color = vec3(0.0);
	vec3 sx = vec3(0.0);
	vec3 sy = vec3(0.0);

	for (int i = 0; i < 9; i++) {
		vec3 rgb = vec3(texture(textureSampler, fUV.st + offsets[i]));

		sampleColorTexture[i] = rgb;
		color += rgb * kernel[i];
		
		//float y = 0.2126 * rbgToLin(rgb.x) + 0.7152 * rbgToLin(rgb.y) + 0.0722 * rbgToLin(rgb.z);
		//sampleGrayTexture[i] = vec3(y);
		//sx += sampleGrayTexture[i] * gx[i];
		//sy += sampleGrayTexture[i] * gy[i];
	}

	//vec3 s = vec3(1.0) - sqrt(sx*sx + sy*sy);

	return vec4(color, 1.0);
}

vec4 defaultEffect() {
	return texture(textureSampler, fUV.st);
}

vec4 pixelEdgeEffect() {
	vec4 color = vec4(fwidth(defaultEffect().x * 64.0));
	return 1.0 - floor(color);
}

vec4 knitEffect() {
	vec2 resolution = vec2(1920, 1080);
	vec2 tileSize = vec2(16.0, 16.0);
	float threads = 10.0;

	vec2 posInTile = mod(gl_FragCoord.xy, tileSize);
	vec2 tileNum = floor(gl_FragCoord.xy / tileSize);

	vec2 nrmPosInTile = posInTile / tileSize;
	tileNum.y += floor(abs(nrmPosInTile.x - 0.5) + nrmPosInTile.y);

	vec2 texCoord = tileNum * tileSize / resolution;
	//texCoord.y = 1.0 - texCoord.y;

	vec3 color = texture(textureSampler, texCoord).rgb;

	color *= fract((nrmPosInTile.y + abs(nrmPosInTile.x - 0.5)) * floor(threads));

	return vec4(color, 1.0);
}

void main() {
	outColor = defaultEffect();
}