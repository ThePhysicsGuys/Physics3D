#include "core.h"

#include "ecsFrame.h"

#include "application.h"
#include "screen.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "ecs/components.h"
#include "../graphics/resource/textureResource.h"
#include "../util/resource/resourceManager.h"
#include "ecs/entityBuilder.h"
#include "input/standardInputHandler.h"
#include "Physics3D/hardconstraints/fixedConstraint.h"
#include "Physics3D/hardconstraints/motorConstraint.h"
#include "Physics3D/softlinks/elasticLink.h"
#include "Physics3D/softlinks/magneticLink.h"
#include "Physics3D/softlinks/springLink.h"
#include "picker/tools/attachmentTool.h"
#include "picker/tools/elasticLinkTool.h"
#include "picker/tools/fixedConstraintTool.h"
#include "picker/tools/magneticLinkTool.h"
#include "picker/tools/motorConstraintTool.h"
#include "picker/tools/selectionTool.h"
#include "picker/tools/springLinkTool.h"
#include "util/stringUtil.h"

namespace P3D::Application {

	void* selectedNode = nullptr;
	Engine::Registry64::entity_type selectedNodeEntity = 0;

	Graphics::TextureResource* folderIcon;
	Graphics::TextureResource* entityIcon;
	Graphics::TextureResource* colliderIcon;
	Graphics::TextureResource* attachmentsIcon;
	Graphics::TextureResource* hardConstraintsIcon;
	Graphics::TextureResource* softConstraintsIcon;
	Graphics::TextureResource* cframeIcon;

	static ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
	static ImGuiTreeNodeFlags leafFlags = baseFlags | ImGuiTreeNodeFlags_Leaf;

	template <typename OnPressed>
	static bool IconTreeNode(const char* label,
	                         ImGuiTreeNodeFlags flags,
	                         GLID icon,
	                         bool selected,
	                         const OnPressed& onPressed,
	                         Engine::Registry64::entity_type entity = 0) {
		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		ImU32 id = entity == 0 ? window->GetID(label) : window->GetID(entity);

		ImVec2 pos = window->DC.CursorPos;
		ImRect button(pos, ImVec2(pos.x + ImGui::GetContentRegionAvail().x, pos.y + g.FontSize + g.Style.FramePadding.y * 2));

		bool opened = ImGui::TreeNodeBehaviorIsOpen(id);
		bool leaf = (flags & ImGuiTreeNodeFlags_Leaf) != 0;

		bool hovered, held;
		if (ImGui::ButtonBehavior(button, id, &hovered, &held, ImGuiButtonFlags_PressedOnClickRelease)) {
			onPressed();

			if (!leaf)
				window->DC.StateStorage->SetInt(id, opened ? 0 : 1);
		}

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
		window->DrawList->AddImage(reinterpret_cast<ImTextureID>(icon),
		                           ImVec2(pos.x + arrowWidth, pos.y),
		                           ImVec2(pos.x + buttonSize + arrowWidth, pos.y + buttonSize),
		                           ImVec2(0, 1),
		                           ImVec2(1, 0));

		// Text
		ImVec2 textPos = ImVec2(pos.x + buttonSize + arrowWidth + g.Style.ItemInnerSpacing.x, pos.y + g.Style.FramePadding.y);
		ImGui::RenderText(textPos, label);

		// Arrow
		//ImVec2 padding = ImVec2(g.Style.FramePadding.x, ImMin(window->DC.CurrLineTextBaseOffset, g.Style.FramePadding.y));
		float arrowOffset = std::abs(g.FontSize - arrowWidth) / 2.0f;
		if (!leaf)
			ImGui::RenderArrow(window->DrawList,
			                   ImVec2(pos.x + arrowOffset, textPos.y),
			                   ImGui::GetColorU32(ImGuiCol_Text),
			                   opened ? ImGuiDir_Down : ImGuiDir_Right,
			                   1.0f);

		ImGui::ItemSize(button, g.Style.FramePadding.y);
		ImGui::ItemAdd(button, id);

		if (opened)
			ImGui::TreePush(label);

		return opened;
	}

