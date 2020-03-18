[common]

#version 330 core

//------------------------------------------------------------------------------//

[vertex]

layout(location = 0) in vec3 vposition;
layout(location = 1) in vec3 vnormal;
layout(location = 2) in vec2 vuv;
layout(location = 3) in mat4 vmodelMatrix;
layout(location = 7) in vec4 vambient;
layout(location = 8) in vec3 vdiffuse;
layout(location = 9) in vec3 vspecular;
layout(location = 10) in float vreflectance;

out vec3 gposition;
out vec2 guv;

out vec4 gambient;
out vec3 gdiffuse;
out vec3 gspecular;
out float greflectance;

out VS_OUT {
	vec3 value;
} gnormal;

uniform mat4 viewMatrix;

void main() {
	gposition = vposition;
	guv = vuv;
	gnormal.value = (vmodelMatrix * vec4(vnormal, 0.0)).xyz;

	gambient = vambient;
	gdiffuse = vdiffuse;
	gspecular = vspecular;
	greflectance = vreflectance;

	gl_Position = vmodelMatrix * vec4(vposition, 1.0);
}

//------------------------------------------------------------------------------//

[geometry]

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

in vec3 gposition[];
in vec2 guv[];

in VS_OUT{
	vec3 value;
} gnormal[];

in vec4 gambient[];
in vec3 gdiffuse[];
in vec3 gspecular[];
in float greflectance[];

out vec3 fposition;
out vec3 fnormal;
out vec2 fuv;
out vec3 fcenter;

out vec4 fambient;
out vec3 fdiffuse;
out vec3 fspecular;
out float freflectance;

vec3 center() {
	return vec3(gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3;
}

vec3 normal() {
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	
	vec3 norm = normalize(cross(a, b));
	return -norm;
}

vec2 uv(vec3 p, vec3 n, vec3 u, vec3 v) {
	u = normalize(u);
	v = normalize(v);
	p = p - dot(p, n) * n;
	return vec2(dot(p, u), dot(p, v));
}

void main() {
	fnormal = normal();
	fcenter = center();

	mat4 transform = projectionMatrix * viewMatrix;

	fambient = gambient[0];
	fdiffuse = gdiffuse[0];
	fspecular = gspecular[0];
	freflectance = greflectance[0];

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

	vec3 u = gposition[2] - gposition[0];
	vec3 n = normalize(cross(gposition[0] - gposition[1], gposition[2] - gposition[0]));
	vec3 v = cross(n, u);

	// Vertex 1
	fposition = gl_in[0].gl_Position.xyz;
	fuv = uv(gposition[0], n, u, v);

	gl_Position = transform * gl_in[0].gl_Position; EmitVertex();

	// Vertex 2
	fposition = gl_in[1].gl_Position.xyz;
	fuv = uv(gposition[1], n, u, v);

	gl_Position = transform * gl_in[1].gl_Position; EmitVertex();

	// Vertex 3
	fposition = gl_in[2].gl_Position.xyz;
	fuv = uv(gposition[2], n, u, v);

	gl_Position = transform * gl_in[2].gl_Position; EmitVertex();
	EndPrimitive();
}

//------------------------------------------------------------------------------//

[fragment]

out vec4 outColor;

in vec2 fuv;
in vec3 fposition;
in vec3 fnormal;
in vec3 fcenter;

in vec4 fambient;
in vec3 fdiffuse;
in vec3 fspecular;
in float freflectance;

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

const int maxLights = 5;
uniform Light lights[maxLights];

// Environment
uniform vec3 sunDirection = vec3(1, 1, 0);
uniform vec3 sunColor = vec3(1, 1, 1);
uniform float exposure = 1.0;
uniform float gamma = 1.0;
uniform int hdr = 1;

vec4 fog(vec4 color) {
	vec3 cameraDirection = -(viewMatrix * vec4(fposition, 1)).xyz;

	float b = 0.01;
	float fogAmount = 1.0 - exp(-length(cameraDirection) * b);
	float sunAmount = max(dot(normalize(cameraDirection), sunDirection), 0.0);
	vec3 fogColor = mix(
		vec3(0.5, 0.6, 0.7), // bluish
		vec3(1.0, 0.9, 0.7), // yellowish
		pow(sunAmount, 8.0)
	);
	return vec4(mix(color.rgb, fogColor, fogAmount), color.a);
}

vec3 calcDirectionalLight() {
	// Directional light
	vec3 directionalLight = normalize(sunDirection);
	float directionalFactor = 0.4 * max(dot(fnormal, directionalLight), 0.0);
	vec3 directional = directionalFactor * sunColor;
	return directional;
}

vec3 calcLightColor(Light light) {

	// Ambient
	float ambientStrength = 0.5;
	vec3 ambient = ambientStrength * light.color;

	// Diffuse light
	vec3 lightDirection = light.position - fposition;
	vec3 toLightSource = normalize(lightDirection);
	float diffuseFactor = max(dot(fnormal, toLightSource), 0.0);
	vec3 diffuse = fdiffuse * diffuseFactor * light.color;

	// Specular light
	float specularPower = 10.0f;
	vec3 cameraDirection = normalize(-(viewMatrix * vec4(fposition, 1)).xyz);
	vec3 fromLightSource = -toLightSource;
	vec3 reflectedLight = normalize(reflect(fromLightSource, fnormal));
	float specularFactor = max(dot(cameraDirection, (viewMatrix * vec4(reflectedLight, 0)).xyz), 0.0);
	specularFactor = pow(specularFactor, specularPower);
	vec3 specular = fspecular * freflectance * specularFactor * light.color;

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
	for (int i = 0; i < maxLights; i++) {
		if (lights[i].intensity > 0) {
			lightColors += calcLightColor(lights[i]);
			count++;
		}
	}

	// Take average of colors
	outColor = vec4(lightColors / count * fambient.rgb, fambient.a);

	// Directional light
	outColor = outColor + vec4(calcDirectionalLight(), 0);

	// Apply texture if present
	//outColor *= ftextured * texture(textureSampler, fuv) + (1 - ftextured) * vec4(1);

	// HDR correction
	outColor = hdr * vec4(vec3(1.0) - exp(-outColor.rgb * exposure), outColor.a) + (1 - hdr) * outColor;

	// Gamma correction
	outColor = vec4(pow(outColor.rgb, vec3(1.0 / gamma)), outColor.a);

	// Fog
	// outColor = fog(outColor);

	outColor = vec4(1);
}

