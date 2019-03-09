#shader vertex
#version 330 core 

layout(location = 0) in vec3 vposition; 
layout(location = 1) in vec3 vnormal;

out vec3 fposition;
out vec3 fnormal;

uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

void main() { 
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vposition, 1.0f);
	fposition =  (viewMatrix * modelMatrix * vec4(vposition, 1.0f)).xyz;
	fnormal = (modelMatrix * vec4(vnormal, 0.0f)).xyz;
}

/////////////////////////////////////////////////////////////////////

#shader fragment 
#version 330 core

out vec4 outColor;

in vec3 fposition;
in vec3 fnormal;

uniform vec3 viewPosition;

void main() {
	vec3 lightPosition = vec3(2, 0, 0);

	vec3 ambient = vec3(0.8, 0, 0.1);
	vec3 diffuse = vec3(0.3, 0.3, 0.8);
	vec3 specular = vec3(0.2, 0.2, 1);

	vec3 L = normalize(lightPosition - fposition);
	vec3 E = normalize(-fposition);
	vec3 R = normalize(reflect(L, fnormal));

	vec3 diffuseColor = clamp(diffuse * max(dot(fnormal, L), 0), 0, 1);

	vec3 specularColor = 0.1 * clamp(specular * pow(max(dot(R, E), 0.0), 0.1), 0, 1);

	outColor = vec4(ambient + diffuseColor + specularColor, 1);
}