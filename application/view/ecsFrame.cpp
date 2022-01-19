#include "core.h"

#include "ecsFrame.h"

#define IMGUI_DEFINE_MATH_OPERATORS

#include "application.h"
#include "screen.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "ecs/components.h"
#include "../graphics/resource/textureResource.h"
#include "../graphics/meshRegistry.h"
#include "../util/resource/resourceManager.h"
#include "Physics3D/hardconstraints/fixedConstraint.h"
#include "Physics3D/hardconstraints/motorConstraint.h"
#include "Physics3D/softlinks/elasticLink.h"
#include "Physics3D/softlinks/magneticLink.h"
#include "Physics3D/softlinks/springLink.h"
#include "picker/tools/alignmentLinkTool.h"
#include "picker/tools/attachmentTool.h"
#include "picker/tools/elasticLinkTool.h"
#include "picker/tools/fixedConstraintTool.h"
#include "picker/tools/magneticLinkTool.h"
#include "picker/tools/motorConstraintTool.h"
#include "picker/tools/selectionTool.h"
#include "picker/tools/springLinkTool.h"
#include "util/stringUtil.h"

namespace P3D::Application {

Graphics::TextureResource* folderIcon;
Graphics::TextureResource* openFolderIcon;
Graphics::TextureResource* entityIcon;
Graphics::TextureResource* colliderIcon;
Graphics::TextureResource* terrainIcon;
Graphics::TextureResource* mainColliderIcon;
Graphics::TextureResource* childColliderIcon;
Graphics::TextureResource* attachmentsIcon;
Graphics::TextureResource* hardConstraintsIcon;
Graphics::TextureResource* softLinksIcon;
Graphics::TextureResource* cframeIcon;
Graphics::TextureResource* cubeClassIcon;
Graphics::TextureResource* sphereClassIcon;
Graphics::TextureResource* cylinderClassIcon;
Graphics::TextureResource* cornerClassIcon;
Graphics::TextureResource* wedgeClassIcon;
Graphics::TextureResource* polygonClassIcon;
Graphics::TextureResource* shownIcon;
Graphics::TextureResource* hiddenIcon;
Graphics::TextureResource* physicsIcon;
Graphics::TextureResource* addIcon;

static int nodeIndex = 0;
static void* selectedNode = nullptr;
static Engine::Registry64::entity_type selectedNodeEntity = Engine::Registry64::null_entity;
static ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
static ImGuiTreeNodeFlags leafFlags = baseFlags | ImGuiTreeNodeFlags_Leaf;

static GLID getColliderIcon(IRef<Comp::Collider> collider) {
	if (collider.invalid())
		return -1;

	IRef<const ShapeClass> shape = collider->part->hitbox.baseShape;
	if (shape.get() == &CubeClass::instance)
		return cubeClassIcon->getID();
	if (shape.get() == &SphereClass::instance)
		return sphereClassIcon->getID();
	if (shape.get() == &CornerClass::instance)
		return cornerClassIcon->getID();
	if (shape.get() == &WedgeClass::instance)
		return wedgeClassIcon->getID();
	if (shape.get() == &CylinderClass::instance)
		return cylinderClassIcon->getID();

	return polygonClassIcon->getID();
}

static GLID getMeshIcon(IRef<Comp::Mesh> mesh) {
	using namespace Graphics::MeshRegistry;

	if (mesh.invalid())
		return -1;

	int shape = mesh->id;
	if (shape == getOrCreateMeshFor(&CubeClass::instance).id)
		return cubeClassIcon->getID();
	if (shape == getOrCreateMeshFor(&SphereClass::instance).id)
		return sphereClassIcon->getID();
	if (shape == getOrCreateMeshFor(&CornerClass::instance).id)
		return cornerClassIcon->getID();
	if (shape == getOrCreateMeshFor(&WedgeClass::instance).id)
		return wedgeClassIcon->getID();
	if (shape == getOrCreateMeshFor(&CylinderClass::instance).id)
		return cylinderClassIcon->getID();

	return polygonClassIcon->getID();
}

// Draw buttons
static void drawButton(const ImRect& button, bool even, bool selected, bool held, bool hovered) {
	ImU32 color = even ? ImGui::GetColorU32(ImVec4(0, 0, 0, 0)) : ImGui::GetColorU32(ImVec4(0.4f, 0.4f, 0.4f, 0.1f));
	if (held || selected)
		color = ImGui::GetColorU32(ImGuiCol_ButtonActive);
	else if (hovered)
		color = ImGui::GetColorU32(ImGuiCol_ButtonHovered);

	GImGui->CurrentWindow->DrawList->AddRectFilled(button.Min - ImVec2(0, GImGui->Style.ItemSpacing.y / 2), button.Max + ImVec2(0, GImGui->Style.ItemSpacing.y / 2), color);
};

// Draw icons
static void drawIcon(GLID icon, const ImVec2& min, const ImVec2& max) {
	if (icon == 0)
		return;

	GImGui->CurrentWindow->DrawList->AddImage(reinterpret_cast<ImTextureID>(icon), min, max, ImVec2(0, 1), ImVec2(1, 0));
}

static void drawLineBetween(const ImRect& button1, const ImRect& button2, float yExtent = 0) {
	ImVec2 pos1 = ImVec2(
		button1.Max.x + (button2.Min.x - button1.Max.x) / 2,
		ImMin(button1.Min.y, button2.Min.y) - yExtent / 2
	);
	ImVec2 pos2 = ImVec2(
		pos1.x,
		ImMax(button1.Max.y, button2.Max.y) + yExtent / 2
	);

	GImGui->CurrentWindow->DrawList->AddLine(pos1, pos2, ImGui::GetColorU32(ImVec4(0.7f, 0.7f, 0.7f, 0.3f)));
}

static void drawLine(const ImVec2& pos1, const ImVec2& pos2) {
	GImGui->CurrentWindow->DrawList->AddLine(pos1, pos2, ImGui::GetColorU32(ImVec4(0.7f, 0.7f, 0.7f, 0.3f)));
}

static void HeaderNode() {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return;

	ImGuiContext& g = *GImGui;

	float arrowWidth = g.FontSize;
	float buttonSize = g.FontSize + g.Style.FramePadding.y * 2;
	float buttonMargin = g.Style.ItemInnerSpacing.x;
	float buttonPadding = 5.0;

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 startPos = ImVec2(pos.x - window->DC.Indent.x + g.Style.WindowPadding.x, pos.y);
	ImVec2 endPos = ImVec2(startPos.x + ImGui::GetContentRegionMax().x, startPos.y + buttonSize);
	ImRect visibilityButton(startPos, ImVec2(startPos.x + buttonSize - buttonPadding * 2, startPos.y + buttonSize));
	ImRect arrowButton(ImVec2(pos.x + visibilityButton.GetWidth() + buttonMargin, pos.y), ImVec2(pos.x + visibilityButton.GetWidth() + buttonMargin + arrowWidth, pos.y + buttonSize));
	ImRect iconButton(ImVec2(arrowButton.Max.x + buttonMargin, pos.y), ImVec2(arrowButton.Max.x + buttonMargin + buttonSize, pos.y + buttonSize));
	ImRect colliderButton(ImVec2(iconButton.Max.x + buttonMargin, pos.y), ImVec2(iconButton.Max.x + buttonMargin + buttonSize - buttonPadding * 2, pos.y + buttonSize));
	ImRect mainButton(ImVec2(colliderButton.Max.x, pos.y), endPos);
	ImVec2 textPos = ImVec2(mainButton.Min.x + buttonMargin * 3, pos.y + g.Style.FramePadding.y);
	ImRect totalSize = ImRect(startPos, endPos);

	// Background
	window->DrawList->AddRectFilled(totalSize.Min - ImVec2(0, GImGui->Style.ItemSpacing.y / 2), totalSize.Max + ImVec2(0, GImGui->Style.ItemSpacing.y / 2), ImGui::GetColorU32(ImGuiCol_TabUnfocusedActive));

	// Icons
	drawIcon(shownIcon->getID(), visibilityButton.Min + ImVec2(0, buttonPadding), visibilityButton.Max - ImVec2(0, buttonPadding));
	drawIcon(entityIcon->getID(), iconButton.Min + ImVec2(buttonPadding, buttonPadding), iconButton.Max - ImVec2(buttonPadding, buttonPadding));
	drawIcon(physicsIcon->getID(), colliderButton.Min + ImVec2(0, buttonPadding), colliderButton.Max - ImVec2(0, buttonPadding));

	// Lines
	drawLineBetween(visibilityButton, arrowButton, g.Style.ItemSpacing.y);
	drawLineBetween(iconButton, colliderButton, g.Style.ItemSpacing.y);
	drawLineBetween(colliderButton, mainButton, g.Style.ItemSpacing.y);

	// Text
	ImGui::RenderText(textPos, "Name");

	ImGui::ItemSize(totalSize, g.Style.FramePadding.y);
	ImGui::ItemAdd(totalSize, 0);
}

template <typename OnPressed>
static bool IconTreeNode(ImU32 id, Engine::Registry64& registry, const char* label, ImGuiTreeNodeFlags flags, GLID mainIcon, bool selected, const OnPressed& onPressed, Engine::Registry64::entity_type entity = 0) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	// ID
	ImGuiContext& g = *GImGui;
	ImU32 mainId = id == 0 ? window->GetID(label) : id;
	ImU32 arrowId = window->GetID("Arrow");
	ImU32 colliderId = window->GetID("Collider");
	ImU32 meshId = window->GetID("Mesh");

