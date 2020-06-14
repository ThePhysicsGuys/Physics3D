[common]

#version 330 core

[vertex]

layout(location = 0) in vec4 vPosition;

uniform mat4 projectionMatrix;

out vec2 fUV;

void main() {
	gl_Position = vec4(vPosition.xy, 0.0, 1.0);
	fUV = vPosition.zw;
}

[fragment]

out vec4 outColor;

in vec2 fUV;

uniform sampler2D depthMap;
uniform float near;
uniform float far;

float linearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main() {             
    float depthValue = texture(depthMap, fUV).r;
    outColor = vec4(vec3(linearizeDepth(depthValue) / far), 1.0); // perspective
    outColor = vec4(vec3(depthValue), 1.0); // orthographic
}