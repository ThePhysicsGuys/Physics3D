#pragma once

#include <map>
#include <vector>
#include <initializer_list>

namespace Graphics {

template<typename T>
struct ValueCycle {
private:
	std::map<float, T> keyFrames;

public:
	ValueCycle() {}
	ValueCycle(std::initializer_list<std::pair<float, T>> keyFrames) : keyFrames(keyFrames) {}

	void addKeyFrame(float time, const T& frame) {
		keyFrames[time] = frame;
	}

	void deleteKeyFrame(float time) {
		keyFrames.erase(time);
	}

	T sample(float time) {
		std::pair<float, T> previous;
		std::pair<float, T> next;

		for (const auto& iterator : keyFrames) {
			if (iterator.first > time) {
				next = iterator;
				break;
			}
			previous = iterator;
		}

		float blend = (time - previous.first) / (next.first - previous.first);
		return (1.0f - blend) * previous.second + blend * next.second;
	}

	std::vector<float> getKeys() {
		std::vector<float> keys;
		keys.reserve(keyFrames.size());

		for (const auto& key : keyFrames)
			keys.push_back(key.first);
	}
};

}