#shader vertex // vertex shader
#version 330 core 

layout(location = 0) in vec3 position; 
layout(location = 1) in vec3 rotation;

out vec3 grotation;

void main() { 
	grotation = rotation;
	gl_Position = vec4(position, 1);
}

///////////////////////////////////////////////////////////////////////////

#shader geometry // geometry shader
#version 330

layout(points) in;
layout(line_strip, max_vertices = 2) out;

in vec3 grotation[];

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {
	vec4 origin = gl_in[0].gl_Position;
	vec4 end = gl_in[0].gl_Position + vec4(grotation[0], 0);

	gl_Position = projectionMatrix * viewMatrix * origin;
	EmitVertex();

	gl_Position = projectionMatrix * viewMatrix * end;
	EmitVertex();

	EndPrimitive();
}

///////////////////////////////////////////////////////////////////////////

#shader fragment // fragment shader
#version 330 core

layout(location = 0) out vec4 outColor;

void main() {
	outColor = vec4(1, 1, 1, 1);
}