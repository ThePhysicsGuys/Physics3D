#include "core.h"
#include "toolbarFrame.h"

#include "../layer/pickerLayer.h"
#include "../engine/tool/toolManager.h"
#include "../graphics/gui/imgui/imguiExtension.h"
#include "../input/standardInputHandler.h"
#include "../engine/options/keyboardOptions.h"
#include "application.h"
#include "screen.h"
#include "picker/tools/toolSpacing.h"

namespace P3D::Application {

void ToolbarFrame::onInit(Engine::Registry64& registry) {
	std::string path = "../res/textures/icons/";
	ResourceManager::add<Graphics::TextureResource>("play", path + "Play.png");
	ResourceManager::add<Graphics::TextureResource>("pause", path + "Pause.png");
	ResourceManager::add<Graphics::TextureResource>("tick", path + "Tick.png");
	ResourceManager::add<Graphics::TextureResource>("reset", path + "Reset.png");
}

void ToolbarFrame::onRender(Engine::Registry64& registry) {
	ImGui::BeginToolbar("Toolbar");
	
	for (Engine::ToolManager& toolManager : PickerLayer::toolManagers) {
		for (Engine::Tool* tool : toolManager) {
			bool selected = toolManager.isSelected(tool);
			if (dynamic_cast<ToolSpacing*>(tool) != nullptr) {
				ImGui::SameLine();
				ImGui::BulletText("");
			} else if (ImGui::ToolBarButton(tool, selected)) {
				if (selected)
					toolManager.deselectTool();
				else
					toolManager.selectTool(tool);
			}
		}
	}

	ImGui::ToolBarSpacing();
	

	if (ImGui::ToolBarButton("Play / Pause", "Play / Pause the simulation", isPaused() ? "play" : "pause")) {
		Engine::KeyPressEvent event(Engine::KeyboardOptions::Tick::pause);
		handler->onEvent(event);
	}

	if (ImGui::ToolBarButton("Tick", "Run one tick of the simulation", "tick")) {
		Engine::KeyPressEvent event(Engine::KeyboardOptions::Tick::run);
		handler->onEvent(event);
	}


	ImGui::EndToolBar();
}
	
}


