[common]

#version 330 core

//------------------------------------------------------------------------------//

[vertex]

layout (location = 0) in vec3 vposition;

out vec3 ftextureUV;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform vec3 uLightDirection;

const float PI = 3.141592654;
const vec4 Esun = vec4(1.0, 1.0, 1.0, 12.0);
const float g = 5.2;

float density(float h) {
	float H = 8500;
	return exp(-h / H);
}

vec3 Bm(float h) {
	vec3 cBm = vec3(4e-6, 6e-6, 2.4e-6);
	return cBm;
}

vec3 Br(float h) {
	vec3 cBr = vec3(0.00000519673, 0.0000121427, 0.0000296453);
	return density(h) * cBr;
}

vec3 rayleigh(float h, float cosphi) {
	float phase = 1.0 + cosphi * cosphi;
	return 3.0 / 16.0 / PI * Br(h) * phase;
}

vec3 mie(float h, float cosphi) {
	float phase = pow(1.0 - g, 2.0) / pow(1.0 + g * g - 2.0 * g * cosphi, 1.5);
	return 1.0 / 4.0 / PI * Bm(h) * phase;
}

vec3 Fex(float h, float s) {
	return exp(-(Br(h) + Bm(h)) * s);
}

vec3 scatter(float h, float s, float cosphi) {
	return (rayleigh(h, cosphi) + mie(h, cosphi)) / (Br(h) + Bm(h)) * Esun.w * (1.0 - Fex(h, s));
}

void main() {
	vec4 position = uProjectionMatrix * vec4((uViewMatrix * vec4(vposition, 0.0)).xyz, 1.0);
	vec3 light = uLightDirection;
	vec3 normal = vposition;

	/*float distance = (1.05 - pow(normal.y, 0.2)) * 150000;
	float cosphi = dot(normal, light) / length(light) / length(normal);
	float h = position.y;
	scattering = scatter(h, distance, cosphi);
	extinction = Fex(h, distance);*/

	ftextureUV = vposition;
	gl_Position = position;
}

//------------------------------------------------------------------------------//

[fragment]

out vec4 outColor;

in vec3 ftextureUV;

uniform samplerCube uSkyboxTexture;

void main() {    
	vec4 skybox = texture(uSkyboxTexture, ftextureUV);
	outColor = skybox;
}