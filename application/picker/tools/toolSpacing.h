#pragma once
#include "engine/tool/tool.h"
#include "graphics/glfwUtils.h"

namespace P3D::Application {
	
	class ToolSpacing : public Engine::Tool {
	public:
		DEFINE_TOOL("Spacing", "", Graphics::GLFW::Cursor::CROSSHAIR);
	};
}
