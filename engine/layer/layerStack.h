#pragma once

#include "layer.h"
#include "../engine/ecs/registry.h"

namespace P3D::Engine {

class LayerStack {
private:
	std::vector<Layer*> stack;
	int insert;

public:
	LayerStack();

	void pushLayer(Layer* layer);
	void popLayer(Layer* layer);

	void onInit(Registry64& registry);
	void onUpdate(Registry64& registry);
	void onEvent(Registry64& registry, Event& event);
	void onRender(Registry64& registry);
	void onClose(Registry64& registry);

	std::vector<Layer*>::iterator begin();
	std::vector<Layer*>::iterator end();

	std::vector<Layer*>::reverse_iterator rbegin();
	std::vector<Layer*>::reverse_iterator rend();
};

};