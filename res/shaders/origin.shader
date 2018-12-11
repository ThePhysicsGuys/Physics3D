#shader vertex // vertex shader
#version 330 core

layout(location = 0) in vec3 rotation;

out vec3 grotation;

void main() {
	grotation = rotation;
	gl_Position = vec4(0, 0, 0, 1);
}

///////////////////////////////////////////////////////////////////////////

#shader geometry // geometry shader
#version 330 core

layout(points) in;
layout(line_strip, max_vertices = 18) out;

in vec3 grotation[];

out vec3 fcolor;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 viewPosition;

void main() {
	vec3 unitVectors[3] = vec3[](vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1));
	for (int i = 0; i < 3; i++) {
		vec3 currentUnitVector = unitVectors[i];

		float arrowLength = 0.1;
		float arrowWidth = 0.025;
		vec4 origin = gl_in[0].gl_Position;
		vec4 arrowTop = gl_in[0].gl_Position + vec4(currentUnitVector, 0);
		vec3 norm = normalize(cross(arrowTop.xyz - viewPosition, currentUnitVector));
		vec3 unitRotation = normalize(currentUnitVector);
		vec4 arrowLeft = arrowTop - vec4(arrowLength * unitRotation - arrowWidth * norm, 0);
		vec4 arrowRight = arrowTop - vec4(arrowLength * unitRotation + arrowWidth * norm, 0);
		vec4 arrowBase = arrowTop - arrowLength * vec4(unitRotation, 0);

		fcolor = currentUnitVector;

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
}

///////////////////////////////////////////////////////////////////////////

#shader fragment // fragment shader
#version 330 core

layout(location = 0) out vec4 outColor;

in vec3 fcolor;

void main() {
	outColor = vec4(fcolor, 1);
}