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

	void onInit(Engine::Registry64& registry);
	void onUpdate(Engine::Registry64& registry);
	void onEvent(Engine::Registry64& registry, Engine::Event& event);
	void onRender(Engine::Registry64& registry);
	void onClose(Engine::Registry64& registry);

	std::vector<Layer*>::iterator begin();
	std::vector<Layer*>::iterator end();

	std::vector<Layer*>::reverse_iterator rbegin();
	std::vector<Layer*>::reverse_iterator rend();
};

};