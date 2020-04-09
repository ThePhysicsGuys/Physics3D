[common]
#version 450

vec4 apply(mat4 matrix, vec3 vector) {
	return matrix * vec4(vector, 0.0);
}

vec3 apply3(mat4 matrix, vec3 vector) {
	return mat3(matrix) * vector;
}

vec4 applyT(mat4 matrix, vec3 vector) {
	return matrix * vec4(vector, 1.0);
}

vec3 applyT3(mat4 matrix, vec3 vector) {
	return (matrix * vec4(vector, 1.0)).xyz;
}

vec4 applyN(mat4 matrix, vec3 vector) {
	return normalize(matrix * vec4(vector, 0.0));
}

vec3 apply3N(mat4 matrix, vec3 vector) {
	return normalize(mat3(matrix) * vector);
}

vec4 applyTN(mat4 matrix, vec3 vector) {
	return normalize(matrix * vec4(vector, 1.0));
}

vec3 applyT3N(mat4 matrix, vec3 vector) {
	return normalize((matrix * vec4(vector, 1.0)).xyz);
}

vec4 rgba(vec3 color) {
	return vec4(color, 1.0);
}

vec4 rgba(float color) {
	return vec4(color, color, color, 1.0);
}

[vertex]
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec3 vBitangent;
layout(location = 5) in mat4 vModelMatrix;
layout(location = 9) in vec4 vAmbient;
layout(location = 10) in vec3 vDiffuse;
layout(location = 11) in vec3 vSpecular;
layout(location = 12) in float vReflectance;

out vec4 fAmbient;
out vec3 fDiffuse;
out vec3 fSpecular;
out float fReflectance;

out vec3 fPosition;
out vec2 fUV;
out vec3 fNormal;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {
	fAmbient = vAmbient;
	fDiffuse = vDiffuse;
	fSpecular = vSpecular;
	fReflectance = vReflectance;

	fUV = vUV;
	fPosition = applyT3(vModelMatrix, vPosition);
	fNormal = apply3N(vModelMatrix, vNormal);

	gl_Position = applyT(projectionMatrix * viewMatrix, fPosition);
}

[fragment]

out vec4 outColor;

in vec2 fUV;
in vec3 fPosition;
in vec3 fNormal;

in vec4 fAmbient;
in vec3 fDiffuse;
in vec3 fSpecular;
in float fReflectance;

struct Attenuation {
	float constant;
	float linear;
	float exponent;
};

struct Light {
	vec3 position;
	vec3 color;
	float intensity;
	Attenuation attenuation;
};

// Transform
uniform mat4 viewMatrix;
uniform vec3 viewPosition;

// Appearance
#define maxLights 10
uniform int lightCount;
uniform Light lights[maxLights];

uniform sampler2D textureMap;
uniform sampler2D normalMap;
uniform int textured;

// Environment
uniform vec3 sunDirection = vec3(1, 1, 1);
uniform vec3 sunColor = vec3(1, 1, 1);
uniform float exposure = 1.0;
uniform float gamma = 1.0;
uniform int hdr = 1;

vec3 calcDirectionalLight() {
	vec3 directionalLight = normalize(sunDirection);
	float directionalFactor = 0.4 * max(dot(fNormal, directionalLight), 0.0);
	vec3 directional = directionalFactor * sunColor;
	return directional;
}

vec3 calcLightColor(Light light) {

	// Ambient
	float ambientStrength = 0.5;
	vec3 ambient = ambientStrength * light.color;

	// Diffuse light
	vec3 lightDirection = fPosition - light.position;
	vec3 toLightSource = -normalize(lightDirection);
	float diffuseFactor = max(dot(fNormal, toLightSource), 0.0);
	vec3 diffuse = fDiffuse * diffuseFactor * light.color;

	// Specular light  
	float specularPower = 12.0f;
	vec3 viewDirection = normalize(fPosition - viewPosition);
	vec3 fromLightSource = -toLightSource;
	vec3 reflectedLight = normalize(reflect(fromLightSource, fNormal));
	float specularFactor = max(dot(-viewDirection, reflectedLight), 0.0);
	specularFactor = pow(specularFactor, specularPower);
	vec3 specular = fSpecular * fReflectance * specularFactor * light.color;

	// Attenuation
	float distance = length(lightDirection) / light.intensity;
	float attenuationInverse = light.attenuation.constant + light.attenuation.linear * distance + light.attenuation.exponent * distance * distance;
	vec3 specularDiffuse = (diffuse + specular) / attenuationInverse;

	return ambient + specularDiffuse;
}

void main() {
	// Light calculations
	vec3 lightColors = vec3(0);
	int count = 0;
	for (int i = 0; i < min(maxLights, lightCount); i++) {
		if (lights[i].intensity > 0) {
			lightColors += calcLightColor(lights[i]);
			count++;
		}
	}

	// Take average of colors
	outColor = vec4(lightColors / count * fAmbient.rgb, fAmbient.a);

	// Directional light
	outColor = outColor + vec4(calcDirectionalLight(), 0);

	// Apply texture if present
	outColor *= textured * texture(textureMap, fUV) + (1 - textured) * vec4(1);

	// HDR correction
	outColor = hdr * vec4(vec3(1.0) - exp(-outColor.rgb * exposure), outColor.a) + (1 - hdr) * outColor;

	// Gamma correction
	outColor = vec4(pow(outColor.rgb, vec3(1.0 / gamma)), outColor.a);

	//outColor = rgba(fNormal);
	//outColor = texture(normalMap, fUV);
	//outColor = texture(textureMap, fUV);
	//outColor = vec4(fUV, 0, 1);
}

