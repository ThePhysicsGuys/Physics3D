#include "core.h"
#include "toolbarFrame.h"

#include "../engine/tool/toolManager.h"
#include "../graphics/gui/imgui/imguiExtension.h"
#include "../input/standardInputHandler.h"
#include "../engine/options/keyboardOptions.h"
#include "application.h"
#include "screen.h"

namespace P3D::Application {

void ToolbarFrame::onInit(Engine::Registry64& registry) {
	std::string path = "../res/textures/icons/";
	ResourceManager::add<Graphics::TextureResource>("play", path + "play.png");
	ResourceManager::add<Graphics::TextureResource>("pause", path + "pause.png");
	ResourceManager::add<Graphics::TextureResource>("tick", path + "tick.png");
	ResourceManager::add<Graphics::TextureResource>("reset", path + "reset.png");
}

void ToolbarFrame::onRender(Engine::Registry64& registry) {
	ImGui::BeginToolbar("Toolbar");
	for (auto& [name, tool] : Engine::ToolManager::tools) {
		bool selected = Engine::ToolManager::isSelected(tool);
		if (ImGui::ToolBarButton(tool, selected)) {
			if (selected)
				Engine::ToolManager::deselectTool();
			else
				Engine::ToolManager::selectTool(tool);
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

	ImGui::ToolBarButton("Play / Pause", "Play / Pause the simulation", "object");
	ImGui::ToolBarButton("Play / Pause", "Play / Pause the simulation", "folder_open");
	ImGui::ToolBarButton("Play / Pause", "Play / Pause the simulation", "folder_closed");
	ImGui::ToolBarSpacing();
	ImGui::ToolBarButton("Play / Pause", "Play / Pause the simulation", "default");
	
	ImGui::EndToolBar();
}
	
}