	// Constants
	float arrowWidth = g.FontSize;
	float buttonSize = g.FontSize + g.Style.FramePadding.y * 2;
	float buttonMargin = g.Style.ItemInnerSpacing.x;
	float arrowOffset = std::abs(g.FontSize - arrowWidth) / 2.0f;
	float buttonPadding = 5.0;

	// Positions
	ImVec2 pos = window->DC.CursorPos;
	ImVec2 startPos = ImVec2(pos.x - window->DC.Indent.x + g.Style.WindowPadding.x, pos.y);
	ImVec2 endPos = ImVec2(startPos.x + ImGui::GetContentRegionMax().x, startPos.y + buttonSize);
	ImRect visibilityButton(startPos, ImVec2(startPos.x + buttonSize - buttonPadding * 2, startPos.y + buttonSize));
	ImRect arrowButton(ImVec2(pos.x + visibilityButton.GetWidth() + buttonMargin, pos.y), ImVec2(pos.x + visibilityButton.GetWidth() + buttonMargin + arrowWidth, pos.y + buttonSize));
	ImRect iconButton(ImVec2(arrowButton.Max.x + buttonMargin, pos.y), ImVec2(arrowButton.Max.x + buttonMargin + buttonSize, pos.y + buttonSize));
	ImRect colliderButton(ImVec2(iconButton.Max.x + buttonMargin, pos.y), ImVec2(iconButton.Max.x + buttonMargin + buttonSize - buttonPadding * 2, pos.y + buttonSize));
	ImRect mainButton(ImVec2(colliderButton.Max.x, pos.y), endPos);
	ImVec2 textPos = ImVec2(mainButton.Min.x + buttonMargin * 3, pos.y + g.Style.FramePadding.y);
	ImRect totalSize = ImRect(startPos, endPos);

