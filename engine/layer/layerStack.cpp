#include "core.h"

#include "layerStack.h"

LayerStack::LayerStack() {
	insert = 0;
}

void LayerStack::pushLayer(Layer* layer) {
	stack.insert(stack.begin() + insert++, layer);
	layer->onAttach();
}

void LayerStack::popLayer(Layer* layer) {
	std::vector<Layer*>::iterator where = std::find(begin(), end(), layer);
		
	if (where != end()) {
		stack.erase(where);
		insert--;

		layer->onDetach();
	}
}

void LayerStack::pushOverlay(Layer* layer) {
	stack.emplace_back(layer);

	layer->onAttach();
}

void LayerStack::popOverlay(Layer * layer) {
	std::vector<Layer*>::iterator where = std::find(begin(), end(), layer);

	if (where != end()) {
		stack.erase(where);
		layer->onDetach();
	}	
}

void LayerStack::onInit() {
	for (auto i = begin(); i != end(); ++i) {
		(*i)->onInit();
	}
}

void LayerStack::onUpdate() {
	for (auto i = begin(); i != end(); ++i) {
		Layer* layer = *i;
		
		if (layer->flags & (Layer::Disabled | Layer::NoUpdate))
			continue;

		layer->onUpdate();
	}
}

void LayerStack::onEvent(Event& event) {
	for (auto i = rbegin(); i != rend(); ++i) {
		Layer* layer = *i;

		if (layer->flags & (Layer::Disabled | Layer::NoEvents))
			continue;

		layer->onEvent(event);
		
		if (event.handled)
			break;
	}
}

void LayerStack::onRender() {
	for (auto i = begin(); i != end(); ++i) {
		Layer* layer = *i;

		if (layer->flags & (Layer::Disabled | Layer::NoRender))
			continue;

		layer->onRender();
	}
}

void LayerStack::onClose() {
	for (auto i = rbegin(); i != rend(); ++i) {
		(*i)->onClose();
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