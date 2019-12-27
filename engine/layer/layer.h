#pragma once

#include "../event/event.h"

#include <string>

class Application::Screen;

class Layer {
protected:
	Screen* screen = nullptr;

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

	Layer() {};
	Layer(const std::string& name, Screen* screen, char flags = None) : name(name), screen(screen), flags(flags) {};
	~Layer() {}

	virtual void onAttach() {}
	virtual void onDetach() {}

	virtual void onInit() {}
	virtual void onEvent(Event& event) {}
	virtual void onUpdate() {}
	virtual void onRender() {}
	virtual void onClose() {}
};