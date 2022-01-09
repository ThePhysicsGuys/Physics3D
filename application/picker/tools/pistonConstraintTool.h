#pragma once

#include "engine/tool/buttonTool.h"
#include "graphics/glfwUtils.h"

namespace P3D::Application {

class PistonConstraintTool : public Engine::ButtonTool {
public:
	DEFINE_TOOL("Piston Constraint", "Select two entities to create a motor constraint beteen.", Graphics::GLFW::Cursor::CROSSHAIR);

	~PistonConstraintTool() override = default;

	void onRegister() override;
	void onDeregister() override;
	void onSelect() override;

	static void constrainSelection();
};

}