	static bool EntityTreeNode(Engine::Registry64& registry,
	                           Engine::Registry64::entity_type entity,
	                           IRef<Comp::Collider> collider,
	                           ImGuiTreeNodeFlags flags,
	                           const char* label) {

		auto onPressed = [&]() {
			if (ImGui::GetIO().KeyCtrl)
				SelectionTool::toggle(entity);
			else {
				SelectionTool::selection.clear();
				SelectionTool::select(entity);
			}

			if (collider.valid())
				screen.selectedPart = collider->part;
		};

		GLID icon = collider.valid() ? colliderIcon->getID() : flags & ImGuiTreeNodeFlags_Leaf ? entityIcon->getID() : folderIcon->getID();
		bool selected = SelectionTool::selection.contains(entity);
		bool result = IconTreeNode(label, flags, icon, selected, onPressed, entity);

		return result;
	}

	void ECSFrame::renderEntity(Engine::Registry64& registry, const Engine::Registry64::entity_type& entity) {
		if (entity == selectedNodeEntity)
			return;

		auto children = registry.getChildren(entity);
		bool leaf = children.begin() == registry.end();
		auto collider = registry.get<Comp::Collider>(entity);
		bool terrain = collider.valid() && collider->part->parent == nullptr;
		std::string name = registry.getOr<Comp::Name>(entity, std::to_string(entity)).name;

		// Collect hard constraints
		std::vector<HardPhysicalConnection*> hardConstraints;
		if (!terrain && collider.valid()) {
			collider->part->parent->forEachHardConstraint([&hardConstraints](Physical& parent, ConnectedPhysical& child) {
				hardConstraints.push_back(&child.connectionToParent);
			});
		}

		// Collect soft links
		std::vector<SoftLink*> softLinks;
		if (!terrain && collider.valid()) {
			for (SoftLink* softLink : world.softLinks) {
				if (softLink->attachedPart1.part == collider->part || softLink->attachedPart2.part == collider->part)
					softLinks.push_back(softLink);
			}
		}

		// Collect attachments
		std::vector<Part*> attachments;
		if (!terrain && collider.valid()) {
			collider->part->parent->rigidBody.forEachAttachedPart([&attachments](Part& attachment) {
				attachments.push_back(&attachment);
			});
		}

		// Entity node
		bool expandable = !leaf || !hardConstraints.empty() || !softLinks.empty() || !attachments.empty();
		bool open = EntityTreeNode(registry, entity, collider, expandable ? baseFlags : leafFlags, name.c_str());

		// Render popup
		if (ImGui::BeginPopupContextItem()) {
			// Collapse / unfold
			if (!leaf) {
				if (open) {
					if (ImGui::BeginMenu("Collapse")) {
						if (ImGui::MenuItem("Node")) {}
						if (ImGui::MenuItem("Recursively")) {}
						ImGui::EndMenu();
					}
				} else {
					if (ImGui::MenuItem("Unfold node")) {
						if (ImGui::MenuItem("Node")) {}
						if (ImGui::MenuItem("Recursively")) {}
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
				if (ImGui::MenuItem("Entity")) {}
				if (ImGui::MenuItem("Folder")) {}
				ImGui::EndMenu();
			}

			// Delete
			if (ImGui::MenuItem("Delete")) {}

			ImGui::EndPopup();
		}

		// Render content
		if (open) {
			// Render attachments
			if (!attachments.empty()) {
				if (IconTreeNode("Attachments", baseFlags, attachmentsIcon->getID(), false, []() {})) {
					renderAttachments(registry, entity, *collider, attachments);

					ImGui::TreePop();
				}
			}

			// Render hard constraints
			if (!hardConstraints.empty()) {
				if (IconTreeNode("Hard Constraints", baseFlags, hardConstraintsIcon->getID(), false, []() {})) {
					renderHardConstraints(registry, entity, *collider, hardConstraints);
					ImGui::TreePop();
				}
			}

			// Render soft links
			if (!softLinks.empty()) {
				if (IconTreeNode("Soft Links", baseFlags, softConstraintsIcon->getID(), false, []() {})) {
					renderSoftLinks(registry, entity, *collider, softLinks);

					ImGui::TreePop();
				}
			}

			// Render children
			if (!leaf) {
				for (auto child : children)
					renderEntity(registry, child);
			}

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

	void ECSFrame::renderHardConstraints(Engine::Registry64& registry,
	                                     const Engine::Registry64::entity_type& entity,
	                                     const Comp::Collider& collider,
	                                     const std::vector<HardPhysicalConnection*>& hardConstraints) {

		auto onPressed = [&](HardPhysicalConnection* hardConstraint, const std::string& name) {
			SelectionTool::selection.remove(selectedNodeEntity);
			registry.destroy(selectedNodeEntity);
			selectedNodeEntity = Engine::Registry64::null_entity;

			if (selectedNode != hardConstraint) {
				selectedNode = hardConstraint;
				selectedNodeEntity = EntityBuilder(registry).name(name).hardConstraint(hardConstraint).get();

				SelectionTool::selection.clear();
				SelectionTool::select(selectedNodeEntity);
			} else {
				selectedNode = nullptr;
			}
		};

		for (std::size_t index = 0; index < hardConstraints.size(); index++) {
			HardPhysicalConnection* hardConstraint = hardConstraints[index];

			bool selected = selectedNode == hardConstraint;

			ImGui::PushID(hardConstraint);
			if (FixedConstraint* fixedConstraint = dynamic_cast<FixedConstraint*>(hardConstraint->constraintWithParent.get())) {
				std::string name = Util::format("Fixed constraint %d", index);
				IconTreeNode(name.c_str(),
				             leafFlags,
				             ResourceManager::get<Graphics::TextureResource>(FixedConstraintTool::getStaticName())->getID(),
				             selected,
				             [&]() {
					             onPressed(hardConstraint, name);
				             });
			} else if (ConstantSpeedMotorConstraint* motorConstraint = dynamic_cast<ConstantSpeedMotorConstraint*>(hardConstraint->
				constraintWithParent.get())) {
				std::string name = Util::format("Constant motor constraint %d", index);

				IconTreeNode(name.c_str(),
				             leafFlags,
				             ResourceManager::get<Graphics::TextureResource>(MotorConstraintTool::getStaticName())->getID(),
				             selected,
				             [&]() {
					             onPressed(hardConstraint, name);
				             });
			} else {
				IconTreeNode("Unknown hard constraint",
				             leafFlags,
				             ResourceManager::getDefaultResource<Graphics::TextureResource>()->getID(),
				             selected,
				             []() {});
			}
			ImGui::PopID();
		}
	}

	void ECSFrame::renderSoftLinks(Engine::Registry64& registry,
	                               const Engine::Registry64::entity_type& entity,
	                               const Comp::Collider& collider,
	                               const std::vector<SoftLink*>& softLinks) {

		auto onPressed = [&](SoftLink* softLink, const std::string& name) {
			SelectionTool::selection.remove(selectedNodeEntity);
			registry.destroy(selectedNodeEntity);
			selectedNodeEntity = Engine::Registry64::null_entity;

			if (selectedNode != softLink) {
				selectedNode = softLink;
				selectedNodeEntity = EntityBuilder(registry).name(name).softLink(softLink).get();

				SelectionTool::selection.clear();
				SelectionTool::select(selectedNodeEntity);
			} else {
				selectedNode = nullptr;
			}
		};

		for (std::size_t index = 0; index < softLinks.size(); index++) {
			SoftLink* softLink = softLinks[index];

			bool selected = selectedNode == softLink;

			ImGui::PushID(softLink);
			if (MagneticLink* magneticLink = dynamic_cast<MagneticLink*>(softLink)) {
				std::string name = Util::format("Magnetic link %d", index);
				IconTreeNode(name.c_str(),
				             leafFlags,
				             ResourceManager::get<Graphics::TextureResource>(MagneticLinkTool::getStaticName())->getID(),
				             selected,
				             [&]() {
					             onPressed(softLink, name);
				             });
			} else if (ElasticLink* elasticLink = dynamic_cast<ElasticLink*>(softLink)) {
				std::string name = Util::format("Elastic link %d", index);
				IconTreeNode(name.c_str(),
				             leafFlags,
				             ResourceManager::get<Graphics::TextureResource>(ElasticLinkTool::getStaticName())->getID(),
				             selected,
				             [&]() {
					             onPressed(softLink, name);
				             });
			} else if (SpringLink* springLink = dynamic_cast<SpringLink*>(softLink)) {
				std::string name = Util::format("Spring link %d", index);
				IconTreeNode(name.c_str(),
				             leafFlags,
				             ResourceManager::get<Graphics::TextureResource>(SpringLinkTool::getStaticName())->getID(),
				             selected,
				             [&]() {
					             onPressed(softLink, name);
				             });
			} else {
				IconTreeNode("Unknown soft link",
				             leafFlags,
				             ResourceManager::getDefaultResource<Graphics::TextureResource>()->getID(),
				             selected,
				             []() {});
			}
			ImGui::PopID();
		}
	}

	void ECSFrame::renderAttachments(Engine::Registry64& registry,
	                                 const Engine::Registry64::entity_type& entity,
	                                 const Comp::Collider& collider,
	                                 const std::vector<Part*>& attachments) {

		auto onPressed = [&](Part* attachment, const std::string& name) {
			SelectionTool::selection.remove(selectedNodeEntity);
			registry.destroy(selectedNodeEntity);
			selectedNodeEntity = Engine::Registry64::null_entity;

			if (selectedNode != attachment) {
				AttachedPart* part = &collider.part->parent->rigidBody.getAttachFor(attachment);

				selectedNode = attachment;
				selectedNodeEntity = EntityBuilder(registry).name(name).attachment(part).get();

				SelectionTool::selection.clear();
				SelectionTool::select(selectedNodeEntity);
			} else {
				selectedNode = nullptr;
			}
		};

		for (std::size_t index = 0; index < attachments.size(); index++) {
			Part* attachment = attachments[index];
			ImGui::PushID(attachment);

			bool selected = selectedNode == attachment;
			std::string name = Util::format("Attachment %d", index);

			IconTreeNode(name.c_str(),
			             leafFlags,
			             ResourceManager::get<Graphics::TextureResource>(AttachmentTool::getStaticName())->getID(),
			             selected,
			             [&]() {
				             onPressed(attachment, name);
			             });

			ImGui::PopID();
		}
	}

	void ECSFrame::onInit(Engine::Registry64& registry) {
		folderIcon = ResourceManager::add<Graphics::TextureResource>("folder", "../res/textures/icons/Folder.png");
		entityIcon = ResourceManager::add<Graphics::TextureResource>("entity", "../res/textures/icons/Entity.png");
		colliderIcon = ResourceManager::add<Graphics::TextureResource>("collider", "../res/textures/icons/Collider.png");
		hardConstraintsIcon = ResourceManager::add<Graphics::TextureResource>("hard constraints", "../res/textures/icons/Hard Constraints.png");
		softConstraintsIcon = ResourceManager::add<Graphics::TextureResource>("soft constraints", "../res/textures/icons/Soft Constraints.png");
		attachmentsIcon = ResourceManager::add<Graphics::TextureResource>("attachments", "../res/textures/icons/Attachments.png");
		cframeIcon = ResourceManager::add<Graphics::TextureResource>("cframe", "../res/textures/icons/Axes.png");
	}


	void ECSFrame::onRender(Engine::Registry64& registry) {
		using namespace Engine;

		ImGui::Begin("Tree");


		auto filter = [&registry](const Registry64::entity_set_iterator& iterator) {
			return registry.getParent(*iterator) == 0;
		};

		for (auto entity : registry.filter(filter))
			renderEntity(registry, registry.getSelf(entity));

		ImGui::End();
	}

}
