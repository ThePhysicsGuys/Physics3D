#include "layerStack.h"

#include <algorithm>

LayerStack::LayerStack() {
	insert = begin();
}

void LayerStack::pushLayer(Layer* layer) {
	insert = stack.emplace(insert, layer);

	layer->attach();
}

void LayerStack::popLayer(Layer* layer) {
	std::vector<Layer*>::iterator where = std::find(begin(), end(), layer);
		
	if (where != end()) {
		stack.erase(where);
		insert--;

		layer->detach();
	}
}

void LayerStack::pushOverlay(Layer* layer) {
	stack.emplace_back(layer);

	layer->attach();
}

void LayerStack::popOverlay(Layer * layer) {
	std::vector<Layer*>::iterator where = std::find(begin(), end(), layer);

	if (where != end()) {
		stack.erase(where);
		layer->detach();
	}	
}

std::vector<Layer*>::iterator LayerStack::begin() {
	return stack.begin();
}

std::vector<Layer*>::iterator LayerStack::end() {
	return stack.end();
}
