#pragma once

#include "engine/tool/buttonTool.h"
#include "graphics/glfwUtils.h"

namespace P3D::Application {

class MotorConstraintTool : public Engine::ButtonTool {
public:
	DEFINE_TOOL("Motor Constraint", "Select two entities to create a motor constraint beteen.", Graphics::GLFW::Cursor::CROSSHAIR);

	~MotorConstraintTool() override = default;

	void onRegister() override;
	void onDeregister() override;
	void onSelect() override;

	static void constrainSelection();
};

}
