#include "core.h"
#include "layerFrame.h"

#include "screen.h"
#include "application.h"
#include "imgui/imgui.h"

namespace P3D::Application {

bool LayerFrame::doEvents;
bool LayerFrame::noRender;
bool LayerFrame::doUpdate;
bool LayerFrame::isDisabled;
Engine::Layer* LayerFrame::selectedLayer = nullptr;
	
void LayerFrame::onInit(Engine::Registry64& registry) {
	
}

void LayerFrame::onRender(Engine::Registry64& registry) {
	ImGui::Begin("Layers");
	
	using namespace Engine;

	ImGui::Columns(2, 0, true);
	ImGui::Separator();
	for (Layer* layer : screen.layerStack) {
		if (ImGui::Selectable(layer->name.c_str(), selectedLayer == layer))
			selectedLayer = layer;
	}

	ImGui::NextColumn();

	ImGui::Text("Name: %s", (selectedLayer) ? selectedLayer->name.c_str() : "-");
	if (selectedLayer) {
		ImGui::SetNextTreeNodeOpen(true);
		if (ImGui::TreeNode("Flags")) {
			char& flags = selectedLayer->flags;

			doEvents = !(flags & Layer::NoEvents);
			doUpdate = !(flags & Layer::NoUpdate);
			noRender = !(flags & Layer::NoRender);
			isDisabled = flags & Layer::Disabled;

			ImGui::Checkbox("Disabled", &isDisabled);
			ImGui::Checkbox("Events", &doEvents);
			ImGui::Checkbox("Update", &doUpdate);
			ImGui::Checkbox("Render", &noRender);

			flags = (isDisabled) ? flags | Layer::Disabled : flags & ~Layer::Disabled;
			flags = (doEvents) ? flags & ~Layer::NoEvents : flags | Layer::NoEvents;
			flags = (doUpdate) ? flags & ~Layer::NoUpdate : flags | Layer::NoUpdate;
			flags = (noRender) ? flags & ~Layer::NoRender : flags | Layer::NoRender;

			ImGui::TreePop();
		}
	}

	ImGui::Columns(1);
	ImGui::Separator();

	ImGui::End();
}

	
}
