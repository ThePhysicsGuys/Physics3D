#shader vertex
#version 330 core

layout(location = 0) in vec4 positions;

out vec2 fUV;

void main() {
	gl_Position = vec4(positions.xy, 0.0, 1.0);
	fUV = positions.zw;
}

////////////////////////////////////////////////////////////////////////////

#shader fragment
#version 330 core

out vec4 outColor;

in vec2 fUV;

uniform sampler2D image;

uniform bool horizontal;
uniform float weight[5] = { 0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f };

void main() {
	vec2 offset = 4.0 / textureSize(image, 0);
	vec3 result = texture(image, fUV).rgb * weight[0];

	if (horizontal) {
		for (int i = 1; i < 5; ++i) {
			result += texture(image, fUV + vec2(offset.x * i, 0.0)).rgb * weight[i];
			result += texture(image, fUV - vec2(offset.x * i, 0.0)).rgb * weight[i];
		}
	} else {
		for (int i = 1; i < 5; ++i) {
			result += texture(image, fUV + vec2(0.0, offset.y * i)).rgb * weight[i];
			result += texture(image, fUV - vec2(0.0, offset.y * i)).rgb * weight[i];
		}
	}
	outColor = vec4(result, 1.0);
}