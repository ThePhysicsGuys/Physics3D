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
	/*ImGui::SameLine(ImGui::GetWindowWidth() - 25.0f); \
	/*if (ImGui::Button("+", ImVec2(25.0f, 0)))*/ \
		/*ImGui::OpenPopup("ComponentSettings");*/ \
	if (open) { \
		ImGui::Columns(2)

#define ECS_PROPERTY_FRAME_END \
		ImGui::Columns(1); \
	}
	
#define ECS_PROPERTY_IF(text, widget, code) \
	{ \
		ImGui::TextUnformatted(text); \
		ImGui::NextColumn(); \
		ImGui::SetNextItemWidth(-1); \
		if (widget) { \
			code \
		}; \
		ImGui::NextColumn(); \
	}

#define ECS_PROPERTY(text, widget) \
	{ \
		ImGui::TextUnformatted(text); \
		ImGui::NextColumn(); \
		ImGui::SetNextItemWidth(-1); \
		widget; \
		ImGui::NextColumn(); \
	} 

#define ECS_TITLE(text, newline) \
	{ \
		if (newline) { \
			ECS_PROPERTY("", ); \
		} \
		ImGui::TextColored(GImGui->Style.Colors[ImGuiCol_ButtonActive], text); \
		ImGui::NextColumn(); \
		ImGui::SetNextItemWidth(-1); \
		ImGui::NextColumn(); \
	}
	
void renderEntity(Engine::Registry64& registry, Engine::Registry64::component_type index, const Ref<RefCountable>& component) {
	std::string label(registry.getComponentName(index));

	ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_Leaf);
}

void renderEntity(Engine::Registry64& registry, Engine::Registry64::component_type index, const Ref<Comp::Model>& component) {
	ECS_PROPERTY_FRAME_START(registry, index);

	ExtendedPart* selectedPart = component->part;
	Motion motion = selectedPart->getMotion();

	float mass = selectedPart->getMass();
	float friction = selectedPart->getFriction();
	float density = selectedPart->getDensity();
	float bouncyness = selectedPart->getBouncyness();
	Vec3f conveyorEffect = selectedPart->getConveyorEffect();

	ECS_TITLE("Part Info", false);
	ECS_PROPERTY("Velocity:", ImGui::Text(str(motion.getVelocity()).c_str()));
	ECS_PROPERTY("Acceleration:", ImGui::Text(str(motion.getAcceleration()).c_str()));
	ECS_PROPERTY("Angular velocity:", ImGui::Text(str(motion.getAngularVelocity()).c_str()));
	ECS_PROPERTY("Angular acceleration:", ImGui::Text(str(motion.getAngularAcceleration()).c_str()));
	
	ECS_PROPERTY_IF("Mass:", ImGui::DragFloat("##Mass", &mass, 0.05f),
		selectedPart->setMass(mass);
	);

	ECS_TITLE("Part Properties", true);
	ECS_PROPERTY_IF("Friction:", ImGui::DragFloat("##Friction", &friction, 0.05f),
		selectedPart->setFriction(friction);
	);
	ECS_PROPERTY_IF("Density:", ImGui::DragFloat("##Density", &density, 0.05f),
		selectedPart->setDensity(density);
	);
	ECS_PROPERTY_IF("Bouncyness:", ImGui::DragFloat("##Bouncyness", &bouncyness, 0.05f),
		selectedPart->setBouncyness(bouncyness);
	);
	ECS_PROPERTY_IF("Conveyor effect:", ImGui::DragVec3("##ConveyorEffect", conveyorEffect.data),
		selectedPart->setConveyorEffect(conveyorEffect);
	);
	
	ECS_PROPERTY("Inertia:", ImGui::Text(str(selectedPart->getInertia()).c_str()));

	if (selectedPart->parent != nullptr) {
		const MotorizedPhysical* physical = selectedPart->parent->mainPhysical;
		Motion comMotion = physical->getMotionOfCenterOfMass();
		
		ECS_TITLE("Physical Info:", true);
		ECS_PROPERTY("Total impulse:", ImGui::Text(str(physical->getTotalImpulse()).c_str()));
		ECS_PROPERTY("Total angular momentum:", ImGui::Text(str(physical->getTotalAngularMomentum()).c_str()));
		ECS_PROPERTY("COM Velocity:", ImGui::Text(str(comMotion.getVelocity()).c_str()));
		ECS_PROPERTY("COM Acceleration:", ImGui::Text(str(comMotion.getAcceleration()).c_str()));
		ECS_PROPERTY("COM Angular velocity:", ImGui::Text(str(comMotion.getAngularVelocity()).c_str()));
		ECS_PROPERTY("COM Angular acceleration:", ImGui::Text(str(comMotion.getAngularAcceleration()).c_str()));
	}

	static volatile ExtendedPart* sp = nullptr;
	if (sp != nullptr) sp = selectedPart;

	ECS_TITLE("Debug", true);
	
	ECS_PROPERTY_IF("Debug part:", ImGui::Button("Debug"), 
		Log::debug("Debugging part %d", reinterpret_cast<uint64_t>(sp));
		P3D_DEBUGBREAK;
	);
	
	ECS_PROPERTY_FRAME_END;
}

