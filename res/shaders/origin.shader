[common]

#version 330 core

//------------------------------------------------------------------------------//

[vertex]

layout(location = 0) in vec3 rotation;

void main() {
	gl_Position = vec4(0.8, -0.8, -1, 1);
}

//------------------------------------------------------------------------------//

[geometry]

layout(points) in;
layout(line_strip, max_vertices = 256) out;

uniform mat4 uRotatedViewMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uOrthoMatrix;
uniform vec3 uViewPosition;

out vec3 fcolor;

int gridSize = 32;

void main() {
	vec3 unitVectors[3] = vec3[](vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1));

	float size = 0.1;
	for (int i = 0; i < 3; i++) {
		vec3 rotatedUnitVector = (uRotatedViewMatrix * vec4(unitVectors[i], 0)).xyz * size;

		float arrowLength = 0.025;
		float arrowWidth = 0.00625;
		vec4 origin = vec4(0, 0, gl_in[0].gl_Position.zw);
		vec4 arrowTop = origin + vec4(rotatedUnitVector, 0);
		vec3 norm = normalize(cross(arrowTop.xyz, rotatedUnitVector));
		vec3 unitRotation = normalize(rotatedUnitVector);
		vec4 arrowLeft = arrowTop - vec4(arrowLength * unitRotation - arrowWidth * norm, 0);
		vec4 arrowRight = arrowTop - vec4(arrowLength * unitRotation + arrowWidth * norm, 0);
		vec4 arrowBase = arrowTop - arrowLength * vec4(unitRotation, 0);

		fcolor = unitVectors[i];
		vec4 position = vec4(gl_in[0].gl_Position.xy, 0, 0);
		gl_Position = position + uOrthoMatrix * origin; EmitVertex();
		gl_Position = position + uOrthoMatrix * arrowBase; EmitVertex();
		gl_Position = position + uOrthoMatrix * arrowLeft; EmitVertex();
		gl_Position = position + uOrthoMatrix * arrowTop; EmitVertex();
		gl_Position = position + uOrthoMatrix * arrowRight; EmitVertex();
		gl_Position = position + uOrthoMatrix * arrowBase; EmitVertex();
		EndPrimitive();
	}

	/*float camerax = floor(viewPosition.x);
	float cameraz = floor(viewPosition.z);
	
	for (float t = 0.0; t < gridSize / 2; t++) {	
		fcolor = vec3(0.5 - t / gridSize);
		gl_Position = projectionMatrix * uViewMatrix * vec4(camerax - gridSize / 2, 0, cameraz + t, 1); EmitVertex();
		gl_Position = projectionMatrix * uViewMatrix * vec4(camerax + gridSize / 2, 0, cameraz + t, 1); EmitVertex();
		EndPrimitive();

		gl_Position = projectionMatrix * uViewMatrix * vec4(camerax - gridSize / 2, 0, cameraz - t, 1); EmitVertex();
		gl_Position = projectionMatrix * uViewMatrix * vec4(camerax + gridSize / 2, 0, cameraz - t, 1); EmitVertex();
		EndPrimitive();

		gl_Position = projectionMatrix * uViewMatrix * vec4(camerax + t, 0, cameraz - gridSize / 2, 1); EmitVertex();
		gl_Position = projectionMatrix * uViewMatrix * vec4(camerax + t, 0, cameraz + gridSize / 2, 1); EmitVertex();
		EndPrimitive();

		gl_Position = projectionMatrix * uViewMatrix * vec4(camerax - t, 0, cameraz - gridSize / 2, 1); EmitVertex();
		gl_Position = projectionMatrix * uViewMatrix * vec4(camerax - t, 0, cameraz + gridSize / 2, 1); EmitVertex();
		EndPrimitive();
	}*/
}

//------------------------------------------------------------------------------//

[fragment]

layout(location = 0) out vec4 outColor;

in vec3 fcolor;

void main() {
	outColor = vec4(fcolor, 1);
}