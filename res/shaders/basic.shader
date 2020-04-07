[common]

#version 330 core

//------------------------------------------------------------------------------//

[vertex]

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec3 vBitangent;

out vec3 gPosition;
out vec2 gUV;

out VS_OUT {
	vec3 value;
} gNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

void main() {
	gUV = vUV;
	gPosition = vPosition;
	gNormal.value = (modelMatrix * vec4(vNormal, 0.0)).xyz;
	gl_Position = modelMatrix * vec4(vPosition, 1.0);
}

//------------------------------------------------------------------------------//

[geometry]

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform int includeUvs;
uniform int includeNormals;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

in vec3 gPosition[];
in vec2 gUV[];

in VS_OUT {
	vec3 value;
} gNormal[];

out vec2 fUV;
out vec3 fPosition;
out vec3 fNormal;
out vec3 fCenter;

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
	fNormal = normal();
	fCenter = center();

	mat4 transform = projectionMatrix * viewMatrix;

	vec3 u = gPosition[2] - gPosition[0];
	vec3 n = normalize(cross(gPosition[0] - gPosition[1], gPosition[2] - gPosition[0]));
	vec3 v = cross(n, u);

	// Vertex 1
	fPosition = gl_in[0].gl_Position.xyz;
	fUV = includeUvs * gUV[0] + (1 - includeUvs) * uv(gPosition[0], n, u, v);
	fNormal = includeNormals * gNormal[0].value + (1 - includeNormals) * fNormal;

	gl_Position = transform * gl_in[0].gl_Position; EmitVertex();

	// Vertex 2
	fPosition = gl_in[1].gl_Position.xyz;
	fUV = includeUvs * gUV[1] + (1 - includeUvs) * uv(gPosition[1], n, u, v);
	fNormal = includeNormals * gNormal[1].value + (1 - includeNormals) * fNormal;

	gl_Position = transform * gl_in[1].gl_Position; EmitVertex();

	// Vertex 3
	fPosition = gl_in[2].gl_Position.xyz;
	fUV = includeUvs * gUV[2] + (1 - includeUvs) * uv(gPosition[2], n, u, v);
	fNormal = includeNormals * gNormal[2].value + (1 - includeNormals) * fNormal;

	gl_Position = transform * gl_in[2].gl_Position; EmitVertex();
	EndPrimitive();
}

//------------------------------------------------------------------------------//

[fragment]

out vec4 outColor;

in vec2 fUV;
in vec3 fPosition;
in vec3 fNormal;
in vec3 fCenter;

struct Material {
	vec4 ambient;
	vec3 diffuse;
	vec3 specular;
	float reflectance;
	int textured;
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

uniform sampler2D uniforms;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform Material material;
uniform sampler2D textureSampler;
#define maxLights 10
uniform int lightCount;
uniform Light lights[maxLights];

// Environment
uniform vec3 sunDirection = vec3(1, 1, 0);
uniform vec3 sunColor = vec3(1, 1, 1);
uniform float exposure = 1.0;
uniform float gamma = 1.0;
uniform int hdr = 1;

vec4 fog(vec4 color) {
	vec3 cameraDirection = -(viewMatrix * vec4(fPosition, 1)).xyz;

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
	float directionalFactor = 0.4 * max(dot(fNormal, directionalLight), 0.0);
	vec3 directional = directionalFactor * sunColor;
	return directional;
}

vec3 calcLightColor(Light light) {

	// Ambient
	float ambientStrength = 0.5;
	vec3 ambient = ambientStrength * light.color;

	// Diffuse light
	vec3 lightDirection = light.position - fPosition;
	vec3 toLightSource = normalize(lightDirection);
	float diffuseFactor = max(dot(fNormal, toLightSource), 0.0);
	vec3 diffuse = material.diffuse * diffuseFactor * light.color;

	// Specular light
	float specularPower = 10.0f;
	vec3 cameraDirection = normalize(-(viewMatrix * vec4(fPosition, 1)).xyz);
	vec3 fromLightSource = -toLightSource;
	vec3 reflectedLight = normalize(reflect(fromLightSource, fNormal));
	float specularFactor = max(dot(cameraDirection, (viewMatrix * vec4(reflectedLight, 0)).xyz), 0.0);
	specularFactor = pow(specularFactor, specularPower);
	vec3 specular = material.specular * material.reflectance * specularFactor * light.color;

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
	outColor = vec4(lightColors / count * material.ambient.rgb, material.ambient.a);

	// Directional light
	outColor = outColor + vec4(calcDirectionalLight(), 0);

	// Apply texture if present
	outColor *= material.textured * texture(textureSampler, fUV) + (1 - material.textured) * vec4(1);

	// HDR correction
	outColor = hdr * vec4(vec3(1.0) - exp(-outColor.rgb * exposure), outColor.a) + (1 - hdr) * outColor;

	// Gamma correction
	outColor = vec4(pow(outColor.rgb, vec3(1.0 / gamma)), outColor.a);

	// Fog
	// outColor = fog(outColor);
}

