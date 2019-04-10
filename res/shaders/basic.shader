#shader vertex // vertex Shader
#version 330 core

layout(location = 0) in vec3 vposition;

out vec2 gtextureUV;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

void main() {
	gtextureUV = vposition.xy;
	gl_Position = modelMatrix * vec4(vposition, 1.0f);
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

uniform vec3 viewPosition;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

in vec2 gtextureUV[];

out vec2 ftextureUV;
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

	mat4 transform = projectionMatrix * viewMatrix;

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

	fposition = gl_in[0].gl_Position.xyz;
	ftextureUV = vec2(1, 1);//gtextureUV[0];
	gl_Position = transform * gl_in[0].gl_Position; EmitVertex();

	fposition = gl_in[1].gl_Position.xyz;
	ftextureUV = vec2(0, 1);
	gl_Position = transform * gl_in[1].gl_Position; EmitVertex();

	fposition = gl_in[2].gl_Position.xyz;
	ftextureUV = vec2(0, 0);
	gl_Position = transform * gl_in[2].gl_Position; EmitVertex();
	EndPrimitive();
}

//////////////////////////////////////////////////////////////////////////

#shader fragment
#version 330

out vec4 outColor;

in vec2 ftextureUV;
in vec3 fposition;
in vec3 fnormal;
in vec3 fcenter;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float reflectance;
	bool textured;
};

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

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform Material material;
uniform sampler2D textureSampler;

const int maxLights = 4;
uniform Light lights[maxLights];

vec3 calcLightColor(Light light) {

	// Ambient
	float ambientStrength = 0.5;
	vec3 ambient = ambientStrength * light.color;

	// Diffuse light
	vec3 lightDirection = light.position - fposition;
	vec3 toLightSource = normalize(lightDirection);
	float diffuseFactor = max(dot(fnormal, toLightSource), 0.0);
	vec3 diffuse = material.diffuse * diffuseFactor * light.color;

	// Directional light
	vec3 directionalLight = normalize(vec3(1, 1, 0));
	float directionalFactor = 0.4 * max(dot(fnormal, directionalLight), 0.0);
	vec3 directional = directionalFactor * light.color;

	// Specular light
	float specularPower = 10.0f;
	vec3 cameraDirection = normalize(-(viewMatrix * vec4(fposition, 1)).xyz);
	vec3 fromLightSource = -toLightSource;
	vec3 reflectedLight = normalize(reflect(fromLightSource, fnormal));
	float specularFactor = max(dot(cameraDirection, (viewMatrix * vec4(reflectedLight, 0)).xyz), 0.0);
	specularFactor = pow(specularFactor, specularPower);
	vec3 specular = material.specular * material.reflectance * specularFactor * light.color;

	// Attenuation
	float distance = length(lightDirection) / light.intensity;
	float attenuationInverse = light.attenuation.constant + light.attenuation.linear * distance + light.attenuation.exponent * distance * distance;
	vec3 specularDiffuse = (diffuse + specular) / attenuationInverse;

	return ambient + directional + specularDiffuse;
}

void main() {
	vec3 lightColors = vec3(0);
	int count = 0;
	for (int i = 0; i < maxLights; i++) {
		if (lights[i].intensity > 0) {
			lightColors += calcLightColor(lights[i]);
			count++;
		}
	}
	if (material.textured) {
		outColor = vec4(lightColors / count, 1) * texture(textureSampler, ftextureUV);
	} else {
		outColor = vec4(lightColors / count * material.ambient, 1);
	}
}

