#include "core.h"

#include "propertiesFrame.h"

#include "screen.h"
#include "application.h"
#include "extendedPart.h"
#include "ecs/components.h"
#include "imgui/imgui.h"
#include "../graphics/gui/imgui/imguiExtension.h"
#include "../physics/misc/toString.h"
#include "../physics/physical.h"

namespace P3D::Application {

Vec3f PropertiesFrame::position = Vec3f(0, 0, 0);

#define ENTITY_DISPATCH_START(index) \
	if ((index) == -1) \
		continue
	
#define ENTITY_DISPATCH(index, type, registry, component) \
	else if ((index) == (registry).getComponentIndex<type>()) \
		renderEntity(registry, index, intrusive_cast<type>(component))
	
#define ENTITY_DISPATCH_END(index, registry, component) \
	else \
		renderEntity(registry, index, component)

#define ECS_PROPERTY_FRAME_START(registry, index) \
	std::string label((registry).getComponentName(index)); \
	bool open = ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen); \
	/*ImGui::SameLine(ImGui::GetWindowWidth() - 25.0f);*/ \
	/*if (ImGui::Button("+", ImVec2(25.0f, 0)))*/ \
		/*ImGui::OpenPopup("ComponentSettings");*/ \
	if (open) { \
		ImGui::Columns(2)

#define ECS_PROPERTY_FRAME_END \
		ImGui::Columns(1); \
	}
	
#define ECS_PROPERTY_IF(text, widget, code) \
	ImGui::TextUnformatted(text); \
	ImGui::NextColumn(); \
	ImGui::SetNextItemWidth(-1); \
	if (widget) { \
		code \
	}; \
	ImGui::NextColumn()

#define ECS_PROPERTY(text, widget) \
	ImGui::TextUnformatted(text); \
	ImGui::NextColumn(); \
	ImGui::SetNextItemWidth(-1); \
	widget; \
	ImGui::NextColumn()

#define ECS_TITLE(text, newline) \
	if (newline) { \
		ECS_PROPERTY("", ); \
	} \
	ImGui::TextColored(ImVec4(0.28f, 0.56f, 1.00f, 1.00f), text); \
	ImGui::NextColumn(); \
	ImGui::SetNextItemWidth(-1); \
	ImGui::NextColumn()
	
void renderEntity(Engine::Registry64& registry, Engine::Registry64::component_type index, const Ref<RefCountable>& component) {
	std::string label(registry.getComponentName(index));

	ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_Leaf);
}

void renderEntity(Engine::Registry64& registry, Engine::Registry64::component_type index, const Ref<Comp::Model>& component) {
	ECS_PROPERTY_FRAME_START(registry, index);

	ExtendedPart* selectedPart = component->part;
	
	ECS_PROPERTY("Velocity:", ImGui::Text(str(selectedPart->getMotion().getVelocity()).c_str()));
	ECS_PROPERTY("Acceleration:", ImGui::Text(str(selectedPart->getMotion().getAcceleration()).c_str()));
	ECS_PROPERTY("Angular velocity:", ImGui::Text(str(selectedPart->getMotion().getAngularVelocity()).c_str()));
	ECS_PROPERTY("Angular acceleration:", ImGui::Text(str(selectedPart->getMotion().getAngularAcceleration()).c_str()));
	ECS_PROPERTY("Mass:", ImGui::Text(str(selectedPart->getMass()).c_str()));
	ECS_PROPERTY("Friction:", ImGui::Text(str(selectedPart->properties.friction).c_str()));
	ECS_PROPERTY("Density:", ImGui::Text(str(selectedPart->properties.density).c_str()));
	ECS_PROPERTY("Bouncyness:", ImGui::Text(str(selectedPart->properties.bouncyness).c_str()));
	ECS_PROPERTY("Conveyor:", ImGui::Text(str(selectedPart->properties.conveyorEffect).c_str()));
	ECS_PROPERTY("Inertia:", ImGui::Text(str(selectedPart->getInertia()).c_str()));

	if (selectedPart->parent != nullptr) {
		const MotorizedPhysical* physical = selectedPart->parent->mainPhysical;
		
		ECS_TITLE("Physical Info:", true);
		ECS_PROPERTY("Total impulse:", ImGui::Text(str(physical->getTotalImpulse()).c_str()));
		ECS_PROPERTY("Total angular momentum:", ImGui::Text(str(physical->getTotalAngularMomentum()).c_str()));
		ECS_PROPERTY("Motion of COM:", ImGui::Text(str(physical->getMotionOfCenterOfMass()).c_str()));
	}

	static volatile ExtendedPart* sp = nullptr;
	if (sp != nullptr) sp = selectedPart;

	ECS_PROPERTY_IF("Debug part:", ImGui::Button("Debug"), 
		Log::debug("Debugging part %d", reinterpret_cast<uint64_t>(sp));
		P3D_DEBUGBREAK;
	);
	
	ECS_PROPERTY_FRAME_END;
}

