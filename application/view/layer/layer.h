#pragma once

#include <string>

class Layer {
public:
	std::string name;

	Layer(const std::string& name = "Layer") { this->name = name; };
	~Layer() {}

	virtual void attach() {}
	virtual void detach() {}

	virtual void init() {}

	virtual void update() {}
	virtual void render() {}

	virtual void close() {}
};