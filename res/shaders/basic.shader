#shader vertex // vertex Shader
#version 330 core 

layout(location = 0) in vec3 position; 

void main() { 
	gl_Position = vec4(position, 1);
}

///////////////////////////////////////////////////////////////////////////

#shader geometry // geometry shader
#version 330

#define DEBUG 

layout(triangles) in;
#ifndef DEBUG
layout(triangle_strip, max_vertices = 3) out;
#else
layout(line_strip, max_vertices = 9) out;
#endif

uniform vec3 viewPosition;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

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

#ifdef DEBUG
	float arrowLength = 0.05;
	float arrowWidth = 0.02;
	vec4 arrowTop = vec4(fcenter + 0.3 * fnormal, 1);
	vec3 norm = normalize(cross(arrowTop.xyz - viewPosition, fnormal));
	vec4 arrowLeft = arrowTop - vec4(arrowLength * fnormal - arrowWidth * norm, 0);
	vec4 arrowRight = arrowTop - vec4(arrowLength * fnormal + arrowWidth * norm, 0);
	vec4 arrowBase = arrowTop - arrowLength * vec4(fnormal, 0);

	gl_Position = projectionMatrix * viewMatrix * vec4(fcenter, 1); EmitVertex();
	gl_Position = projectionMatrix * viewMatrix * arrowBase; EmitVertex();
	gl_Position = projectionMatrix * viewMatrix * arrowLeft; EmitVertex();
	gl_Position = projectionMatrix * viewMatrix * arrowTop; EmitVertex();
	gl_Position = projectionMatrix * viewMatrix * arrowRight; EmitVertex();
	gl_Position = projectionMatrix * viewMatrix * arrowBase; EmitVertex();
	EndPrimitive();
#endif

	gl_Position = projectionMatrix * viewMatrix * gl_in[0].gl_Position; EmitVertex();
	gl_Position = projectionMatrix * viewMatrix * gl_in[1].gl_Position; EmitVertex();
	gl_Position = projectionMatrix * viewMatrix * gl_in[2].gl_Position; EmitVertex();
	EndPrimitive();
}

///////////////////////////////////////////////////////////////////////////

#shader fragment // fragment shader
#version 330 core

layout(location = 0) out vec4 outColor;

uniform vec3 viewPos;
uniform vec3 color;

in vec3 fcenter;
in vec3 fnormal;

void main() {
	float ambientStrength = 0.4;
	float specularStrength = 1;
	vec3 lightPos = vec3(-2, 3, 4);

	vec3 ambient = ambientStrength * color;
	
	vec3 lightDir = normalize(lightPos - fcenter);
	float diff = max(dot(fnormal, lightDir), 0.0);
	vec3 diffuse = diff * color;
	
	vec3 viewDir = normalize(viewPos - fcenter);
	vec3 reflectDir = reflect(-lightDir, fnormal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * color;

	vec3 result = (ambient + diffuse + specular) * 1;
	outColor = vec4(result, 1.0);
}