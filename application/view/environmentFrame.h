#pragma once

#include "../engine/ecs/registry.h"
#include "../graphics/gui/color.h"

namespace P3D::Application {
	
struct EnvironmentFrame {

	static float hdr;
	static float gamma;
	static float exposure;
	static Color3 sunColor;

	static void onInit(Engine::Registry64& registry);
	static void onRender(Engine::Registry64& registry);

};
	
}
