[common]

#version 410 core

//------------------------------------------------------------------------------//

[vertex]

layout(location = 0) in vec3 vposition;
layout(location = 1) in vec3 vnormal;
layout(location = 2) in vec2 vUV;

uniform mat4 modelMatrix;

out vec3 tcposition;
out vec3 tcnormal;
out vec2 tcUV;

void main() {
	tcposition = (modelMatrix * vec4(vposition, 1.0)).xyz;
	tcnormal = (modelMatrix * vec4(vnormal, 0.0)).xyz;
	tcUV = vUV;
}

//------------------------------------------------------------------------------//

[tesselation control]

// define the number of CPs in the output patch                                                 
layout(vertices = 4) out;

uniform vec3 viewPosition;

// attributes of the input CPs                                                                  
in vec3 tcposition[];
in vec3 tcnormal[];
in vec2 tcUV[];

// attributes of the output CPs                                                                 
out vec3 teposition[];
out vec3 tenormal[];
out vec2 teUV[];

void main() {
	// Set the control points of the output patch                                               
	teposition[gl_InvocationID] = tcposition[gl_InvocationID];
	tenormal[gl_InvocationID] = tcnormal[gl_InvocationID];
	teUV[gl_InvocationID] = tcUV[gl_InvocationID];

	// Calculate the distance from the camera to the three control points 
	if (gl_InvocationID == 0) {
		gl_TessLevelInner[0] = 5.0;
		gl_TessLevelInner[1] = 5.0;
		gl_TessLevelOuter[0] = 5.0;
		gl_TessLevelOuter[1] = 5.0;
		gl_TessLevelOuter[2] = 5.0;
		gl_TessLevelOuter[3] = 5.0;
	}
}

//------------------------------------------------------------------------------//

[tesselation evaluate]

layout(quads, equal_spacing, ccw) in;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform sampler2D displacementMap;
//uniform float gDispFactor;

in vec3 teposition[];
in vec3 tenormal[];
in vec2 teUV[];

out vec3 fposition;
out vec3 fnormal;
out vec2 fUV;

vec2 interpolate(vec2 v0, vec2 v1, vec2 v2, vec2 v3) {
	// interpolate in horizontal direction between vert. 0 and 3
	vec2 p0 = mix(v0, v3, gl_TessCoord.x);
	// interpolate in horizontal direction between vert. 1 and 2
	vec2 p1 = mix(v1, v2, gl_TessCoord.x);

	// interpolate in vertical direction
	vec2 p = mix(p0, p1, gl_TessCoord.y);

	return p;
}

vec3 interpolate(vec3 v0, vec3 v1, vec3 v2, vec3 v3) {
	// interpolate in horizontal direction between vert. 0 and 3
	vec3 p0 = mix(v3, v0, gl_TessCoord.x);
	// interpolate in horizontal direction between vert. 1 and 2
	vec3 p1 = mix(v2, v1, gl_TessCoord.x);

	// interpolate in vertical direction
	vec3 p = mix(p0, p1, gl_TessCoord.y);

	return p;
}

void main() {
	// Interpolate the attributes of the output vertex using the barycentric coordinates        
	fposition = (interpolate(teposition[0], teposition[1], teposition[2], teposition[3]));
	fnormal = normalize(interpolate(tenormal[0], tenormal[1], tenormal[2], tenormal[3]));
	fUV = interpolate(teUV[0], teUV[1], teUV[2], teUV[3]);

	//fposition += fnormal * length(gl_TessCoord.xy);

	gl_Position = projectionMatrix * viewMatrix * vec4(fposition, 1.0);
}

//------------------------------------------------------------------------------//

[fragment]

in vec3 fposition;
in vec3 fnormal;
in vec2 fUV;

out vec4 outColor;

void main() {
	fposition;
	fnormal;
	fUV;

	vec3 color = vec3(1);
	outColor = vec4(color, 1);
}  