#shader vertex // vertex shader
#version 330 core 

layout(location = 0) in vec3 vposition; 
layout(location = 1) in vec3 vrotation;
layout(location = 2) in vec3 vcolor;

out vec3 grotation;
out vec3 gcolor;

void main() { 
	gcolor = vcolor;
	grotation = vrotation;
	gl_Position = vec4(vposition, 1);
}

///////////////////////////////////////////////////////////////////////////

#shader geometry // geometry shader
#version 330

layout(points) in;
layout(line_strip, max_vertices = 6) out;

in vec3 grotation[];
in vec3 gcolor[];

out vec3 fcolor;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 viewPosition;

void main() {
	float arrowLength = 0.1;
	float arrowWidth = 0.025;
	vec4 origin = gl_in[0].gl_Position;
	vec4 arrowTop = gl_in[0].gl_Position + vec4(grotation[0], 0);
	vec3 norm = normalize(cross(arrowTop.xyz - viewPosition, grotation[0]));
	vec3 unitRotation = normalize(grotation[0]);
	vec4 arrowLeft = arrowTop - vec4(arrowLength * unitRotation - arrowWidth * norm, 0);
	vec4 arrowRight = arrowTop - vec4(arrowLength * unitRotation + arrowWidth * norm, 0);
	vec4 arrowBase = arrowTop - arrowLength * vec4(unitRotation, 0);

	fcolor = gcolor[0];

	gl_Position = projectionMatrix * viewMatrix * origin;
	EmitVertex();

	gl_Position = projectionMatrix * viewMatrix * arrowBase;
	EmitVertex();

	gl_Position = projectionMatrix * viewMatrix * arrowLeft;
	EmitVertex();

	gl_Position = projectionMatrix * viewMatrix * arrowTop;
	EmitVertex();

	gl_Position = projectionMatrix * viewMatrix * arrowRight;
	EmitVertex();

	gl_Position = projectionMatrix * viewMatrix * arrowBase;
	EmitVertex();

	EndPrimitive();
}

///////////////////////////////////////////////////////////////////////////

#shader fragment // fragment shader
#version 330 core

layout(location = 0) out vec4 outColor;

in vec3 fcolor;

void main() {
	outColor = vec4(fcolor, 1);
}