void renderEntity(Engine::Registry64& registry, Engine::Registry64::component_type index, const Ref<Comp::Tag>& component) {
	ECS_PROPERTY_FRAME_START(registry, index);
	
	ECS_PROPERTY("Name:", ImGui::Text(component->name.c_str()));
	
	ECS_PROPERTY_FRAME_END;
}

void renderEntity(Engine::Registry64& registry, Engine::Registry64::component_type index, const Ref<Comp::Mesh>& component) {
	ECS_PROPERTY_FRAME_START(registry, index);

	ECS_PROPERTY("ID:", ImGui::Text(str(component->id).c_str()));
	ECS_PROPERTY("Mode:", ImGui::Text(str(component->mode).c_str()));
	ECS_PROPERTY("Normals:", ImGui::Text(component->hasNormals ? "Yes" : "No"));
	ECS_PROPERTY("UVs:", ImGui::Text(component->hasUVs ? "Yes" : "No"));

	ECS_PROPERTY_FRAME_END;
}

void renderEntity(Engine::Registry64& registry, Engine::Registry64::component_type index, const Ref<Comp::Material>& component) {
	ECS_PROPERTY_FRAME_START(registry, index);

	ECS_PROPERTY("Albedo", ImGui::ColorEdit4("##Albedo", component->albedo.data));
	ECS_PROPERTY("Metalness", ImGui::SliderFloat("##Metalness", &component->metalness, 0, 1));
	ECS_PROPERTY("Roughness", ImGui::SliderFloat("##Roughness", &component->roughness, 0, 1));
	ECS_PROPERTY("Ambient occlusion", ImGui::SliderFloat("##AO", &component->ao, 0, 1));

	ECS_PROPERTY_FRAME_END;
}

void renderEntity(Engine::Registry64& registry, Engine::Registry64::component_type index, const Ref<Comp::Transform>& component) {

	ECS_PROPERTY_FRAME_START(registry, index);

	GlobalCFrame frame = component->getCFrame();
	Vec3f position = castPositionToVec3f(frame.getPosition());
	Vec3f rotation = frame.getRotation().asRotationVector();
	bool part_attached = component->isPartAttached;
	ExtendedPart* part = component->isPartAttached ? component->part : nullptr;
	DiagonalMat3f scale = part->hitbox.scale;
	
	ECS_PROPERTY_IF("Position:", ImGui::InputVec3("Position", position.data, 0),
		frame.position = castVec3fToPosition(position);
		component->setCFrame(frame);
	);

	ECS_PROPERTY_IF("Rotation:", ImGui::InputVec3("Rotation", rotation.data, 0, 0.01f),
		frame.rotation = Rotation::fromRotationVec(rotation);
		component->setCFrame(frame);
	);

	if (part_attached) {
		ECS_PROPERTY_IF("Scale:", ImGui::InputVec3("Scale", scale.data, 0, 0.01f),
			part->hitbox.scale = scale;
		);
	}
	
	ECS_PROPERTY_FRAME_END;

}
	
void PropertiesFrame::onInit(Engine::Registry64& registry) {
	
}

void PropertiesFrame::onRender(Engine::Registry64& registry) {
	ImGui::Begin("Properties");
	
	if (screen.selectedEntity) {
		auto components = registry.getComponents(screen.selectedEntity);
		for (auto [index, component] : components) {
			ENTITY_DISPATCH_START(index);
			ENTITY_DISPATCH(index, Comp::Tag, registry, component);
			ENTITY_DISPATCH(index, Comp::Transform, registry, component);
			ENTITY_DISPATCH(index, Comp::Model, registry, component);
			ENTITY_DISPATCH(index, Comp::Mesh, registry, component);
			ENTITY_DISPATCH(index, Comp::Material, registry, component);
			ENTITY_DISPATCH_END(index, registry, component);
		}

		if (ImGui::Button("Add new component...", ImVec2(-1, 0)))
			ImGui::OpenPopup("Add component");
		
		if (ImGui::BeginPopupModal("Add component")) {
			std::vector<const char*> components;
			for (Engine::Registry64::component_type index = 0; index < Engine::Registry64::component_index<void>::index(); index++)
				components.push_back(registry.getComponentName(index).data());
			
			static int item_current = 0;
			ImGui::SetNextItemWidth(-1);
			ImGui::ListBox("##ComponentsModal", &item_current, components.data(), components.size(), 6);
			
			if (ImGui::Button("Cancel", ImVec2(-1, 0)))
				ImGui::CloseCurrentPopup();
			
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopup("ComponentSettings")) {
			if (ImGui::MenuItem("Remove component"));
			ImGui::EndPopup();
		}
		
	} else {
		std::string label = "Select an entity to see properties";
		auto [wx, wy] = ImGui::GetContentRegionAvail();
		auto [tx, ty] = ImGui::CalcTextSize(label.c_str());
		auto [cx, cy] = ImGui::GetCursorPos();
		ImVec2 cursor = ImVec2(cx + (wx - tx) / 2.0f, cy + (wy - ty) / 2.0f);
		ImGui::SetCursorPos(cursor);
		ImGui::Text(label.c_str());
	}
	
	ImGui::End();
}
	
}
