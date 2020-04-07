#pragma once

#include <map>
#include <vector>
#include <initializer_list>

namespace Util {

float linear(float t);
float smoothstep(float t);
float easeInQuad(float t);
float easeOutQuad(float t);
float easeInOutQuad(float t);
float easeInCubic(float t);
float easeOutCubic(float t);
float easeInOuCubic(float t);

template<typename T, float (*interpolate)(float)>
struct ValueCycle {
private:
	std::map<float, T> keyframes;

public:
	ValueCycle() {}
	ValueCycle(std::initializer_list<std::pair<float, T>> keyframes) : keyframes(keyframes) {}

	/*
		Add a keyframe at the given time
	*/
	void addKeyframe(float time, const T& frame) {
		keyframes[time] = frame;
	}

	/*
		Delete the keyframe at the given time
	*/
	void deleteKeyframe(float time) {
		keyframes.erase(time);
	}

	/*
		Sample the keyframe, time should be between 0.0f and 1.0f
	*/
	T sample(float time) {
		std::pair<float, T> previous;
		std::pair<float, T> next;

		for (const auto& iterator : keyframes) {
			if (iterator.first > time) {
				next = iterator;
				break;
			}
			previous = iterator;
		}

		float blend = interpolate((time - previous.first) / (next.first - previous.first));
		return (1.0f - blend) * previous.second + blend * next.second;
	}

	/*
		Get all timestamps
	*/
	std::vector<float> getTimestamps() {
		std::vector<float> timestamps;
		timestamps.reserve(keyframes.size());

		for (const auto& timestamp : keyframes)
			timestamps.push_back(timestamp.first);
	}
};

}