[common]

#version 330 core

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

//------------------------------------------------------------------------------//

[vertex]

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec3 vBitangent;
layout(location = 5) in mat4 vModelMatrix;
layout(location = 9) in vec4 vAlbedo;
layout(location = 10) in vec3 vMRAo;

smooth out vec3 fPosition;
smooth out vec2 fUV;
smooth out vec3 fNormal;
 
flat out vec4 fAlbedo;
flat out float fMetallic;
flat out float fRoughness;
flat out float fAmbientOcclusion;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {
	fAlbedo = vAlbedo;
	fMetallic = vMRAo.x;
	fRoughness = vMRAo.y;
	fAmbientOcclusion = vMRAo.z;

	fUV = vec2(vUV.x, 1-vUV.y);
	fPosition = applyT3(vModelMatrix, vPosition);
	fNormal = apply3(vModelMatrix, vNormal);

	gl_Position = applyT(projectionMatrix * viewMatrix, fPosition);
}

//------------------------------------------------------------------------------//

[fragment]

// Out
out vec4 outColor;

// In
smooth in vec2 fUV;
smooth in vec3 fPosition;
smooth in vec3 fNormal; 

flat in vec4 fAlbedo;
flat in float fRoughness;
flat in float fMetallic;
flat in float fAmbientOcclusion;

// General
vec3 N;
vec3 V;
vec4 albedo;
float roughness;
float metallic;
float ambientOcclusion;

// Structs
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
uniform vec3 viewPosition;

// Light
#define maxLights 10
uniform int lightCount;
uniform Light lights[maxLights];

// Textures
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D ambientOcclusionMap;
uniform int textured;

// Environment
uniform vec3 sunDirection = vec3(1, 1, 1);
uniform vec3 sunColor = vec3(1, 1, 1);
uniform float exposure = 1.0;
uniform float gamma = 1.0;
uniform float hdr = 1.0;

// Constants
const float PI = 3.14159265359;

float ggxTrowbridgeReitz(vec3 N, vec3 H, float roughness) {
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float numerator = alpha2;
	float denominator = (NdotH2 * (alpha2 - 1.0) + 1.0);
	denominator = max(PI * denominator * denominator, 0.001);

	return numerator / denominator;
}

float ggxSchlick(float NdotV, float roughness) {
	float r = roughness + 1.0;
	float k = (r * r) / 8.0;

	float numerator = NdotV;
	float denominator = NdotV * (1.0 - k) + k;

	return numerator / denominator;
}

float smith(vec3 N, vec3 V, vec3 L, float roughness) {
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = ggxSchlick(NdotV, roughness);
	float ggx1 = ggxSchlick(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
	// F0: surface reflection at zero incidence
	return F0 + (1.0 - F0) * pow(1.0 - min(cosTheta, 1.0), 5.0);
}

vec3 calcDirectionalLight() {
	vec3 directionalLight = normalize(sunDirection);
	float directionalFactor = 0.4 * max(dot(N, directionalLight), 0.0);
	vec3 directional = directionalFactor * sunColor;
	return directional;
}

vec3 calcLightColor(Light light) {
	// General light variables
	vec3 L = normalize(light.position - fPosition);
	vec3 H = normalize(V + L);
	float distance = length(light.position - fPosition);
	//float scaledDistance = distance / light.intensity;
	//float attenuation = 1.0 / (light.attenuation.constant + light.attenuation.linear * scaledDistance + light.attenuation.exponent * scaledDistance * scaledDistance);
	float attenuation = 1.0 / (distance * distance);
	vec3 radiance = light.color * attenuation;

	// Fresnel
	vec3 F0_NM = vec3(0.04); // Non metallic F0
	vec3 F0 = mix(F0_NM, albedo.rgb, metallic);
	float cosTheta = max(dot(H, V), 0.0);
	vec3 F = fresnelSchlick(cosTheta, F0);

	// DFG
	float D = ggxTrowbridgeReitz(N, H, roughness);
	float G = smith(N, V, L, roughness);
	vec3 DFG = D * F * G;

	// Cook Torrance
	vec3 numerator = DFG;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
	vec3 specular = numerator / max(denominator, 0.001);

	// Light contribution constants
	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;

	float NdotL = max(dot(N, L), 0.0);
	vec3 Lo = (kD * albedo.rgb / PI + specular) * radiance * NdotL;
	
	return Lo;
}

vec3 getNormalFromMap() {
	vec3 tangentNormal = texture(normalMap, fUV).xyz * 2.0 - 1.0;

	vec3 Q1 = dFdx(fPosition);
	vec3 Q2 = dFdy(fPosition);
	vec2 st1 = dFdx(fUV);
	vec2 st2 = dFdy(fUV);

	vec3 N = normalize(fNormal);
	vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
}

void main() {
	if (textured == 1) {
		albedo = texture(albedoMap, fUV) * texture(albedoMap, fUV);
		N = getNormalFromMap();
		roughness = 1-texture(roughnessMap, fUV).r;
		metallic = texture(metallicMap, fUV).r;
		ambientOcclusion = texture(ambientOcclusionMap, fUV).r;
	} else {
		albedo = fAlbedo;
		roughness = fRoughness;
		metallic = fMetallic;
		ambientOcclusion = fAmbientOcclusion;
	}

	V = normalize(viewPosition - fPosition);

	// Light calculations
	vec3 Lo = vec3(0);
	for (int i = 0; i < min(maxLights, lightCount); i++) {
		if (lights[i].intensity > 0) {
			Lo += calcLightColor(lights[i]);
		}
	}

	// Ambient
	vec3 ambient = vec3(0.03) * albedo.rgb * ambientOcclusion;

	// Combine ambient and lighting
	vec3 color = ambient + Lo;

	// HDR 
	color = hdr * (vec3(1.0) - exp(-color * exposure)) + (1.0 - hdr) * color;

	// Gamma
	color = pow(color, vec3(1.0 / gamma));

	// Outcolor
	outColor = vec4(color, albedo.a);

	//outColor = rgba(N);
	//outColor = texture(normalMap, fUV);
	//outColor = texture(textureMap, fUV);
	//outColor = vec4(fUV, 0, 1);
}