void renderEntity(Engine::Registry64& registry, Engine::Registry64::component_type index, const Ref<Comp::Name>& component) {
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
	
	if (component->flags != 0) {
		ECS_TITLE("Textures:", true);
		float size = ImGui::GetTextLineHeightWithSpacing();
		if (component->has(Comp::Material::ALBEDO)) 
			ECS_PROPERTY("Albedo", ImGui::Image((ImTextureID) component->get(Comp::Material::ALBEDO)->getID(), ImVec2(size, size)));
		
		
		if (component->has(Comp::Material::NORMAL)) 
			ECS_PROPERTY("Normal", ImGui::Image((ImTextureID) component->get(Comp::Material::NORMAL)->getID(), ImVec2(size, size)));
		
		
		if (component->has(Comp::Material::METALNESS)) 
			ECS_PROPERTY("Metalness", ImGui::Image((ImTextureID) component->get(Comp::Material::METALNESS)->getID(), ImVec2(size, size)));
		
	
		if (component->has(Comp::Material::ROUGHNESS)) 
			ECS_PROPERTY("Roughness", ImGui::Image((ImTextureID) component->get(Comp::Material::ROUGHNESS)->getID(), ImVec2(size, size)));
		
			
		if (component->has(Comp::Material::AO)) 
			ECS_PROPERTY("Ambient occlusion", ImGui::Image((ImTextureID) component->get(Comp::Material::AO)->getID(), ImVec2(size, size)));
		
			
		if (component->has(Comp::Material::GLOSS)) 
			ECS_PROPERTY("Gloss", ImGui::Image((ImTextureID) component->get(Comp::Material::GLOSS)->getID(), ImVec2(size, size)));
		
			
		if (component->has(Comp::Material::SPECULAR)) 
			ECS_PROPERTY("Specular", ImGui::Image((ImTextureID) component->get(Comp::Material::SPECULAR)->getID(), ImVec2(size, size)));
		
			
		if (component->has(Comp::Material::DISPLACEMENT)) 
			ECS_PROPERTY("Displacement", ImGui::Image((ImTextureID) component->get(Comp::Material::DISPLACEMENT)->getID(), ImVec2(size, size)));
		
	}
		
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
	
	ECS_PROPERTY_IF("Position:", ImGui::DragVec3("Position", position.data, 0),
		frame.position = castVec3fToPosition(position);
		component->setCFrame(frame);
	);

	ECS_PROPERTY_IF("Rotation:", ImGui::DragVec3("Rotation", rotation.data, 0.01f),
		frame.rotation = Rotation::fromRotationVec(rotation);
		component->setCFrame(frame);
	);

	if (part_attached) {
		ECS_PROPERTY_IF("Scale:", ImGui::DragVec3("Scale", scale.data, 0, 0.01f),
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
			ENTITY_DISPATCH(index, Comp::Name, registry, component);
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
