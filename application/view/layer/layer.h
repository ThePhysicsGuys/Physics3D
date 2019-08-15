#pragma once

#include <string>

class Screen;

class Layer {
protected:
	Screen* screen = nullptr;
public:

	static enum {
		// No flags, update, render and events are called
		none = 0,

		// Whether the layer is disabled, no update, render or event calls
		disabled = 1 << 0,

		// Whether the layer is hidden, no render calls
		noRender = 1 << 1,

		// Whether the layer receives events
		noEvents = 1 << 2,

		// Whether the layer updates
		noUpdate = 1 << 3
	};

	std::string name;

	char flags;

	Layer() {};
	Layer(const std::string& name, Screen* screen, char flags = none) : name(name), screen(screen), flags(flags) {};
	~Layer() {}

	virtual void attach() {}
	virtual void detach() {}

	virtual void init() {}
	virtual void event() {}
	virtual void update() {}
	virtual void render() {}

	virtual void close() {}
};