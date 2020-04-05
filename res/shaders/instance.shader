[common]
#version 330 core

vec4 apply(mat4 matrix, vec3 vector) {
	return matrix * vec4(vector, 0.0);
}

vec3 apply3(mat4 matrix, vec3 vector) {
	return (matrix * vec4(vector, 0.0)).xyz;
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
	return normalize((matrix * vec4(vector, 0.0)).xyz);
}

vec4 applyTN(mat4 matrix, vec3 vector) {
	return normalize(matrix * vec4(vector, 1.0));
}

vec3 applyT3N(mat4 matrix, vec3 vector) {
	return normalize((matrix * vec4(vector, 1.0)).xyz);
}

[vertex]
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;
layout(location = 3) in mat4 vModelMatrix;
layout(location = 7) in vec4 vAmbient;
layout(location = 8) in vec3 vDiffuse;
layout(location = 9) in vec3 vSpecular;
layout(location = 10) in float vReflectance;

out vec4 gAmbient;
out vec3 gDiffuse;
out vec3 gSpecular;
out float gReflectance;

out vec3 gPosition;
out vec2 gUV;
out VS_OUT {
	vec3 value;
} gNormal;

uniform mat4 viewMatrix;

void main() {
	gAmbient = vAmbient;
	gDiffuse = vDiffuse;
	gSpecular = vSpecular;
	gReflectance = vReflectance;

	gUV = vUV;
	gPosition = vPosition;
	gNormal.value = apply3(vModelMatrix, vNormal);

	gl_Position = applyT(vModelMatrix, vPosition);
}  

[geometry]
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform int includeUVs;
uniform int includeNormals;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

in vec4 gAmbient[];
in vec3 gDiffuse[];
in vec3 gSpecular[];
in float gReflectance[];

in vec3 gPosition[];
in vec2 gUV[];
in VS_OUT {
	vec3 value;
} gNormal[];

out vec2 fUV;
out vec3 fPosition;
out vec3 fNormal;
out vec4 fAmbient;
out vec3 fDiffuse;
out vec3 fSpecular;
out float fReflectance;

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

	mat4 transform = projectionMatrix * viewMatrix;

	vec3 u = gPosition[2] - gPosition[0];
	vec3 n = normalize(cross(gPosition[0] - gPosition[1], gPosition[2] - gPosition[0]));
	vec3 v = cross(n, u);

	// Vertex 1
	fPosition = gl_in[0].gl_Position.xyz;
	fUV = includeUVs * gUV[0] + (1 - includeUVs) * uv(gPosition[0], n, u, v);
	fNormal = includeNormals * gNormal[0].value + (1 - includeNormals) * fNormal;
	fAmbient = gAmbient[0];
	fDiffuse = gDiffuse[0];
	fSpecular = gSpecular[0];
	fReflectance = gReflectance[0];

	gl_Position = transform * gl_in[0].gl_Position; EmitVertex();

	// Vertex 2
	fPosition = gl_in[1].gl_Position.xyz;
	fUV = includeUVs * gUV[1] + (1 - includeUVs) * uv(gPosition[1], n, u, v);
	fNormal = includeNormals * gNormal[1].value + (1 - includeNormals) * fNormal;
	fAmbient = gAmbient[1];
	fDiffuse = gDiffuse[1];
	fSpecular = gSpecular[1];
	fReflectance = gReflectance[1];

	gl_Position = transform * gl_in[1].gl_Position; EmitVertex();

	// Vertex 3
	fPosition = gl_in[2].gl_Position.xyz;
	fUV = includeUVs * gUV[2] + (1 - includeUVs) * uv(gPosition[2], n, u, v);
	fNormal = includeNormals * gNormal[2].value + (1 - includeNormals) * fNormal;
	fAmbient = gAmbient[2];
	fDiffuse = gDiffuse[2];
	fSpecular = gSpecular[2];
	fReflectance = gReflectance[2];

	gl_Position = transform * gl_in[2].gl_Position; EmitVertex();
	EndPrimitive();
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

// Appearance
#define maxLights 10
uniform int lightCount;
uniform Light lights[maxLights];
uniform sampler2D textureSampler;
uniform int textured;

// Environment
uniform vec3 sunDirection = vec3(1, 1, 0);
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
	vec3 lightDirection = light.position - fPosition;
	vec3 toLightSource = normalize(lightDirection);
	float diffuseFactor = max(dot(fNormal, toLightSource), 0.0);
	vec3 diffuse = fDiffuse * diffuseFactor * light.color;

	// Specular light
	float specularPower = 10.0f;
	vec3 cameraDirection = -applyT3N(viewMatrix, fPosition);
	vec3 fromLightSource = -toLightSource;
	vec3 reflectedLight = normalize(reflect(fromLightSource, fNormal));
	float specularFactor = max(dot(cameraDirection, apply3(viewMatrix, reflectedLight)), 0.0);
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
	outColor *= textured * texture(textureSampler, fUV) + (1 - textured) * vec4(1);

	// HDR correction
	outColor = hdr * vec4(vec3(1.0) - exp(-outColor.rgb * exposure), outColor.a) + (1 - hdr) * outColor;

	// Gamma correction
	outColor = vec4(pow(outColor.rgb, vec3(1.0 / gamma)), outColor.a);

}

