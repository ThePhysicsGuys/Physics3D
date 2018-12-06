#shader vertex // vertex shader
#version 330 core 

layout(location = 0) in vec3 position; 
layout(location = 1) in vec3 rotation;
layout(location = 2) in float vcolor;

out vec3 grotation;
out float gcolor;

void main() { 
	gcolor = vcolor;
	grotation = rotation;
	gl_Position = vec4(position, 1);
}

///////////////////////////////////////////////////////////////////////////

#shader geometry // geometry shader
#version 330

layout(points) in;
layout(line_strip, max_vertices = 2) out;

in vec3 grotation[];
in float gcolor[];

out float fcolor;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {
	vec4 origin = gl_in[0].gl_Position;
	vec4 end = gl_in[0].gl_Position + vec4(grotation[0], 0);
	fcolor = gcolor[0];

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

in float fcolor;

vec3 hsvrgb(float hue) {
	vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	vec3 P = abs(fract(vec3(hue) + K.xyz) * 6.0 - K.www);
	return mix(K.xxx, clamp(P - K.xxx, 0.0, 1.0), 1);
}

void main() {
	outColor = vec4(hsvrgb(fcolor), 1);
}