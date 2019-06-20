#shader vertex
#version 410 core

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

///////////////////////////////////////////////////////////////////////////

#shader tesselation control
#version 410 core

// define the number of CPs in the output patch                                                 
layout(vertices = 3) out;

uniform vec3 viewPosition;

// attributes of the input CPs                                                                  
in vec3 tcposition[];
in vec3 tcnormal[];
in vec2 tcUV[];

// attributes of the output CPs                                                                 
out vec3 teposition[];
out vec3 tenormal[];
out vec2 teUV[];


float getTesselationLevel(float d1, float d2) {
	float avg = (d1 + d2) / 8.0;

	/*if (avg <= 1.0) {
		return 15.0;
	} else if (avg <= 2.0) {
		return 10.0;
	} else if (avg <= 3.0) {
		return 8.0;
	} else if (avg <= 4.0) {
		return 7.0;
	} else if (avg <= 5.0) {
		return 4.0;
	} else if (avg <= 6.0) {
		return 2.0;
	} else {
		return 1.0;
	}*/

	return clamp(15*(1 - avg / 4), 0, 30);
}

void main() {
	// Set the control points of the output patch                                               
	teposition[gl_InvocationID] = tcposition[gl_InvocationID];
	tenormal[gl_InvocationID] = tcnormal[gl_InvocationID];
	teUV[gl_InvocationID] = tcUV[gl_InvocationID];

	// Calculate the distance from the camera to the three control points                       
	float d0 = distance(viewPosition, teposition[0]);
	float d1 = distance(viewPosition, teposition[1]);
	float d2 = distance(viewPosition, teposition[2]);

	// Calculate the tessellation levels                                                        
	gl_TessLevelOuter[0] = getTesselationLevel(d1, d2);
	gl_TessLevelOuter[1] = getTesselationLevel(d2, d0);
	gl_TessLevelOuter[2] = getTesselationLevel(d0, d1);
	gl_TessLevelInner[0] = gl_TessLevelOuter[2];
}

///////////////////////////////////////////////////////////////////////////

#shader tesselation evaluate
#version 410 core

layout(triangles, fractional_even_spacing, ccw) in;

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

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2) {
	return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2) {
	return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

void main() {
	// Interpolate the attributes of the output vertex using the barycentric coordinates        
	fposition = interpolate3D(teposition[0], teposition[1], teposition[2]);
	fnormal = normalize(interpolate3D(tenormal[0], tenormal[1], tenormal[2]));
	fUV = interpolate2D(teUV[0], teUV[1], teUV[2]);

	float displacement = texture(displacementMap, fUV).x;
	fposition += fnormal * displacement * 2;
	gl_Position = projectionMatrix * viewMatrix * vec4(fposition, 1.0);
}

///////////////////////////////////////////////////////////////////////////

#shader fragment
#version 410 core

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