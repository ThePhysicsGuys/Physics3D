[common]

#version 330 core

///////////////////////////////////////////////////////////////////////////////////////

[vertex]

layout(location = 0) in vec3 vposition;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 viewPosition;

uniform float skyboxSize = 550.0;
uniform float scroll = 0.0;
uniform float time = 0.0;
uniform float segmentCount = 25.0;

out vec3 fposition;
out vec2 nightUV;

const float tiling = 1;
const float scrollFactor = 0.5;

void main() {
	fposition = vposition * skyboxSize;
	//fposition.z -= skyboxSize * 0.95;

	gl_Position = projectionMatrix * vec4((viewMatrix * vec4(fposition, 0.0)).xyz, 1.0);

	float x = floor(gl_VertexID * 0.5);
	float y = vposition.y * 0.5 + 0.5;
	nightUV = vec2(x, y);
	nightUV.x -= scroll * tiling * scrollFactor;
	nightUV -= time;
}

///////////////////////////////////////////////////////////////////////////////////////

[fragment]

out vec4 outColor;
in vec3 fposition;
in vec2 nightUV;

const float lowerLimit = -20.0;
const float upperLimit = 120.0;

uniform vec3 horizonColor;
uniform vec3 skyColor;

uniform float starBrightness;
uniform sampler2D nightTexture;

void main() {
	float factor = clamp((fposition.y - lowerLimit) / (upperLimit - lowerLimit), 0.0, 1.0);
	outColor = vec4(mix(horizonColor, skyColor, factor), 1);

	//if (starBrightness > 0.001) {
		vec4 starColor = texture(nightTexture, nightUV);
		outColor.rgb = mix(outColor.rgb, starColor.rgb, starColor.a * 1 * factor);
	//}

	// grid
	float d1 = dot(vec3(0, 1, 0), normalize(fposition));
	float d2 = dot(vec3(0, 0, 1), normalize(fposition));
	float d3 = dot(vec3(1, 0, 0), normalize(fposition));
	if (mod(d1, 0.2) < 0.001 || mod(d2, 0.2) < 0.001 || mod(d3, 0.2) < 0.001)
		outColor = vec4(1);
	if (abs(d1) < 0.003 || abs(d2) < 0.003 || abs(d3) < 0.003)
		outColor = vec4(d3, d1, d2, 1);
}