	// General
	bool opened = ImGui::TreeNodeBehaviorIsOpen(mainId);
	bool leaf = (flags & ImGuiTreeNodeFlags_Leaf) != 0;
	bool even = nodeIndex++ % 2 == 0;

	// Get entity components
	auto mesh = registry.get<Comp::Mesh>(entity);
	auto collider = registry.get<Comp::Collider>(entity);
	bool visible = mesh.valid() && mesh->mode == static_cast<int>(Graphics::Renderer::FILL);

	// Main button
	bool mainHovered, mainHeld;
	if (ImGui::ButtonBehavior(mainButton, mainId, &mainHovered, &mainHeld, ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick)) {
		if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
			if (!leaf)
				window->DC.StateStorage->SetInt(mainId, opened ? 0 : 1);
		} else
			onPressed();
	}
	drawButton(totalSize, even, selected, mainHeld, mainHovered);
	if (mainIcon == 0) {
		if (mesh.invalid())
			mainIcon = leaf ? entityIcon->getID() : opened ? openFolderIcon->getID() : folderIcon->getID();
		else
			mainIcon = getMeshIcon(mesh);
	}
	drawIcon(mainIcon, iconButton.Min, iconButton.Max);

	// Visibility Button
	if (mesh.valid()) {
		bool visibilityHovered, visibilityHeld;
		if (ImGui::ButtonBehavior(visibilityButton, meshId, &visibilityHovered, &visibilityHeld, ImGuiButtonFlags_PressedOnClickRelease)) {
			if (entity != Engine::Registry64::null_entity)
				if (mesh.valid())
					mesh->mode = static_cast<GLFLAG>(mesh->mode) == Graphics::Renderer::FILL ? Graphics::Renderer::WIREFRAME : Graphics::Renderer::FILL;
		}
		drawButton(visibilityButton, true, selected, visibilityHeld, false);
		if (!visible)
			drawIcon(hiddenIcon->getID(), visibilityButton.Min + ImVec2(0, buttonPadding), visibilityButton.Max - ImVec2(0, buttonPadding));
		else if (visibilityHovered)
			drawIcon(shownIcon->getID(), visibilityButton.Min + ImVec2(0, buttonPadding), visibilityButton.Max - ImVec2(0, buttonPadding));
	}

	// Arrow button
	if (!leaf) {
		bool arrowHovered, arrowHeld;
		if (ImGui::ButtonBehavior(arrowButton, arrowId, &arrowHovered, &arrowHeld, ImGuiButtonFlags_PressedOnClickRelease)) {
			if (!leaf)
				window->DC.StateStorage->SetInt(mainId, opened ? 0 : 1);
		}
		drawButton(arrowButton, true, selected, arrowHeld, false);
		ImGui::RenderArrow(window->DrawList, ImVec2(arrowButton.Min.x + arrowOffset, textPos.y), ImGui::GetColorU32(ImGuiCol_Text), opened ? ImGuiDir_Down : ImGuiDir_Right, 1.0f);
	}

	// Collider button
	if (collider.valid()) {
		bool colliderHovered, colliderHeld;
		if (ImGui::ButtonBehavior(colliderButton, colliderId, &colliderHovered, &colliderHeld, ImGuiButtonFlags_PressedOnClickRelease)) {
			if (entity != Engine::Registry64::null_entity) {
				if (collider.valid()) {
					std::unique_lock<UpgradeableMutex> lock(worldMutex);
					if (collider->part->isTerrainPart()) {
						world.removePart(collider->part);
						world.addPart(collider->part);
					} else {
						world.removePart(collider->part);
						world.addTerrainPart(collider->part);
					}
				}
			}
		}
		drawButton(colliderButton, true, selected, colliderHeld, false);
		std::string text;
		if (collider->part->isTerrainPart()) {
			text = "T";
		} else if (!collider->part->parent->rigidBody.parts.empty()) {
			if (collider->part->isMainPart())
				text = "M";
			else
				text = "C";
		}

		ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
		ImGui::RenderText(ImVec2(colliderButton.Min.x + (buttonSize - buttonPadding * 2 - textSize.x) / 2, textPos.y), text.c_str());
	}

	// Lines
	drawLine(
		ImVec2(visibilityButton.Max.x + buttonMargin / 2, visibilityButton.Min.y - g.Style.ItemSpacing.y / 2), 
		ImVec2(visibilityButton.Max.x + buttonMargin / 2, visibilityButton.Max.y + g.Style.ItemSpacing.y / 2)
	);

	// Text
	ImGui::RenderText(textPos, label);

	ImGui::ItemSize(totalSize, g.Style.FramePadding.y);
	ImGui::ItemAdd(totalSize, mainId);

	if (opened)
		ImGui::TreePush(label);

	return opened;
}

