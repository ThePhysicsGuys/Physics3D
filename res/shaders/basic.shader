#shader vertex // vertex Shader
#version 330 core 

layout(location = 0) in vec3 vposition; 

out vec3 gposition;

void main() { 
	gl_Position = vec4(vposition, 1);
	gposition = vposition;
}

///////////////////////////////////////////////////////////////////////////

#shader geometry // geometry shader
#version 330

// #define DEBUG 

layout(triangles) in;
#ifndef DEBUG
layout(triangle_strip, max_vertices = 3) out;
#else
layout(line_strip, max_vertices = 9) out;
#endif

uniform mat4 modelMatrix;
uniform vec3 viewPosition;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

in vec3 gposition[];

out vec3 fposition;
out vec3 fcenter;
out vec3 fnormal;

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
	fcenter = center();
	fnormal = normal();

	mat4 transform = projectionMatrix * viewMatrix * transpose(modelMatrix);

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

///////////////////////////////////////////////////////////////////////////

#shader fragment // fragment shader
#version 330 core

layout(location = 0) out vec4 outColor;

uniform mat4 modelMatrix;
uniform vec3 viewPosition;
uniform vec3 color;

in vec3 fposition;
in vec3 fcenter;
in vec3 fnormal;

struct Light {
	vec3 position;
	vec3 color;
};

const int lightCount = 3;

Light lights[lightCount] = Light[](
	Light(vec3(-14, 17, 10), vec3(1, 0, 0)),
	Light(vec3(15, 13, -13), vec3(0, 1, 0)),
	Light(vec3(-15, -14, -13), vec3(0, 0, 1))
);

void main() {
	vec3 fragPosition = vec3(modelMatrix * vec4(fposition, 1));
	vec3 fragNormal = vec3(modelMatrix * vec4(fnormal, 1));
	vec3 viewDirection = normalize(viewPosition - fragPosition);
	
	float ambientStrength = 0.4;
	vec3 ambient = ambientStrength * color;
	
	vec3 lightColorBuffer;
	for (int i = 0; i < lightCount; i++) {
		Light currentLight = lights[i];

		vec3 lightDirection = normalize(currentLight.position - fragPosition);
		vec3 reflectDirection = reflect(-lightDirection, fragNormal);

		float diffuseStrength = 2.0f / lightCount;
		float diffuseFactor = dot(fragNormal, lightDirection);
		vec3 diffuse = diffuseStrength * clamp(diffuseFactor, 0, 1) * currentLight.color;

		float specularStrength = 0.4;
		float specularFactor = pow(max(dot(viewDirection, reflectDirection), 0.0), 32);
		vec3 specular = specularStrength * specularFactor * currentLight.color;

		lightColorBuffer += diffuse + specular;
	}

	outColor = vec4(ambient + lightColorBuffer, 1);
}