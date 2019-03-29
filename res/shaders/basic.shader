#shader vertex // vertex Shader
#version 330 core 

layout(location = 0) in vec3 vposition; 

out vec3 gposition;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

void main() { 
	gl_Position = viewMatrix * modelMatrix * vec4(vposition, 1.0f);
	gposition = gl_Position.xyz;
}

///////////////////////////////////////////////////////////////////////////

#shader geometry // geometry shader
#version 330 

layout(triangles) in;

//#define DEBUG 
#ifndef DEBUG
layout(triangle_strip, max_vertices = 3) out;
#else
layout(line_strip, max_vertices = 9) out;
#endif

in vec3 gposition[];

uniform vec3 viewPosition;
uniform mat4 projectionMatrix;

out vec3 fposition;
out vec3 fnormal;
out vec3 fcenter;

vec3 center() {
	return vec3(gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3;
}

vec3 normal() {
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 norm = normalize(cross(a, b));
	return -norm;
}

void main() {
	fnormal = normal();
	fcenter = center();

	mat4 transform = projectionMatrix;

#ifdef DEBUG
	float arrowLength = 0.05;
	float arrowWidth = 0.02;
	vec4 arrowTop = vec4(fcenter + 0.3 * fnormal, 1);
	vec3 norm = normalize(cross(arrowTop.xyz - viewPosition, fnormal));
	vec4 arrowLeft = arrowTop - vec4(arrowLength * fnormal - arrowWidth * norm, 0);
	vec4 arrowRight = arrowTop - vec4(arrowLength * fnormal + arrowWidth * norm, 0);
	vec4 arrowBase = arrowTop - arrowLength * vec4(fnormal, 0);

	gl_Position = transform * vec4(fcenter, 1); EmitVertex();
	gl_Position = transform * arrowBase; EmitVertex();
	gl_Position = transform * arrowLeft; EmitVertex();
	gl_Position = transform * arrowTop; EmitVertex();
	gl_Position = transform * arrowRight; EmitVertex();
	gl_Position = transform * arrowBase; EmitVertex();
	EndPrimitive();
#endif

	fposition = gposition[0];
	gl_Position = transform * gl_in[0].gl_Position; EmitVertex();
	fposition = gposition[1];
	gl_Position = transform * gl_in[1].gl_Position; EmitVertex();
	fposition = gposition[2];
	gl_Position = transform * gl_in[2].gl_Position; EmitVertex();
	EndPrimitive();
}

//////////////////////////////////////////////////////////////////////////

#shader fragment
#version 330

out vec4 outColor;

in vec3 fposition;
in vec3 fnormal;
in vec3 fcenter;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float reflectance;
};

struct Attenuation {
	float constant;
	float linear;
	float exponent;
};

struct Light {
	vec3 color;
	vec3 position;
	float intensity;
	Attenuation attenuation;
};

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 viewPosition;
uniform vec3 color;
uniform Material material;

const int lightCount = 4;
Attenuation att = Attenuation(0.0f, 0.0f, 1.0f);
Light lights[lightCount] = Light[](
	Light(vec3(1, 0, 0), vec3(5, 0, 0), 4, att),
	Light(vec3(0, 1, 0), vec3(0, 5, 0), 4, att),
	Light(vec3(0, 0, 1), vec3(0, 0, 5), 4, att),
	Light(vec3(1, 1, 1), vec3(0, 0, 0), 1, att)
);

vec3 calcLightColor(Light light) {
	material;
	viewPosition;

	// Ambient
	float ambientStrength = 0.5;
	vec3 ambient = ambientStrength * light.color;
	
	// Diffuse light
	vec3 lightDirection = (viewMatrix * vec4(light.position, 1.0)).xyz - fposition;
	vec3 toLightSource = normalize(lightDirection);
	float diffuseFactor = max(dot(fnormal, toLightSource), 0.0);
	vec3 diffuseColor = /*material.diffuse*/ diffuseFactor * light.color;

	// Directional light
	vec3 directionalLight = normalize(vec3(1, 1, 0));
	float directionalFactor = 0.4 * max(dot((transpose(viewMatrix) * vec4(fnormal,1)).xyz, directionalLight), 0.0);
	vec3 directional = directionalFactor * light.color;

	// Specular light
	float specularPower = 10.0f;
	vec3 cameraDirection = normalize(-fposition);
	vec3 fromLightSource = -toLightSource;
	vec3 reflectedLight = normalize(reflect(fromLightSource, fnormal));
	float specularFactor = max(dot(cameraDirection, reflectedLight), 0.0);
	specularFactor = pow(specularFactor, specularPower);
	vec3 specularColor = /*material.specular * material.reflectance **/ specularFactor * light.color;

	// Attenuation
	float distance = length(lightDirection) / light.intensity;
	float attenuationInverse = light.attenuation.constant + light.attenuation.linear * distance + light.attenuation.exponent * distance * distance;
	vec3 specularDiffuse = (diffuseColor + specularColor) / attenuationInverse;

	return (ambient + directional + specularDiffuse) * color;
}

void main() {
	vec3 lightColors = vec3(0);
	for (int i = 0; i < lightCount; i++) 
		lightColors += calcLightColor(lights[i]);
	
	outColor = vec4(lightColors, 1);
}

//int lightCount = 3;
//Attenuation att = Attenuation(0.0f, 0.0f, 1.0f);
//Light lights[3] = Light[](
//	Light(vec3(1, 0, 0), vec3(5, 5, 5), 5, att),
//	Light(vec3(0, 1, 0), vec3(2, -5, 6), 5, att),
//	Light(vec3(0, 0, 1), vec3(-8, 5, -5), 5, att)
//	);
//
//vec3 calcLightColor(Light light) {
//
//	// Diffuse light
//	vec3 lightDirection = (viewMatrix * vec4(light.position, 1.0)).xyz - fposition;
//	vec3 toLightSource = normalize(lightDirection);
//	float diffuseFactor = max(dot(fnormal, toLightSource), 0.0);
//	vec3 diffuseColor = /*material.diffuse **/ light.color * light.intensity * diffuseFactor;
//
//	// Specular light
//	float specularPower = 20.0f;
//	vec3 cameraDirection = normalize(-fposition);
//	vec3 fromLightSource = -toLightSource;
//	vec3 reflectedLight = normalize(reflect(fromLightSource, fnormal));
//	float specularFactor = max(dot(cameraDirection, reflectedLight), 0.0);
//	specularFactor = pow(specularFactor, specularPower);
//	vec3 specularColor = /*material.specular * */specularFactor /** material.reflectance */* light.color;
//
//	// Attenuation
//	float distance = length(lightDirection);
//	float attenuationInverse = light.attenuation.constant + light.attenuation.linear * distance + light.attenuation.exponent * distance * distance;
//	vec3 speculardiffuse = (diffuseColor + specularColor) / attenuationInverse;
//	return speculardiffuse;
//}
//
//void main() {
//	vec3 lightColors = vec3(0);
//	for (int i = 0; i < lightCount; i++)
//		lightColors += calcLightColor(lights[i]);
//	outColor = vec4(color + 30 * lightColors, 1);
//}