#pragma once

#include "../engine/ecs/registry.h"

namespace P3D::Application {
	
struct ECSFrame {
private:
	static void renderEntity(Engine::Registry64& registry, Engine::Registry64::entity_type entity);
	
public:
	static void onInit(Engine::Registry64& registry);
	static void onRender(Engine::Registry64& registry);

};
	
}
