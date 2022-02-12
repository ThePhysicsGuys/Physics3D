[properties]
int mode = 0;
int reflectionSamples = 8;

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
layout(location = 11) in uvec2 vTextureFlags;

smooth out vec3 fPosition;
smooth out vec3 fNormal;
smooth out vec2 fUV;
smooth out vec4 fLightSpacePosition;

flat out vec4 fAlbedo;
flat out float fMetalness;
flat out float fRoughness;
flat out float fAmbientOcclusion;
flat out uvec2 fTextureFlags;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 lightMatrix;

void main() {
	fAlbedo = vAlbedo;
	fMetalness = vMRAo.x;
	fRoughness = vMRAo.y;
	fAmbientOcclusion = vMRAo.z;
	fTextureFlags = vTextureFlags;

	fUV = vUV;
	fPosition = applyT3(vModelMatrix, vPosition);
	mat3 normalMatrix = transpose(inverse(mat3(vModelMatrix)));
	fNormal = normalMatrix * vNormal;
	fLightSpacePosition = applyT(lightMatrix, fPosition);

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
smooth in vec4 fLightSpacePosition;

flat in vec4 fAlbedo;
flat in float fRoughness;
flat in float fMetalness;
flat in float fAmbientOcclusion;
flat in uvec2 fTextureFlags;

const ivec2 Flag_Albedo       = ivec2(0, 24);
const ivec2 Flag_Normal       = ivec2(0, 16);
const ivec2 Flag_Metalness    = ivec2(0, 8);
const ivec2 Flag_Roughness    = ivec2(0, 0);
const ivec2 Flag_AO           = ivec2(1, 24);
const ivec2 Flag_Gloss        = ivec2(1, 16);
const ivec2 Flag_Specular     = ivec2(1, 8);
const ivec2 Flag_Displacement = ivec2(1, 0);
						    
const int Mode_Default        = 0;
const int Mode_Position       = 1;
const int Mode_Normal         = 2;
const int Mode_UV             = 3;
const int Mode_LightSpace     = 4;
const int Mode_Albedo         = 5;
const int Mode_Metalness      = 6;
const int Mode_Roughness      = 7;
const int Mode_AO             = 8;
const int Mode_TextureFlags   = 9;

uniform int mode = Mode_Default;

// General
vec3 N;
vec3 V;
vec4 albedo;
float roughness;
float metalness;
float ambientOcclusion;

// Attenuation
struct Attenuation {
	float constant;
	float linear;
	float exponent;
};

// Light
struct Light {
	vec3 position;
	vec3 color;
	float intensity;
	Attenuation attenuation;
};

#define maxLights 10
uniform int lightCount;
uniform Light lights[maxLights];

// Shadow
uniform sampler2D shadowMap;

// Transform
uniform vec3 viewPosition;

// Textures
//uniform sampler2D textures[##MAX_TEXTURE_IMAGE_UNITS];
uniform sampler2D textures[31];
uniform samplerCube skyboxTexture;

// Environment
uniform int reflectionSamples = 10;
uniform vec3 sunDirection = vec3(1, 1, 1);
uniform vec3 sunColor = vec3(1, 1, 1);
uniform float exposure = 0.8;
uniform float gamma = 1.0;
uniform float hdr = 1.0;

// Constants
float PI = 3.14159265359;
float TWOPI = 6.28318531;

float ggxTrowbridgeReitz(vec3 N, vec3 H, float alpha) {
	float alpha2 = alpha * alpha;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float numerator = alpha2;
	float denominator = (NdotH2 * (alpha2 - 1.0) + 1.0);
	denominator = max(PI * denominator * denominator, 0.001);

	return numerator / denominator;
}

float ggxSchlick(float NdotV, float alpha) {
	float t = alpha + 1.0;
	float k = (t * t) / 8.0;

	float numerator = NdotV;
	float denominator = NdotV * (1.0 - k) + k;

	return numerator / denominator;
}

float smith(vec3 N, vec3 V, vec3 L, float k) {
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = ggxSchlick(NdotV, k);
	float ggx1 = ggxSchlick(NdotL, k);

	return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
	return F0 + (1.0 - F0) * pow(1.0 - min(cosTheta, 1.0), 5.0);
}

vec3 calcDirectionalLight() {
	vec3 L = normalize(sunDirection);
	float directionalFactor = 0.4 * max(dot(N, -L), 0.0);
	vec3 directional = directionalFactor * sunColor;
	return directional;
}

float calcShadow() {
	// perform perspective divide
	vec3 projCoords = fLightSpacePosition.xyz / fLightSpacePosition.w;

	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;

	if (projCoords.z > 1.0)
		return 0.0;

	// check whether current frag pos is in shadow
	//float bias = 0.005;
	float bias = max(0.05 * (1.0 - dot(N, -sunDirection)), 0.005);
	float currentDepth = projCoords.z;
	float shadow = 0.0;
	vec2 texelSize = 0.5 / textureSize(shadowMap, 0);
	for (int x = -2; x <= 2; ++x) {
		for (int y = -2; y <= 2; ++y) {
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 25.0;

	return shadow;
}

vec3 calcLightColor(Light light) {
	// General light variables
	vec3 L = normalize(light.position - fPosition);
	vec3 H = normalize(V + L);
	float distance = length(light.position - fPosition);
	float cosTheta = max(dot(H, V), 0.0);

	float scaledDistance = distance/* / light.intensity*/;
	float attenuation = 1.0 / (light.attenuation.constant + light.attenuation.linear * scaledDistance + light.attenuation.exponent * scaledDistance * scaledDistance);
	//float attenuation = 1.0 / (distance * distance);
	vec3 radiance = light.color * attenuation * light.intensity * cosTheta;

	// Fresnel
	vec3 F0_NM = vec3(0.04); // Non metallic F0
	vec3 F0 = mix(F0_NM, albedo.rgb, metalness);
	vec3 F = fresnelSchlick(cosTheta, F0);

	// Cook Torrance
	float NDF = ggxTrowbridgeReitz(N, H, roughness * roughness);
	float G = smith(N, V, L, roughness);
	vec3 numerator = NDF * F * G;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
	vec3 specular = numerator / denominator;

	// Light contribution constants
	vec3 kS = F;
	vec3 kD = (vec3(1.0) - kS) * (1.0 - metalness);

	float NdotL = max(dot(N, L), 0.0);
	vec3 Lo = (kD / PI * albedo.rgb + specular) * radiance * NdotL;

	return Lo;
}

float rand(float n) { return fract(sin(n) * 43758.5453123); }

vec3 skybox() {
	vec3 wo = normalize(viewPosition - fPosition);
	vec3 wi = reflect(-wo, N);

	vec3 w = -wi;
	vec3 u = cross(vec3(0.0, 1.0, 0.0), w);
	vec3 v = cross(w, u);

	vec3 result = texture(skyboxTexture, wi).rgb;
	for (int s = 1; s <= reflectionSamples; s++) {
		float progress = float(s) / float(reflectionSamples);
		float angle = (10.0 + 3.0 * rand(s)) * TWOPI * progress;
		vec3 offset = cos(angle) * u + sin(angle) * v;
		vec3 scaledOffset = (1.0 - metalness) * offset * mix(0.02, 0.35, progress);
		vec3 ws = normalize(wi + scaledOffset);
		result += texture(skyboxTexture, ws).rgb;
	}

	result /= max(float(reflectionSamples), 1.0);
	
	return result;
}

int getTextureMapIndex(ivec2 flag) {
	return int((fTextureFlags[flag.x] >> uint(flag.y)) & uint(0xFF));
}

vec4 getTextureMap(flat int map) {
	return texture(textures[map - 1], fUV);
}

vec4 getAlbedo() {
	int map = getTextureMapIndex(Flag_Albedo);
	
	if (map == 0)
		return fAlbedo;

	return vec4(getTextureMap(map).rgb, 1.0);
}

vec3 getNormal() {
	int map = getTextureMapIndex(Flag_Normal);

	if (map == 0)
		return normalize(fNormal);

	//return getTextureMap(map).rgb;
	vec3 tangentNormal = getTextureMap(map).xyz * 2.0 - 1.0;

	vec3 Q1 = dFdx(fPosition);
	vec3 Q2 = dFdy(fPosition);
	vec2 st1 = dFdx(fUV);
	vec2 st2 = dFdy(fUV);

	vec3 N = normalize(fNormal);
	vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return TBN * tangentNormal;
}

float getMetalness() {
	int map = getTextureMapIndex(Flag_Metalness);

	if (map == 0)
		return fMetalness;

	return fMetalness * getTextureMap(map).r;
}

float getRoughness() {
	int map = getTextureMapIndex(Flag_Roughness);

	if (map == 0)
		return fRoughness;

	return fRoughness * getTextureMap(map).r;
}

float getAmbientOcclusion() {
	int map = getTextureMapIndex(Flag_AO);

	if (map == 0)
		return fAmbientOcclusion;

	return fAmbientOcclusion * getTextureMap(map).r;
}

vec3 hsv2rgb(vec3 c) {
	vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
	return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
	albedo = getAlbedo();
	roughness = getRoughness();
	metalness = getMetalness();
	ambientOcclusion = getAmbientOcclusion();

	N = getNormal();
	V = normalize(viewPosition - fPosition);

	// Light calculations
	vec3 Lo = vec3(0);
	for (int i = 0; i < min(maxLights, lightCount); i++)
		if (lights[i].intensity > 0)
			Lo += calcLightColor(lights[i]);

	// Ambient
	vec3 ambient = albedo.rgb * mix(vec3(ambientOcclusion), skybox(), metalness);

	// Directional
	vec3 Ld = calcDirectionalLight();

	// Shadow
	float shadow = calcShadow();

	// Combine ambient and lighting
	vec3 color = ambient + (1.0 - shadow) * Ld * ambient + Lo;

	// HDR 
	color = hdr * (vec3(1.0) - exp(-color * exposure)) + (1.0 - hdr) * color;

	// Gamma
	color = pow(color, vec3(1.0 / gamma));

	// Outcolor
	if (mode == Mode_Default)
		outColor = vec4(color, albedo.a);
	else if (mode == Mode_Position)
		outColor = vec4(fPosition, 1.0);
	else if (mode == Mode_Normal)
		outColor = vec4(N, 1.0);
	else if (mode == Mode_UV)
		outColor = vec4(fUV, 0.0, 1.0);
	else if (mode == Mode_LightSpace)
		outColor = vec4(fLightSpacePosition);
	else if (mode == Mode_Albedo)
		outColor = vec4(albedo);
	else if (mode == Mode_Metalness)
		outColor = vec4(vec3(metalness), 1.0);
	else if (mode == Mode_Roughness)
		outColor = vec4(vec3(roughness), 1.0);
	else if (mode == Mode_AO)
		outColor = vec4(vec3(ambientOcclusion), 1.0);
	else if (mode == Mode_TextureFlags)
		outColor = vec4(vec3(float(getTextureMapIndex(Flag_Albedo)) / 3.0), 1.0);
	else
		outColor = vec4(hsv2rgb(vec3(mod(gl_PrimitiveID, 50) / 50.0, 1.0, 1.0)), 1.0);

	//outColor = vec4(vec3(fbm(fNormal)), 1.0);
}