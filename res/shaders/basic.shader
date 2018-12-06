#shader vertex // vertex Shader
#version 330 core 

layout(location = 0) in vec3 position; 

void main() { 
	gl_Position = vec4(position, 1);
}

///////////////////////////////////////////////////////////////////////////

#shader geometry // geometry shader
#version 330

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

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
	if (dot(fcenter, fnormal) > 0) {
		return norm;
	} else {
		return -norm;
	}
}

void main() {
	fcenter = center();
	fnormal = normal();

	gl_Position = projectionMatrix * viewMatrix * gl_in[0].gl_Position;
	EmitVertex();

	gl_Position = projectionMatrix * viewMatrix * gl_in[1].gl_Position;
	EmitVertex();

	gl_Position = projectionMatrix * viewMatrix * gl_in[2].gl_Position;
	EmitVertex();

	EndPrimitive();
}

///////////////////////////////////////////////////////////////////////////

#shader fragment // fragment shader
#version 330 core

layout(location = 0) out vec4 outColor;

uniform vec3 viewPos;

in vec3 fcenter;
in vec3 fnormal;

void main() {
	// vec3 objectColor = vec3(0.1, 0.8, 0.5);
	float ambientStrength = 0.4;
	float specularStrength = 1;
	vec3 lightColor = vec3(0.5, 0.2, 0.6);
	vec3 lightPos = vec3(-2, 3, 4);

	vec3 ambient = ambientStrength * lightColor;
	
	vec3 lightDir = normalize(lightPos - fcenter);
	float diff = max(dot(fnormal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	
	vec3 viewDir = normalize(viewPos - fcenter);
	vec3 reflectDir = reflect(-lightDir, fnormal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * 1;
	outColor = vec4(result, 1.0);
}