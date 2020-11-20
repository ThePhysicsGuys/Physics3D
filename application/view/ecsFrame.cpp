#include "core.h"

#include "ecsFrame.h"

#include "application.h"
#include "screen.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "ecs/components.h"
#include "../graphics/resource/textureResource.h"
#include "../util/resource/resourceManager.h"

namespace P3D::Application {

Graphics::TextureResource* openFolderIcon;
Graphics::TextureResource* closedFolderIcon;
Graphics::TextureResource* objectIcon;

bool IconTreeNode(Engine::Registry64& registry, Engine::Registry64::entity_type entity, ImGuiTreeNodeFlags flags, const char* label) {
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;

	ImU32 id = window->GetID(entity);
	ImVec2 pos = window->DC.CursorPos;
	ImRect button(pos, ImVec2(pos.x + ImGui::GetContentRegionAvail().x, pos.y + g.FontSize + g.Style.FramePadding.y * 2));

	bool opened = ImGui::TreeNodeBehaviorIsOpen(id);
	bool leaf = (flags & ImGuiTreeNodeFlags_Leaf) != 0;
	
	bool hovered, held;
	if (ImGui::ButtonBehavior(button, id, &hovered, &held, true)) {
		screen.selectedEntity = entity;
		
		Ref<Comp::Model> model = registry.get<Comp::Model>(entity);
		if (model.valid())
			screen.selectedPart = model->part;
		
		if (!leaf)
			window->DC.StateStorage->SetInt(id, opened ? 0 : 1);
	}

	bool selected = screen.selectedEntity == entity;
	ImU32 color = ImGui::GetColorU32(ImGuiCol_Button);
	if (held || selected)
		color = ImGui::GetColorU32(ImGuiCol_ButtonActive);
	else if (hovered)
		color = ImGui::GetColorU32(ImGuiCol_ButtonHovered);

	if (hovered || held || selected)
		window->DrawList->AddRectFilled(button.Min, button.Max, color);

	// Icon
	float arrowWidth = g.FontSize;
	float buttonSize = g.FontSize + g.Style.FramePadding.y * 2;
	ImTextureID texture = (ImTextureID) (leaf ? objectIcon : (opened ? openFolderIcon : closedFolderIcon))->getID();
	window->DrawList->AddImage(texture, ImVec2(pos.x + arrowWidth, pos.y), ImVec2(pos.x + buttonSize + arrowWidth, pos.y + buttonSize), ImVec2(1, 1), ImVec2(0, 0));

	// Text
	ImVec2 textPos = ImVec2(pos.x + buttonSize + arrowWidth + g.Style.ItemInnerSpacing.x, pos.y + g.Style.FramePadding.y);
	ImGui::RenderText(textPos, label);

	// Arrow
	ImVec2 padding = ImVec2(g.Style.FramePadding.x, ImMin(window->DC.CurrLineTextBaseOffset, g.Style.FramePadding.y));
	float arrowOffset = std::abs(g.FontSize - arrowWidth) / 2.0f;
	if (!leaf)
		ImGui::RenderArrow(window->DrawList, ImVec2(pos.x + arrowOffset, textPos.y), ImGui::GetColorU32(ImGuiCol_Text), opened ? ImGuiDir_Down : ImGuiDir_Right, 1.0f);

	ImGui::ItemSize(button, g.Style.FramePadding.y);
	ImGui::ItemAdd(button, id);

	if (opened)
		ImGui::TreePush(label);

	return opened;
}
	
void ECSFrame::renderEntity(Engine::Registry64& registry, const Engine::Registry64::entity_type& entity) {
	auto children = registry.getChildren(entity);
	bool leaf = children.begin() == registry.end();

	std::string name = registry.getOr<Comp::Name>(entity, Comp::Name(std::to_string(entity))).name;
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | (leaf ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_None);
	bool open = IconTreeNode(registry, entity, flags, name.c_str());

	// Render popup
	if (ImGui::BeginPopupContextItem()) {
		// Collapse / unfold
		if (!leaf) {
			if (open) {
				if (ImGui::BeginMenu("Collapse")) {
					if (ImGui::MenuItem("Node")) {

					}
					if (ImGui::MenuItem("Recursively")) {

					}
					ImGui::EndMenu();
				}
			} else {
				if (ImGui::MenuItem("Unfold node")) {
					if (ImGui::MenuItem("Node")) {

					}
					if (ImGui::MenuItem("Recursively")) {

					}
					ImGui::EndMenu();
				}
			}
		}

		// Rename
		if (ImGui::BeginMenu("Rename")) {
			char* buffer = new char[name.size() + 1];
			//strcpy(buffer, node->getName().c_str());
			ImGui::Text("Edit name:");
			ImGui::InputText("##edit", buffer, name.size() + 1);
			if (ImGui::Button("Apply")) {
				std::string newName(buffer);
				registry.get<Comp::Name>(entity)->name = newName;
			}
			ImGui::EndMenu();

			delete[] buffer;
		}

		// Add
		if (ImGui::BeginMenu("Add")) {
			if (ImGui::MenuItem("Entity")) {

			}
			if (ImGui::MenuItem("Folder")) {

			}
			ImGui::EndMenu();
		}

		// Delete
		if (ImGui::MenuItem("Delete")) {

		}

		ImGui::EndPopup();
	}

	// Render children
	if (!leaf && open) {
		for (auto child : children)
			renderEntity(registry, child);
		ImGui::TreePop();
	}

	/*if (ImGui::BeginDragDropSource()) {
		// check sizeof later
		ImGui::SetDragDropPayload("ECS_NODE", id, sizeof(id));

		float buttonSize = GImGui->FontSize + GImGui->Style.FramePadding.y * 2;

		ImGui::Image(texture, ImVec2(buttonSize, buttonSize));
		ImGui::SameLine();
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 4));
		ImGui::Text("Move %s", name.c_str());
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ECS_NODE")) {
			//Engine::Node* source = static_cast<Engine::Node*>(payload->Data);
		}
		ImGui::EndDragDropTarget();
	}*/
}

void ECSFrame::onInit(Engine::Registry64& registry) {
	closedFolderIcon = ResourceManager::add<Graphics::TextureResource>("folder_closed", "../res/textures/icons/folder.png");
	openFolderIcon = ResourceManager::add<Graphics::TextureResource>("folder_open", "../res/textures/icons/folder_open.png");
	objectIcon = ResourceManager::add<Graphics::TextureResource>("object", "../res/textures/icons/box.png");
}


void ECSFrame::onRender(Engine::Registry64& registry) {
	using namespace Engine;

	ImGui::Begin("Tree");

	auto filter = [&registry] (const Registry64::entity_set_iterator& iterator) {
		return registry.getParent(*iterator) == 0;
	};
	
	for (auto entity : registry.filter(filter))
		renderEntity(registry, registry.getSelf(entity));
	
	ImGui::End();
}
	
}
