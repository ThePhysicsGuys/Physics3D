#pragma once

#include "../event/event.h"

#include <string>

class Layer {
protected:
	void* ptr = nullptr;

public:

	enum LayerFlags : char {
		// No flags, update, render and events are called
		None =	   0 << 0,

		// Whether the layer is disabled, no update, render or event calls
		Disabled = 1 << 0,

		// Whether the layer is hidden, no render calls
		NoRender = 1 << 1,

		// Whether the layer receives events
		NoEvents = 1 << 2,

		// Whether the layer updates
		NoUpdate = 1 << 3
	};

	std::string name;

	char flags;

	inline Layer() : name(""), ptr(nullptr), flags(None) {};
	inline Layer(const std::string& name, void* ptr, char flags = None) : name(name), ptr(ptr), flags(flags) {};
	inline ~Layer() {}

	inline virtual void onAttach() {}
	inline virtual void onDetach() {}
	inline virtual void onInit() {}
	inline virtual void onEvent(Engine::Event& event) {}
	inline virtual void onUpdate() {}
	inline virtual void onRender() {}
	inline virtual void onClose() {}
};