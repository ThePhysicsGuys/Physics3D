[common]

#version 400

/* Usage:
	// Once
	ApplicationShaders::debugShader.updateProjection(viewMatrix, projectionMatrix, viewPosition);
	// For every model
	ApplicationShaders::debugShader.updateModel(modelMatrix);
	mesh->render();
*/

// Debug vector type defenitions
#define FACES		    fColor = vec3(1, 1, 1); gl_Position = transform * gl_in[0].gl_Position; EmitVertex(); gl_Position = transform * gl_in[1].gl_Position; EmitVertex(); gl_Position = transform * gl_in[2].gl_Position; EmitVertex(); EndPrimitive();
#define VERTEXNORMALS   fColor = vec3(0, 0, 1); draw(transform, gl_in[0].gl_Position.xyz, gInput[0].gNormal); draw(transform, gl_in[1].gl_Position.xyz, gInput[1].gNormal); draw(transform, gl_in[2].gl_Position.xyz, gInput[2].gNormal);
#define FACENORMALS     vec3 center = center(); vec3 faceNormal = faceNormal(); fColor = vec3(1, 0, 1); draw(transform, center, faceNormal);
#define TANGENTS        fColor = vec3(1, 0, 0); draw(transform, gl_in[0].gl_Position.xyz, gInput[0].gTangent); draw(transform, gl_in[1].gl_Position.xyz, gInput[1].gTangent); draw(transform, gl_in[2].gl_Position.xyz, gInput[2].gTangent);
#define BITANGENTS      fColor = vec3(0, 1, 0); draw(transform, gl_in[0].gl_Position.xyz, gInput[0].gBitangent); draw(transform, gl_in[1].gl_Position.xyz, gInput[1].gBitangent); draw(transform, gl_in[2].gl_Position.xyz, gInput[2].gBitangent);

// Amount of different debug vector to show (max 5)
#define VECTOR_TYPES	3

// Reorder for different types
#define TYPE_1			FACES
#define TYPE_2			FACENORMALS
#define TYPE_3			VERTEXNORMALS
#define TYPE_4			TANGENTS
#define TYPE_5			BITANGENTS

// Define to show arrow head
//#define ARROW_HEAD
#define ARROW_LENGTH	0.05
#define ARROW_WIDTH		0.025

//------------------------------------------------------------------------------//

[vertex]

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec3 vBitangent;

out vec3 gNormal;
out vec3 gTangent;
out vec3 getBitangent;

out VS_OUT {
	vec3 gNormal;
	vec3 gTangent;
	vec3 gBitangent;
} vOutput;

uniform mat4 modelMatrix;

vec4 applyT(mat4 matrix, vec3 vector) {
	return matrix * vec4(vector, 1.0);
}

vec3 apply3(mat4 matrix, vec3 vector) {
	return mat3(matrix) * vector;
}

void main() {
	mat3 invModelMatrix = transpose(inverse(mat3(modelMatrix)));
	vOutput.gNormal = normalize(invModelMatrix * vNormal);
	vOutput.gTangent = normalize(apply3(modelMatrix, vTangent));
	vOutput.gBitangent = normalize(apply3(modelMatrix, vBitangent));

	gl_Position = applyT(modelMatrix, vPosition);
}

//------------------------------------------------------------------------------//

[geometry]

layout(triangles, invocations = VECTOR_TYPES) in;
layout(line_strip, max_vertices = 54) out;

in VS_OUT {
	vec3 gNormal;
	vec3 gTangent;
	vec3 gBitangent;
} gInput[];

out vec3 fColor;

uniform vec3 viewPosition;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

vec3 center() {
	return vec3(gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3;
}

vec3 faceNormal() {
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 norm = normalize(cross(a, b));
	return -norm;
}

void draw(mat4 transform, vec3 center, vec3 normal) {
	float arrowHeight = 0.15 * ARROW_LENGTH;
	vec4 arrowTop = vec4(center + ARROW_LENGTH * normal, 1);
	vec3 norm = normalize(cross(arrowTop.xyz - viewPosition, normal));
	vec4 arrowLeft = arrowTop - vec4(arrowHeight * normal - ARROW_WIDTH * norm, 0);
	vec4 arrowRight = arrowTop - vec4(arrowHeight * normal + ARROW_WIDTH * norm, 0);
	vec4 arrowBase = arrowTop - arrowHeight * vec4(normal, 0);

	gl_Position = transform * vec4(center, 1); EmitVertex();
#ifdef ARROW_HEAD
	gl_Position = transform * arrowBase; EmitVertex();
	gl_Position = transform * arrowLeft; EmitVertex();
#endif
	gl_Position = transform * arrowTop; EmitVertex();
#ifdef ARROW_HEAD
	gl_Position = transform * arrowRight; EmitVertex();
	gl_Position = transform * arrowBase; EmitVertex();
#endif
	EndPrimitive();
}

void main() {
	mat4 transform = projectionMatrix * viewMatrix;

	switch (gl_InvocationID) {
		case 0: { TYPE_1 } break;
		case 1: { TYPE_2 } break;
		case 2: { TYPE_3 } break;
		case 3: { TYPE_4 } break;
		case 4: { TYPE_5 } break;
	}
}


//------------------------------------------------------------------------------//

[fragment]

in vec3 fColor;
out vec4 outColor;

void main() {
	outColor = vec4(fColor, 1.0);
}