static bool EntityTreeNode(ImU32 id, Engine::Registry64& registry,
                           Engine::Registry64::entity_type entity,
                           IRef<Comp::Collider> collider,
                           ImGuiTreeNodeFlags flags,
                           const char* label,
                           GLID icon) {

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

	bool selected = SelectionTool::selection.contains(entity);

	bool result = IconTreeNode(id, registry, label, flags, icon, selected, onPressed, entity);

	return result;
}

static bool ColliderMenuItem(const char* label, GLID icon, bool selected = false, bool enabled = true) {
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	ImVec2 pos = window->DC.CursorPos;
	ImVec2 labelSize = ImGui::CalcTextSize(label, nullptr, true);

	ImGuiSelectableFlags flags = ImGuiSelectableFlags_SelectOnRelease | ImGuiSelectableFlags_SetNavIdOnHover | (enabled ? 0 : ImGuiSelectableFlags_Disabled);

	float minWidth = window->DC.MenuColumns.DeclColumns(labelSize.x, 0, IM_FLOOR(g.FontSize * 1.20f)); // Feedback for next frame
	float extraWidth = ImMax(0.0f, ImGui::GetContentRegionAvail().x - minWidth);

	if (selected)
		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_TabActive));
	bool pressed = ImGui::Selectable(label, false, flags | ImGuiSelectableFlags_SpanAvailWidth, ImVec2(minWidth, 0.0f));
	if (selected)
		ImGui::PopStyleColor(1);

	ImVec2 start = pos + ImVec2(window->DC.MenuColumns.Pos[2] + extraWidth, 0);
	ImVec2 end = start + ImVec2(labelSize.y, labelSize.y);
	drawIcon(icon, start, end);

	return pressed;
}

