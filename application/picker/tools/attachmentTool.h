#pragma once
#include "engine/tool/buttonTool.h"
#include "graphics/glfwUtils.h"

namespace P3D::Application {

class AttachmentTool : public Engine::ButtonTool {
public:
	DEFINE_TOOL("Attachment", "Select two entities to create an attachment beteen.", Graphics::GLFW::Cursor::CROSSHAIR);

	~AttachmentTool() override = default;

	void onRegister() override;
	void onDeregister() override;
	void onSelect() override;

	static void attachSelection();
};
	
}
