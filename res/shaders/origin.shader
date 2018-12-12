#shader vertex // vertex shader
#version 330 core

layout(location = 0) in vec3 rotation;

out vec3 grotation;

void main() {
	grotation = rotation;
	gl_Position = vec4(0.8, -0.8, -1, 1);
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

void main() {
	vec3 unitVectors[3] = vec3[](vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1));
	vec3 viewPosition = vec3(0, 0, 100);
	float size = 0.1;
	for (int i = 0; i < 3; i++) {
		vec3 rotatedUnitVector = (viewMatrix * vec4(unitVectors[i], 0)).xyz * size;

		float arrowLength = 0.025;
		float arrowWidth = 0.00625;
		vec4 origin = vec4(0, 0, gl_in[0].gl_Position.zw);
		vec4 arrowTop = origin + vec4(rotatedUnitVector, 0);
		vec3 norm = normalize(cross(arrowTop.xyz - viewPosition, rotatedUnitVector));
		vec3 unitRotation = normalize(rotatedUnitVector);
		vec4 arrowLeft = arrowTop - vec4(arrowLength * unitRotation - arrowWidth * norm, 0);
		vec4 arrowRight = arrowTop - vec4(arrowLength * unitRotation + arrowWidth * norm, 0);
		vec4 arrowBase = arrowTop - arrowLength * vec4(unitRotation, 0);

		fcolor = unitVectors[i];
		vec4 position = vec4(gl_in[0].gl_Position.xy, 0, 0);
		gl_Position = position + projectionMatrix * origin; EmitVertex();
		gl_Position = position + projectionMatrix * arrowBase; EmitVertex();
		gl_Position = position + projectionMatrix * arrowLeft; EmitVertex();
		gl_Position = position + projectionMatrix * arrowTop; EmitVertex();
		gl_Position = position + projectionMatrix * arrowRight; EmitVertex();
		gl_Position = position + projectionMatrix * arrowBase; EmitVertex();
		

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