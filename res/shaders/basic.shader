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

// #define DEBUG 
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

uniform mat4 viewMatrix;
uniform vec3 viewPosition;
uniform vec3 color;

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

struct Material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float reflectance;
};

// Uniforms
Material material = Material(
	vec4(0.3f, 0.4f, 0.2f, 1.0f),
	vec4(0.3f, 0.2f, 0.6f, 1.0f),
	vec4(0.2f, 0.1f, 0.9f, 1.0f),
	0.5f
);

int lightCount = 3;
Attenuation att = Attenuation(0.0f, 0.0f, 1.0f);
Light lights[3] = Light[](
	Light(vec3(1, 0, 0), vec3(5, 5, 5), 5, att),
	Light(vec3(0, 1, 0), vec3(2, -5, 6), 5, att),
	Light(vec3(0, 0, 1), vec3(-8, 5, -5), 5, att)
);

vec4 calcLightColor(Light light) {
	vec4 diffuseColor = vec4(1.0, 0.0, 0.0, 1.0);
	vec4 specularColor = vec4(0.0, 1.0, 0.0, 1.0);

	// Diffuse light
	vec3 lightDirection = (viewMatrix * vec4(light.position, 1.0)).xyz - fposition;
	vec3 toLightSource = normalize(lightDirection);
	float diffuseFactor = max(dot(fnormal, toLightSource), 0.0);
	diffuseColor = material.diffuse * vec4(light.color, 1.0) * light.intensity * diffuseFactor;

	// Specular light
	float specularPower = 20.0f;
	vec3 cameraDirection = normalize(-fposition);
	vec3 fromLightSource = -toLightSource;
	vec3 reflectedLight = normalize(reflect(fromLightSource, fnormal));
	float specularFactor = max(dot(cameraDirection, reflectedLight), 0.0);
	specularFactor = pow(specularFactor, specularPower);
	specularColor = material.specular * specularFactor * material.reflectance * vec4(light.color, 1.0);

	// Attenuation
	float distance = length(lightDirection);
	float attenuationInverse = light.attenuation.constant + light.attenuation.linear * distance + light.attenuation.exponent * distance * distance;
	vec4 speculardiffuse = (diffuseColor + specularColor) / attenuationInverse;
	return speculardiffuse;
}

void main() {
	vec4 lightColors = vec4(0);
	for (int i = 0; i < lightCount; i++)
		lightColors += calcLightColor(lights[i]);
	outColor = vec4(color, 1) + 30 * lightColors;
}