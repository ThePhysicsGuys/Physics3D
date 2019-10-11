[common]

#version 330 core

//------------------------------------------------------------------------------//

[vertex]

layout(location = 0) in vec3 vposition;
layout(location = 1) in float vsize;
layout(location = 2) in vec3 vcolor1;
layout(location = 3) in vec3 vcolor2;

out vec3 gcolor1;
out vec3 gcolor2;
out float gsize;

void main() { 
	gcolor1 = vcolor1;
	gcolor2 = vcolor2;
	gsize = vsize;
	gl_Position = vec4(vposition, 1);
}

//------------------------------------------------------------------------------//

[geometry]

layout(points) in;
layout(triangle_strip, max_vertices = 24) out;

in vec3 gcolor1[];
in vec3 gcolor2[];
in float gsize[];

out vec3 fcolor;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 viewPosition;

void makeTriangle(vec4 a, vec4 b, vec4 c, vec3 color) {
	fcolor = color;
	gl_Position = projectionMatrix * viewMatrix * a;
	EmitVertex();

	gl_Position = projectionMatrix * viewMatrix * b;
	EmitVertex();

	gl_Position = projectionMatrix * viewMatrix * c;
	EmitVertex();
	EndPrimitive();
}

void main() {
	vec4 origin = gl_in[0].gl_Position;
	float size = gsize[0] * sqrt(length(origin.xyz-viewPosition));
	vec3 colorA = gcolor1[0];
	vec3 colorB = gcolor2[0];
	vec4 xPos = origin + vec4(size, 0, 0, 0);
	vec4 xNeg = origin + vec4(-size, 0, 0, 0);
	vec4 yPos = origin + vec4(0, size, 0, 0);
	vec4 yNeg = origin + vec4(0, -size, 0, 0);
	vec4 zPos = origin + vec4(0, 0, size, 0);
	vec4 zNeg = origin + vec4(0, 0, -size, 0);


	makeTriangle(xPos, yPos, zPos, colorA);
	makeTriangle(zPos, yPos, xNeg, colorB);
	makeTriangle(xNeg, yPos, zNeg, colorA);
	makeTriangle(zNeg, yPos, xPos, colorB);

	makeTriangle(zNeg, yNeg, xNeg, colorB);
	makeTriangle(xPos, yNeg, zNeg, colorA);
	makeTriangle(zPos, yNeg, xPos, colorB);
	makeTriangle(xNeg, yNeg, zPos, colorA);
}

//------------------------------------------------------------------------------//

[fragment]

layout(location = 0) out vec4 outColor;

in vec3 fcolor;

void main() {
	outColor = vec4(fcolor, 1);
}