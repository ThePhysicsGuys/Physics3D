#shader vertex
#version 330 core

layout (location = 0) in vec4 positions;

out vec2 fUV;

uniform mat4 projectionMatrix;

void main() {
    gl_Position = projectionMatrix * vec4(positions.xy, 0, 1);
    fUV = positions.zw;
}

///////////////////////////////////////////////////////////////////////////

#shader fragment
#version 330 core

in vec2 fUV;

out vec4 outColor;

uniform sampler2D depthMap;

float zfar = 1000.0f;
float znear = 0.1f;

float linearize(float depth) {
	return (-zfar * znear / (depth * (zfar - znear) - zfar)) / zfar;
}

void main() {             
    float depthValue = texture(depthMap, fUV).w;
    outColor = vec4(vec3(linearize(depthValue)), 1.0);
}  