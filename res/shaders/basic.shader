#shader vertex // vertex Shader
#version 330 core 

layout(location = 0) in vec3 position; 

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() { 
	gl_Position = projectionMatrix * viewMatrix * vec4(position, 1);
}

/////////////////////////////////////////////////////////////////////

#shader geometry // geometry shader
#version 330

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 fnormal;

vec3 normal() {
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	return normalize(cross(a, b));
}

void main() {
	fnormal = normal();

	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	gl_Position = gl_in[1].gl_Position;
	EmitVertex();

	gl_Position = gl_in[2].gl_Position;
	EmitVertex();

	EndPrimitive();
}


/////////////////////////////////////////////////////////////////////

#shader fragment // fragment shader
#version 330 core

layout(location = 0) out vec4 outColor;

in vec3 fnormal;

void main() {
	vec3 lightDir = vec3(0.2, -0.2, -0.2);
	float intensity  = dot(lightDir, fnormal);
	vec4 color;

	if (intensity > 0.95)
		color = vec4(1.0, 0.5, 0.5, 1.0);
	else if (intensity > 0.5)
		color = vec4(0.6, 0.3, 0.3, 1.0);
	else if (intensity > 0.25)
		color = vec4(0.4, 0.2, 0.2, 1.0);
	else
		color = vec4(0.2, 0.1, 0.1, 1.0);
	gl_FragColor = color;

}