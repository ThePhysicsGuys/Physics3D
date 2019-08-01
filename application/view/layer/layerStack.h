#pragma once

#include "layer.h"

#include <vector>

class LayerStack {
private:
	std::vector<Layer*> stack;
	std::vector<Layer*>::iterator insert;

public:
	LayerStack();

	void pushLayer(Layer* layer);
	void popLayer(Layer* layer);
	void pushOverlay(Layer* layer);
	void popOverlay(Layer* layer);

	std::vector<Layer*>::iterator begin();
	std::vector<Layer*>::iterator end();
};