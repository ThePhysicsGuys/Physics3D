#pragma once

#include "../engine/ecs/registry.h"

namespace P3D::Engine {
	class Layer;
}

namespace P3D::Application {
	
struct LayerFrame {

	static bool doEvents;
	static bool noRender;
	static bool doUpdate;
	static bool isDisabled;
	static Engine::Layer* selectedLayer;

	static void onInit(Engine::Registry64& registry);
	static void onRender(Engine::Registry64& registry);

};
	
}