void ECSFrame::renderEntity(Engine::Registry64& registry, const Engine::Registry64::entity_type& entity) {
	if (entity == selectedNodeEntity)
		return;

	ImGui::PushID(static_cast<int>(entity));

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
			if (softLink->attachedPartA.part == collider->part || softLink->attachedPartB.part == collider->part)
				softLinks.push_back(softLink);
		}
	}

	// Collect attachments
	std::vector<std::pair<Part*, Part*>> attachments;
	if (!terrain && collider.valid()) {
		RigidBody* rigidBody = &collider->part->parent->rigidBody;
		if (collider->part->isMainPart()) {
			rigidBody->forEachAttachedPart([&attachments, &collider, &rigidBody] (Part& attachment) {
				if (collider->part != &attachment)
					attachments.emplace_back(rigidBody->mainPart, &attachment);
			});
		} else {
			attachments.emplace_back(collider->part, rigidBody->mainPart);
		}
	}

	// Entity node
	ImU32 id = GImGui->CurrentWindow->GetID(name.c_str());
	bool expandable = !leaf || !hardConstraints.empty() || !softLinks.empty() || !attachments.empty();
	ImGuiTreeNodeFlags flags = expandable ? baseFlags : leafFlags;
	bool open = EntityTreeNode(id, registry, entity, collider, flags, name.c_str(), 0);

	// Render popup
	if (ImGui::BeginPopupContextItem()) {
		if (collider.valid()) {
			if (ImGui::BeginMenu("Change collider shape")) {
				std::unique_lock<UpgradeableMutex> lock(worldMutex);

				DiagonalMat3 scale = collider->part->hitbox.scale;
				const ShapeClass* shape = collider->part->getShape().baseShape.get();

				if (ColliderMenuItem("Box", cubeClassIcon->getID(), shape == &CubeClass::instance))
					collider->part->setShape(boxShape(scale[0], scale[1], scale[2]));
				if (ColliderMenuItem("Sphere", sphereClassIcon->getID(), shape == &SphereClass::instance))
					collider->part->setShape(sphereShape(scale[0]));
				if (ColliderMenuItem("Cylinder", cylinderClassIcon->getID(), shape == &CylinderClass::instance))
					collider->part->setShape(cylinderShape(scale[0], scale[1]));
				if (ColliderMenuItem("Corner", cornerClassIcon->getID(), shape == &CornerClass::instance))
					collider->part->setShape(cornerShape(scale[0], scale[1], scale[2]));
				if (ColliderMenuItem("Wedge", wedgeClassIcon->getID(), shape == &WedgeClass::instance))
					collider->part->setShape(wedgeShape(scale[0], scale[1], scale[2]));

				ImGui::EndMenu();
			}
		}

		// Collapse / unfold
		if (!leaf) {
			if (open) {
				if (ImGui::MenuItem("Collapse"))
					GImGui->CurrentWindow->DC.StateStorage->SetInt(id, 0);
			} else {
				if (ImGui::MenuItem("Expand")) 
					GImGui->CurrentWindow->DC.StateStorage->SetInt(id, 1);
			}
		}

		// Rename
		if (ImGui::BeginMenu("Rename")) {
			static char buffer[20] = "";
			if (buffer[0] == '\0')
				strcpy(buffer, name.c_str());

			ImGui::Text("Edit name:");
			ImGui::InputText("##edit", buffer, IM_ARRAYSIZE(buffer));
			if (ImGui::Button("Apply")) 
				registry.get<Comp::Name>(entity)->name = buffer;
			
			ImGui::EndMenu();
		}

		// Add
		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImVec4(0.458f, 0.776f, 0.298f, 1.0f)));
		if (ImGui::MenuItem("Add")) {
			auto newEntity = registry.create();
			SelectionTool::single(newEntity);
		}
		ImGui::PopStyleColor();

		// Delete
		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImVec4(0.929f, 0.262f, 0.212f, 1.0f)));
		if (ImGui::MenuItem("Delete")) {
			registry.destroy(entity);
		}
		ImGui::PopStyleColor();

		ImGui::EndPopup();
	}

	// Render content
	if (open) {
		// Render attachments
		if (!attachments.empty()) {
			if (IconTreeNode(0, registry, "Attachments", baseFlags, attachmentsIcon->getID(), false, []() {})) {
				renderAttachments(registry, entity, *collider, attachments);
				ImGui::TreePop();
			}
		}

		// Render hard constraints
		if (!hardConstraints.empty()) {
			if (IconTreeNode(0, registry, "Hard Constraints", baseFlags, hardConstraintsIcon->getID(), false, []() {})) {
				renderHardConstraints(registry, entity, *collider, hardConstraints);
				ImGui::TreePop();
			}
		}

		// Render soft links
		if (!softLinks.empty()) {
			if (IconTreeNode(0, registry, "Soft Links", baseFlags, softLinksIcon->getID(), false, []() {})) {
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
	ImGui::PopID();
}

template <typename Link, typename Tool, typename Component, typename BaseLink>
bool softLinkDispatch(Engine::Registry64& registry, const std::vector<BaseLink*>& links, std::size_t index) {
	BaseLink* baseLink = links[index];

	if (Link* link = dynamic_cast<Link*>(baseLink)) {
		std::string name = Util::format("%s %d", Util::decamel(Util::demangle(Util::typeName<Link>())).c_str(), index + 1);

		bool selected = selectedNode == link;

		auto onPressed = [&] {
			SelectionTool::selection.clear();
			selectedNodeEntity = registry.destroy(selectedNodeEntity);
			if (selectedNode != link) {
				Engine::Registry64::entity_type entity = registry.create();
				registry.add<Comp::Name>(entity, name);
				registry.add<Component>(entity, link);

				selectedNode = link;
				selectedNodeEntity = entity;
				SelectionTool::select(selectedNodeEntity);
			} else
				selectedNode = nullptr;
		};

		IconTreeNode(0, registry, name.c_str(), leafFlags, ResourceManager::get<Graphics::TextureResource>(Tool::getStaticName())->getID(), selected, onPressed);

		return true;
	}

	return false;
}

template <typename Constraint, typename Tool, typename Component>
bool hardConstraintDispatch(Engine::Registry64& registry, const std::vector<HardPhysicalConnection*>& constraints, std::size_t index) {
	HardPhysicalConnection* connection = constraints[index];

	if (Constraint* constraint = dynamic_cast<Constraint*>(connection->constraintWithParent.get())) {
		std::string name = Util::format("%s %d", Util::decamel(Util::demangle(Util::typeName<Constraint>())).c_str(), index + 1);

		bool selected = selectedNode == constraint;

		auto onPressed = [&] {	
			SelectionTool::selection.clear();
			selectedNodeEntity = registry.destroy(selectedNodeEntity);
			if (selectedNode != constraint) {
				Engine::Registry64::entity_type entity = registry.create();
				registry.add<Comp::Name>(entity, name);
				registry.add<Component>(entity, connection);

				selectedNode = constraint;
				selectedNodeEntity = entity;
				SelectionTool::select(selectedNodeEntity);
			} else
				selectedNode = nullptr;
		};

		IconTreeNode(0, registry, name.c_str(), leafFlags, ResourceManager::get<Graphics::TextureResource>(Tool::getStaticName())->getID(), selected, onPressed);

		return true;
	}

	return false;
}

void attachmentDispatch(Engine::Registry64& registry, const std::vector<std::pair<Part*, Part*>>& attachments, std::size_t index) {
	Part* from = attachments[index].first;
	Part* to = attachments[index].second;
	Part* childPart = to->isMainPart() ? from : to;

	std::string name = Util::format("Attachment %d", index + 1);

	bool selected = selectedNode == childPart;

	auto onPressed = [&] {
		SelectionTool::selection.clear();
		selectedNodeEntity = registry.destroy(selectedNodeEntity);
		if (selectedNode != childPart) {
			Engine::Registry64::entity_type entity = registry.create();
			registry.add<Comp::Name>(entity, name);
			registry.add<Comp::Attachment>(entity, from, to);

			selectedNode = childPart;
			selectedNodeEntity = entity;
			SelectionTool::select(selectedNodeEntity);
		} else
			selectedNode = nullptr;
	};

	IconTreeNode(0, registry, name.c_str(), leafFlags, ResourceManager::get<Graphics::TextureResource>(AttachmentTool::getStaticName())->getID(), selected, onPressed);
}

void ECSFrame::renderHardConstraints(Engine::Registry64& registry,
                                     const Engine::Registry64::entity_type& entity,
                                     const Comp::Collider& collider,
                                     const std::vector<HardPhysicalConnection*>& hardConstraints) {

	for (std::size_t index = 0; index < hardConstraints.size(); index++) {
		ImGui::PushID(hardConstraints[index]);

		bool dispatched = false;
		dispatched |= hardConstraintDispatch<FixedConstraint, FixedConstraintTool, Comp::FixedConstraint>(registry, hardConstraints, index);
		dispatched |= hardConstraintDispatch<ConstantSpeedMotorConstraint, MotorConstraintTool, Comp::HardConstraint>(registry, hardConstraints, index);

		if (!dispatched) 
			IconTreeNode(0, registry, "Unknown hard constraint", leafFlags, hardConstraintsIcon->getID(), selectedNode == hardConstraints[index], []() {});

		ImGui::PopID();
	}
}

void ECSFrame::renderSoftLinks(Engine::Registry64& registry,
                               const Engine::Registry64::entity_type& entity,
                               const Comp::Collider& collider,
                               const std::vector<SoftLink*>& softLinks) {

	for (std::size_t index = 0; index < softLinks.size(); index++) {
		ImGui::PushID(softLinks[index]);

		bool dispatched = false;
		dispatched |= softLinkDispatch<MagneticLink, MagneticLinkTool, Comp::MagneticLink>(registry, softLinks, index);
		dispatched |= softLinkDispatch<ElasticLink, ElasticLinkTool, Comp::ElasticLink>(registry, softLinks, index);
		dispatched |= softLinkDispatch<SpringLink, SpringLinkTool, Comp::SpringLink>(registry, softLinks, index);
		dispatched |= softLinkDispatch<AlignmentLink, AlignmentLinkTool, Comp::AlignmentLink>(registry, softLinks, index);

		if (!dispatched)
			IconTreeNode(0, registry, "Unknown soft link", leafFlags, softLinksIcon->getID(), selectedNode == softLinks[index], [] {});
			
		ImGui::PopID();
	}
}

void ECSFrame::renderAttachments(Engine::Registry64& registry,
                                 const Engine::Registry64::entity_type& entity,
                                 const Comp::Collider& collider,
                                 const std::vector<std::pair<Part*, Part*>>& attachments) {

	for (std::size_t index = 0; index < attachments.size(); index++) {
		ImGui::PushID(attachments[index].second);

		attachmentDispatch(registry, attachments, index);

		ImGui::PopID();
	}
}

void ECSFrame::onInit(Engine::Registry64& registry) {
	folderIcon = ResourceManager::add<Graphics::TextureResource>("folder", "../res/textures/icons/Folder.png");
	openFolderIcon = ResourceManager::add<Graphics::TextureResource>("folder open", "../res/textures/icons/Folder Open.png");
	entityIcon = ResourceManager::add<Graphics::TextureResource>("entity", "../res/textures/icons/Entity.png");
	colliderIcon = ResourceManager::add<Graphics::TextureResource>("collider", "../res/textures/icons/Collider.png");
	terrainIcon = ResourceManager::add<Graphics::TextureResource>("terrain collider", "../res/textures/icons/Terrain Collider.png");
	mainColliderIcon = ResourceManager::add<Graphics::TextureResource>("main collider", "../res/textures/icons/Main Collider.png");
	childColliderIcon = ResourceManager::add<Graphics::TextureResource>("child collider", "../res/textures/icons/Child Collider.png");
	hardConstraintsIcon = ResourceManager::add<Graphics::TextureResource>("hard constraints", "../res/textures/icons/Hard Constraints.png");
	softLinksIcon = ResourceManager::add<Graphics::TextureResource>("soft constraints", "../res/textures/icons/Soft Constraints.png");
	attachmentsIcon = ResourceManager::add<Graphics::TextureResource>("attachments", "../res/textures/icons/Attachments.png");
	cframeIcon = ResourceManager::add<Graphics::TextureResource>("cframe", "../res/textures/icons/Axes.png");
	cubeClassIcon = ResourceManager::add<Graphics::TextureResource>("cube", "../res/textures/icons/Cube.png");
	sphereClassIcon = ResourceManager::add<Graphics::TextureResource>("sphere", "../res/textures/icons/Sphere.png");
	cylinderClassIcon = ResourceManager::add<Graphics::TextureResource>("cylinder", "../res/textures/icons/Cylinder.png");
	cornerClassIcon = ResourceManager::add<Graphics::TextureResource>("corner", "../res/textures/icons/Tetrahedron.png");
	wedgeClassIcon = ResourceManager::add<Graphics::TextureResource>("wedge", "../res/textures/icons/Wedge.png");
	polygonClassIcon = ResourceManager::add<Graphics::TextureResource>("polygon", "../res/textures/icons/Dodecahedron.png");
	shownIcon = ResourceManager::add<Graphics::TextureResource>("shown", "../res/textures/icons/Eye.png");
	hiddenIcon = ResourceManager::add<Graphics::TextureResource>("hidden", "../res/textures/icons/Hidden.png");
	physicsIcon = ResourceManager::add<Graphics::TextureResource>("physics", "../res/textures/icons/Physics.png");
	addIcon = ResourceManager::add<Graphics::TextureResource>("add", "../res/textures/icons/Add.png");
}


void ECSFrame::onRender(Engine::Registry64& registry) {
	using namespace Engine;

	ImGui::Begin("Tree");


	auto filter = [&registry](const Registry64::entity_set_iterator& iterator) {
		return registry.getParent(*iterator) == 0;
	};

	nodeIndex = 0;
	HeaderNode();
	for (auto entity : registry.filter(filter))
		renderEntity(registry, registry.getSelf(entity));

	ImGui::End();
}

}
