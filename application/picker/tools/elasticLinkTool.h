#pragma once

#include "engine/tool/buttonTool.h"
#include "graphics/glfwUtils.h"

namespace P3D::Application {

class ElasticLinkTool : public Engine::ButtonTool {
public:
	DEFINE_TOOL("Elastic Link", "Select two entities to create an elastic link beteen.", Graphics::GLFW::Cursor::CROSSHAIR);

	~ElasticLinkTool() override = default;

	void onRegister() override;
	void onDeregister() override;
	void onSelect() override;

	static void linkSelection();
};

}
