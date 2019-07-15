#shader vertex // vertex Shader
#version 330 core

layout(location = 0) in vec3 vposition;
layout(location = 1) in vec3 vnormal;
layout(location = 2) in vec2 vuv;

out vec3 gposition;
out vec2 guv;

out VS_OUT {
	vec3 value;
} gnormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

void main() {
	guv = vuv;
	gposition = vposition;
	gnormal.value = (modelMatrix * vec4(vnormal, 0.0)).xyz;
	gl_Position = modelMatrix * vec4(vposition, 1.0);
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

uniform bool includeUvs;
uniform bool includeNormals;

uniform vec3 viewPosition;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

in vec3 gposition[];
in vec2 guv[];

in VS_OUT {
	vec3 value;
} gnormal[];

out vec2 fuv;
out vec3 fposition;
out vec3 fnormal;
out vec3 fcenter;

vec3 center(bool transform) {
	if (transform)
		return vec3(gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3;
	else
		return (gposition[0] + gposition[1] + gposition[2]) / 3;
}

vec3 normal(bool transform) {
	vec3 a; 
	vec3 b;
	if (transform) {
		a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
		b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	} else {
		a = gposition[0] - gposition[1];
		b = gposition[2] - gposition[0];
	}
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
	fnormal = normal(true);
	fcenter = center(true);

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

	vec3 u = gposition[2] - gposition[0];
	vec3 n = normal(false);
	vec3 v = cross(n, u);
	vec3 c = center(false);
	if (dot(c-dot(c, n) * n, u) < 0) {
		vec3 t = v;
		v = u;
		u = -t;
	}

	// Vertex 1
	fposition = gl_in[0].gl_Position.xyz;

	if (includeUvs) fuv = guv[0];
	else fuv = uv(gposition[0], n, u, v);

	if (includeNormals) fnormal = gnormal[0].value;

	gl_Position = transform * gl_in[0].gl_Position; EmitVertex();


	// Vertex 2
	fposition = gl_in[1].gl_Position.xyz;

	if (includeUvs) fuv = guv[1];
	else fuv = uv(gposition[1], n, u, v);

	if (includeNormals) fnormal = gnormal[1].value;

	gl_Position = transform * gl_in[1].gl_Position; EmitVertex();


	// Vertex 3
	fposition = gl_in[2].gl_Position.xyz;

	if (includeUvs) fuv = guv[2];
	else fuv = uv(gposition[2], n, u, v);

	if (includeNormals) fnormal = gnormal[2].value;

	gl_Position = transform * gl_in[2].gl_Position; EmitVertex();
	EndPrimitive();
}

//////////////////////////////////////////////////////////////////////////

#shader fragment
#version 330

out vec4 outColor;

in vec2 fuv;
in vec3 fposition;
in vec3 fnormal;
in vec3 fcenter;

vec3 normal;

struct Material {
	vec4 ambient;
	vec3 diffuse;
	vec3 specular;
	float reflectance;
	bool textured;
	bool normalmapped;
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

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform Material material;
uniform sampler2D textureSampler;
uniform sampler2D normalSampler;
const int maxLights = 5;
uniform Light lights[maxLights];

// Environment
uniform vec3 sunDirection = vec3(1, 1, 0);
uniform vec3 sunColor = vec3(1, 1, 1);
uniform float exposure = 1.0;
uniform float gamma = 1.0;
uniform bool hdr = true;

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
	float directionalFactor = 0.4 * max(dot(normal, directionalLight), 0.0);
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
	float diffuseFactor = max(dot(normal, toLightSource), 0.0);
	vec3 diffuse = material.diffuse * diffuseFactor * light.color;

	// Specular light
	float specularPower = 10.0f;
	vec3 cameraDirection = normalize(-(viewMatrix * vec4(fposition, 1)).xyz);
	vec3 fromLightSource = -toLightSource;
	vec3 reflectedLight = normalize(reflect(fromLightSource, normal));
	float specularFactor = max(dot(cameraDirection, (viewMatrix * vec4(reflectedLight, 0)).xyz), 0.0);
	specularFactor = pow(specularFactor, specularPower);
	vec3 specular = material.specular * material.reflectance * specularFactor * light.color;

	// Attenuation
	float distance = length(lightDirection) / light.intensity;
	float attenuationInverse = light.attenuation.constant + light.attenuation.linear * distance + light.attenuation.exponent * distance * distance;
	vec3 specularDiffuse = (diffuse + specular) / attenuationInverse;

	return ambient + specularDiffuse;
}

mat3 rodrigues() {
	vec3 z = vec3(0, 0, 1);
	vec3 v = normalize(cross(z, fnormal));
	float a = acos(dot(z, fnormal));
	mat3 W = mat3(0, v.z, -v.y, -v.z, 0, v.x, v.y, -v.x, 0);
	mat3 R = mat3(1) + sin(a) * W + (1 - cos(a)) * W * W;
	return R;
}

void main() {
	/*if (material.normalmapped) {
		normal = texture(normalSampler, fuv).rgb;
		normal = normalize(normal * 2 - 1);
		normal = normalize(modelMatrix * vec4(normal, 0)).xyz;
		normal = rodrigues() * normal;
	}*/
	normal = fnormal;

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
	outColor = vec4(lightColors / count * material.ambient.rgb, material.ambient.a);

	// Directional light
	outColor = outColor + vec4(calcDirectionalLight(), 0);

	// Apply texture if present
	if (material.textured)
		outColor = outColor * texture(textureSampler, fuv);

	// HDR correction
	//outColor = vec4(outColor.rgb / (outColor.rgb + vec3(1.0)), outColor.a);
	if (hdr)
		outColor = vec4(vec3(1.0) - exp(-outColor.rgb * exposure), outColor.a);

	// Gamma correction
	outColor = vec4(pow(outColor.rgb, vec3(1.0 / gamma)), outColor.a);

	// Fog
	// outColor = fog(outColor);
}

