#include "layerStack.h"

#include <algorithm>
#include "../util/log.h"

LayerStack::LayerStack() {
	insert = 0;
}

void LayerStack::pushLayer(Layer* layer) {
	stack.insert(stack.begin() + insert++, layer);
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

void LayerStack::init() {
	for (auto i = begin(); i != end(); ++i) {
		(*i)->init();
	}
}

void LayerStack::update() {
	for (auto i = begin(); i != end(); ++i) {
		Layer* layer = *i;
		
		if (layer->flags & (Layer::disabled | Layer::noUpdate))
			continue;

		layer->update();
	}
}

void LayerStack::event() {
	for (auto i = rbegin(); i != rend(); ++i) {
		Layer* layer = *i;

		if (layer->flags & (Layer::disabled | Layer::noEvents))
			continue;

		layer->event();
		// Check if event had 
	}
}

void LayerStack::render() {
	for (auto i = begin(); i != end(); ++i) {
		Layer* layer = *i;

		if (layer->flags & (Layer::disabled | Layer::noRender))
			continue;

		layer->render();
	}
}

void LayerStack::close() {
	for (auto i = rbegin(); i != rend(); ++i) {
		(*i)->close();
	}
}

std::vector<Layer*>::iterator LayerStack::begin() {
	return stack.begin();
}

std::vector<Layer*>::iterator LayerStack::end() {
	return stack.end();
}

std::vector<Layer*>::reverse_iterator LayerStack::rbegin() {
	return stack.rbegin();
}

std::vector<Layer*>::reverse_iterator LayerStack::rend() {
	return stack.rend();
}