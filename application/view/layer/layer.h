#pragma once

#include <string>

class Screen;

class Layer {
public:
	Screen* screen = nullptr;
	std::string name;

	Layer() {};
	Layer(const std::string& name, Screen* screen) : name(name), screen(screen) {};
	~Layer() {}

	virtual void attach() {}
	virtual void detach() {}

	virtual void init() {}

	virtual void update() {}
	virtual void render() {}

	virtual void close() {}
};