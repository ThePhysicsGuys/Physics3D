#pragma once

#include "engine/tool/buttonTool.h"
#include "graphics/glfwUtils.h"

namespace P3D::Application {

class FixedConstraintTool : public Engine::ButtonTool {
public:
	DEFINE_TOOL("Fixed Constraint", "Select two entities to create a fixed constraints beteen.", Graphics::GLFW::Cursor::CROSSHAIR);

	~FixedConstraintTool() override = default;

	void onRegister() override;
	void onDeregister() override;
	void onSelect() override;

	static void fixSelection();
};

}