//#shader vertex // vertex Shader
//#version 330 core
//
//layout(location = 0) in vec3 vposition;
//
//out vec3 gposition;
//
//uniform mat4 modelMatrix;
//uniform mat4 viewMatrix;
//
//void main() {
//	gl_Position = viewMatrix * modelMatrix * vec4(vposition, 1.0f);
//	gposition = gl_Position.xyz;
//}
//
/////////////////////////////////////////////////////////////////////////////
//
//#shader geometry // geometry shader
//#version 330
//
//layout(triangles) in;
//
////#define DEBUG 
//#ifndef DEBUG
//layout(triangle_strip, max_vertices = 3) out;
//#else
//layout(line_strip, max_vertices = 9) out;
//#endif
//
//in vec3 gposition[];
//
//uniform vec3 viewPosition;
//uniform mat4 projectionMatrix;
//
//out vec3 fposition;
//out vec3 fnormal;
//out vec3 fcenter;
//
//vec3 center() {
//	return vec3(gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3;
//}
//
//vec3 normal() {
//	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
//	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
//	vec3 norm = normalize(cross(a, b));
//	return -norm;
//}
//
//void main() {
//	fnormal = normal();
//	fcenter = center();
//
//	mat4 transform = projectionMatrix;
//
//	#ifdef DEBUG
//	float arrowLength = 0.05;
//	float arrowWidth = 0.02;
//	vec4 arrowTop = vec4(fcenter + 0.3 * fnormal, 1);
//	vec3 norm = normalize(cross(arrowTop.xyz - viewPosition, fnormal));
//	vec4 arrowLeft = arrowTop - vec4(arrowLength * fnormal - arrowWidth * norm, 0);
//	vec4 arrowRight = arrowTop - vec4(arrowLength * fnormal + arrowWidth * norm, 0);
//	vec4 arrowBase = arrowTop - arrowLength * vec4(fnormal, 0);
//
//	gl_Position = transform * vec4(fcenter, 1); EmitVertex();
//	gl_Position = transform * arrowBase; EmitVertex();
//	gl_Position = transform * arrowLeft; EmitVertex();
//	gl_Position = transform * arrowTop; EmitVertex();
//	gl_Position = transform * arrowRight; EmitVertex();
//	gl_Position = transform * arrowBase; EmitVertex();
//	EndPrimitive();
//	#endif
//
//	fposition = gposition[0];
//	gl_Position = transform * gl_in[0].gl_Position; EmitVertex();
//	fposition = gposition[1];
//	gl_Position = transform * gl_in[1].gl_Position; EmitVertex();
//	fposition = gposition[2];
//	gl_Position = transform * gl_in[2].gl_Position; EmitVertex();
//	EndPrimitive();
//}
//
////////////////////////////////////////////////////////////////////////////
//
//#shader fragment
//#version 330
//
//out vec4 outColor;
//
//in vec3 fposition;
//in vec3 fnormal;
//in vec3 fcenter;
//
//struct Material {
//	vec3 ambient;
//	vec3 diffuse;
//	vec3 specular;
//	float reflectance;
//};
//
//struct Attenuation {
//	float constant;
//	float linear;
//	float exponent;
//};
//
//struct Light {
//	vec3 position;
//	vec3 color;
//	float intensity;
//	Attenuation attenuation;
//};
//
//uniform mat4 viewMatrix;
//uniform mat4 projectionMatrix;
//uniform vec3 viewPosition;
//uniform Material material;
//
//const int maxLights = 4;
//uniform Light lights[maxLights];
//
//vec3 calcLightColor(Light light) {
//	material;
//	viewPosition;
//
//	// Ambient
//	float ambientStrength = 0.5;
//	vec3 ambient = ambientStrength * light.color;
//
//	// Diffuse light
//	vec3 lightDirection = (viewMatrix * vec4(light.position, 1.0)).xyz - fposition;
//	vec3 toLightSource = normalize(lightDirection);
//	float diffuseFactor = max(dot(fnormal, toLightSource), 0.0);
//	vec3 diffuse = material.diffuse * diffuseFactor * light.color;
//
//	// Directional light
//	vec3 directionalLight = normalize(vec3(1, 1, 0));
//	float directionalFactor = 0.4 * max(dot((transpose(viewMatrix) * vec4(fnormal, 1)).xyz, directionalLight), 0.0);
//	vec3 directional = directionalFactor * light.color;
//
//	// Specular light
//	float specularPower = 10.0f;
//	vec3 cameraDirection = normalize(-fposition);
//	vec3 fromLightSource = -toLightSource;
//	vec3 reflectedLight = normalize(reflect(fromLightSource, fnormal));
//	float specularFactor = max(dot(cameraDirection, reflectedLight), 0.0);
//	specularFactor = pow(specularFactor, specularPower);
//	vec3 specular = material.specular * material.reflectance * specularFactor * light.color;
//
//	// Attenuation
//	float distance = length(lightDirection) / light.intensity;
//	float attenuationInverse = light.attenuation.constant + light.attenuation.linear * distance + light.attenuation.exponent * distance * distance;
//	vec3 specularDiffuse = (diffuse + specular) / attenuationInverse;
//
//	return (ambient + directional + specularDiffuse) * material.ambient;
//}
//
//void main() {
//	vec3 lightColors = vec3(0);
//	for (int i = 0; i < maxLights; i++)
//		if (lights[i].intensity > 0)
//			lightColors += calcLightColor(lights[i]);
//
//	outColor = vec4(lightColors, 1);
//}

