#pragma once

#include "../engine/ecs/registry.h"
#include <Physics3D/math/linalg/vec.h>

namespace P3D::Application {
	
struct PropertiesFrame {

	static void onInit(Engine::Registry64& registry);
	static void onRender(Engine::Registry64& registry);

};
	
}
