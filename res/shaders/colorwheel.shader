#shader vertex
#version 330 core

layout (location = 0) in vec4 vposition;

out vec2 fUV;

uniform mat4 projectionMatrix;

void main() {
	gl_Position = projectionMatrix * vec4(vposition.xy, 0.0, 1.0);
	fUV = 2 * vposition.zw - 1;
}

////////////////////////////////////////////////////////////////////////////::

#shader fragment
#version 330 core

#define PI 3.14159265359
#define TWOPI 6.28318530718

in vec2 fUV;

out vec4 outColor;

vec3 hsb_rgb(vec3 color) {
	const vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	vec3 p = abs(fract(color.xxx + K.xyz) * 6.0 - K.www);
	return color.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), color.y);
}

float dist(vec2 point) {
	return length(point);
}

float arg(vec2 point) {
	return atan(point.y, point.x) / TWOPI + 0.5;
}

void main() {
	float h = arg(fUV);
	float s = dist(fUV);
	float v = 1;
	float a = 1;

	if (s > 1) {
		v = 0;
		a = 0;
	}

	vec3 color = hsb_rgb(vec3(h, s, v));

	outColor = vec4(color, a);
}