#include "valueCycle.h"

namespace Util {

float linear(float t) {
	return t;
}

float smoothstep(float t) {
	return t * t * (3.0f - 2.0f * t);
}

float easeInQuad(float t) {
	return t * t;
}

float easeOutQuad(float t) {
	return t * (2.0f - t);
}

float easeInOutQuad(float t) {
	return (t < 0.5f) ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
}

float easeInCubic(float t) {
	return t * t * t;
}

float easeOutCubic(float t) {
	return (--t) * t * t + 1;
}

float easeInOuCubic(float t) {
	return (t < 0.5f) ? 4.0f * t * t * t : (t - 1.0f) * (2.0f * t - 2.0f) * (2.0f * t - 2.0f) + 1.0f;
}

};