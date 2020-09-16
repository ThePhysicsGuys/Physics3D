#include "core.h"

#include "layerStack.h"

namespace P3D::Engine {

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

void LayerStack::onInit(Engine::Registry64& registry) {
	for (auto i = begin(); i != end(); ++i) {
		(*i)->onInit(registry);
	}
}

void LayerStack::onUpdate(Engine::Registry64& registry) {
	for (auto i = begin(); i != end(); ++i) {
		Layer* layer = *i;

		if (layer->flags & (Layer::Disabled | Layer::NoUpdate))
			continue;

		layer->onUpdate(registry);
	}
}

void LayerStack::onEvent(Engine::Registry64& registry, Engine::Event& event) {
	for (auto i = rbegin(); i != rend(); ++i) {
		Layer* layer = *i;

		if (layer->flags & (Layer::Disabled | Layer::NoEvents))
			continue;

		layer->onEvent(registry, event);

		if (event.handled)
			return;
	}
}

void LayerStack::onRender(Engine::Registry64& registry) {
	for (auto i = begin(); i != end(); ++i) {
		Layer* layer = *i;

		if (layer->flags & (Layer::Disabled | Layer::NoRender))
			continue;

		layer->onRender(registry);
	}
}

void LayerStack::onClose(Engine::Registry64& registry) {

	for (auto i = rbegin(); i != rend(); ++i) {
		(*i)->onClose(registry);
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

};