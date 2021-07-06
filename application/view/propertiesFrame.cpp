#include "core.h"

#include "propertiesFrame.h"

#include "screen.h"
#include "application.h"
#include "extendedPart.h"
#include "ecs/components.h"
#include "imgui/imgui.h"
#include "../graphics/gui/imgui/imguiExtension.h"
#include <Physics3D/misc/toString.h>
#include <Physics3D/physical.h>
#include "../graphics/renderer.h"

namespace P3D::Application {

Engine::Registry64::component_type deletedComponentIndex = static_cast<Engine::Registry64::component_type>(-1);
std::string errorModalMessage = "";
bool showErrorModal = false;

bool _ecs_property_frame_start(Engine::Registry64& registry, Engine::Registry64::component_type index) {
	std::string label((registry).getComponentName(index));
	bool open = ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
	ImGui::SameLine(ImGui::GetColumnWidth() - 40.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0, 0.0, 0.0, 0.0));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, GImGui->Style.Colors[ImGuiCol_HeaderActive]);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, GImGui->Style.Colors[ImGuiCol_HeaderHovered]);
	ImGui::AlignTextToFramePadding();
	std::string buttonLabel = "...##button_" + label;
	if (ImGui::Button(buttonLabel.c_str())) {
		ImGui::OpenPopup("ComponentSettings");
		deletedComponentIndex = index;
	}

	ImGui::PopStyleColor(3);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Settings");

	return open;
}

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
	if (_ecs_property_frame_start(registry, index)) { \
		ImGui::Columns(2)
	
void renderEntity(Engine::Registry64& registry, Engine::Registry64::component_type index, const IRef<RC>& component) {
	std::string label(registry.getComponentName(index));

	ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_Leaf);
}

void renderEntity(Engine::Registry64& registry, Engine::Registry64::component_type index, const IRef<Comp::Collider>& component) {
	ECS_PROPERTY_FRAME_START(registry, index);
	
	ExtendedPart* selectedPart = component->part;
	Motion motion = selectedPart->getMotion();
	Vec3f velocity = motion.getVelocity();
	Vec3f angularVelocity = motion.getAngularVelocity();

	float mass = selectedPart->getMass();
	float friction = selectedPart->getFriction();
	float density = selectedPart->getDensity();
	float bouncyness = selectedPart->getBouncyness();
	Vec3f conveyorEffect = selectedPart->getConveyorEffect();

	TITLE("Part Info", false);
	PROPERTY_IF("Velocity:", ImGui::DragVec3("##Velocity", velocity.data, 0, 0.1, true),
		selectedPart->setVelocity(velocity);
	);
	PROPERTY_IF("Angular velocity:", ImGui::DragVec3("##AngularVelocity", angularVelocity.data, 0, 0.1, true),
		selectedPart->setAngularVelocity(angularVelocity);
	);
	PROPERTY("Acceleration:", ImGui::Text(str(motion.getAcceleration()).c_str()));
	PROPERTY("Angular acceleration:", ImGui::Text(str(motion.getAngularAcceleration()).c_str()));
	
	PROPERTY_IF("Mass:", ImGui::DragFloat("##Mass", &mass, 0.05f),
		selectedPart->setMass(mass);
	);

	TITLE("Part Properties", true);
	PROPERTY_IF("Friction:", ImGui::DragFloat("##Friction", &friction, 0.05f),
		selectedPart->setFriction(friction);
	);
	PROPERTY_IF("Density:", ImGui::DragFloat("##Density", &density, 0.05f),
		selectedPart->setDensity(density);
	);
	PROPERTY_IF("Bouncyness:", ImGui::DragFloat("##Bouncyness", &bouncyness, 0.05f),
		selectedPart->setBouncyness(bouncyness);
	);
	PROPERTY_IF("Conveyor effect:", ImGui::DragVec3("##ConveyorEffect", conveyorEffect.data, 0, 0.1, true),
		selectedPart->setConveyorEffect(conveyorEffect);
	);
	
	PROPERTY("Inertia:", ImGui::Text(str(selectedPart->getInertia()).c_str()));

	if (selectedPart->parent != nullptr) {
		const MotorizedPhysical* physical = selectedPart->parent->mainPhysical;
		Motion comMotion = physical->getMotionOfCenterOfMass();
		
		TITLE("Physical Info:", true);
		PROPERTY("Total impulse:", ImGui::Text(str(physical->getTotalImpulse()).c_str()));
		PROPERTY("Total angular momentum:", ImGui::Text(str(physical->getTotalAngularMomentum()).c_str()));
		PROPERTY("COM Velocity:", ImGui::Text(str(comMotion.getVelocity()).c_str()));
		PROPERTY("COM Acceleration:", ImGui::Text(str(comMotion.getAcceleration()).c_str()));
		PROPERTY("COM Angular velocity:", ImGui::Text(str(comMotion.getAngularVelocity()).c_str()));
		PROPERTY("COM Angular acceleration:", ImGui::Text(str(comMotion.getAngularAcceleration()).c_str()));
	}

	static volatile ExtendedPart* sp = nullptr;
	if (sp != nullptr) sp = selectedPart;

	TITLE("Debug", true);
	
	PROPERTY_IF("Debug part:", ImGui::Button("Debug"), 
		Log::debug("Debugging part %d", reinterpret_cast<uint64_t>(sp));
		P3D_DEBUGBREAK;
	);
	
	PROPERTY_FRAME_END;
}

void renderEntity(Engine::Registry64& registry, Engine::Registry64::component_type index, const IRef<Comp::Name>& component) {
	ECS_PROPERTY_FRAME_START(registry, index);
	
	PROPERTY("Name:", ImGui::Text(component->name.c_str()));
	
	PROPERTY_FRAME_END;
}

void renderEntity(Engine::Registry64& registry, Engine::Registry64::component_type index, const IRef<Comp::Mesh>& component) {
	ECS_PROPERTY_FRAME_START(registry, index);

	PROPERTY("ID:", ImGui::Text(str(component->id).c_str()));
	PROPERTY("Mode:", ImGui::Text(component->mode == Graphics::Renderer::FILL ? "Fill" : "Wireframe"));
	PROPERTY("Normals:", ImGui::Text(component->hasNormals ? "Yes" : "No"));
	PROPERTY("UVs:", ImGui::Text(component->hasUVs ? "Yes" : "No"));

	PROPERTY_FRAME_END;
}

void renderEntity(Engine::Registry64& registry, Engine::Registry64::component_type index, const IRef<Comp::Material>& component) {
	ECS_PROPERTY_FRAME_START(registry, index);

	PROPERTY("Albedo", ImGui::ColorEdit4("##Albedo", component->albedo.data, ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_AlphaBar));
	PROPERTY("Metalness", ImGui::SliderFloat("##Metalness", &component->metalness, 0, 1));
	PROPERTY("Roughness", ImGui::SliderFloat("##Roughness", &component->roughness, 0, 1));
	PROPERTY("Ambient occlusion", ImGui::SliderFloat("##AO", &component->ao, 0, 1));
	
	if (component->flags != 0) {
		TITLE("Textures:", true);
		float size = ImGui::GetTextLineHeightWithSpacing();
		if (component->has(Comp::Material::ALBEDO)) 
			PROPERTY("Albedo", ImGui::Image((ImTextureID) component->get(Comp::Material::ALBEDO)->getID(), ImVec2(size, size)));
		
		
		if (component->has(Comp::Material::NORMAL)) 
			PROPERTY("Normal", ImGui::Image((ImTextureID) component->get(Comp::Material::NORMAL)->getID(), ImVec2(size, size)));
		
		
		if (component->has(Comp::Material::METALNESS)) 
			PROPERTY("Metalness", ImGui::Image((ImTextureID) component->get(Comp::Material::METALNESS)->getID(), ImVec2(size, size)));
		
	
		if (component->has(Comp::Material::ROUGHNESS)) 
			PROPERTY("Roughness", ImGui::Image((ImTextureID) component->get(Comp::Material::ROUGHNESS)->getID(), ImVec2(size, size)));
		
			
		if (component->has(Comp::Material::AO)) 
			PROPERTY("Ambient occlusion", ImGui::Image((ImTextureID) component->get(Comp::Material::AO)->getID(), ImVec2(size, size)));
		
			
		if (component->has(Comp::Material::GLOSS)) 
			PROPERTY("Gloss", ImGui::Image((ImTextureID) component->get(Comp::Material::GLOSS)->getID(), ImVec2(size, size)));
		
			
		if (component->has(Comp::Material::SPECULAR)) 
			PROPERTY("Specular", ImGui::Image((ImTextureID) component->get(Comp::Material::SPECULAR)->getID(), ImVec2(size, size)));
		
			
		if (component->has(Comp::Material::DISPLACEMENT)) 
			PROPERTY("Displacement", ImGui::Image((ImTextureID) component->get(Comp::Material::DISPLACEMENT)->getID(), ImVec2(size, size)));
		
	}
		
	PROPERTY_FRAME_END;
}

void renderEntity(Engine::Registry64& registry, Engine::Registry64::component_type index, const IRef<Comp::Light>& component) {
	ECS_PROPERTY_FRAME_START(registry, index);

	PROPERTY("Color", ImGui::ColorEdit3("##Color", component->color.data, ImGuiColorEditFlags_PickerHueWheel));
	PROPERTY("Intensity", ImGui::DragFloat("##Intensity", &component->intensity));

	TITLE("Attenuation", true);
	PROPERTY("Constant", ImGui::SliderFloat("##Constant", &component->attenuation.constant, 0, 2));
	PROPERTY("Linear", ImGui::SliderFloat("##Linear", &component->attenuation.linear, 0, 2));
	PROPERTY("Exponent", ImGui::SliderFloat("##Exponent", &component->attenuation.exponent, 0, 2));

	PROPERTY_FRAME_END;
}

void renderEntity(Engine::Registry64& registry, Engine::Registry64::component_type index, const IRef<Comp::Transform>& component) {

	ECS_PROPERTY_FRAME_START(registry, index);

	Vec3f position = castPositionToVec3f(component->getPosition());
	Vec3f rotation = component->getRotation().asRotationVector();
	DiagonalMat3f scale = component->getScale();
	bool standalone = component->isPartAttached();
	
	PROPERTY_DESC("Standalone", "Whether the transform and scale is coming from the part", ImGui::Checkbox("##TransformHitbox", &standalone));
	PROPERTY_IF("Position:", ImGui::DragVec3("TransformPosition", position.data, 0, 0.1, true),
		component->setPosition(castVec3fToPosition(position));
	);

	PROPERTY_IF("Rotation:", ImGui::DragVec3("TransformRotation", rotation.data, 0.01f, 0.02f, true),
		component->setRotation(Rotation::fromRotationVec(rotation));
	);

	float min = 0.01f;
	PROPERTY_IF("Scale:", ImGui::DragVec3("TransformScale", scale.data, 1.0f, 0.01f, true, &min),
		component->setScale(scale);
	);
	
	PROPERTY_FRAME_END;

}

void renderEntity(Engine::Registry64& registry, Engine::Registry64::component_type index, const IRef<Comp::Hitbox>& component) {
	ECS_PROPERTY_FRAME_START(registry, index);

	bool standalone = component->isPartAttached();
	Shape shape = component->getShape();
	DiagonalMat3f scale = shape.scale;

	TITLE("Hitbox", false);
	PROPERTY_DESC("Standalone", "Whether the hitbox is coming from the part", ImGui::Checkbox("##HitboxStandalone", &standalone));
	PROPERTY("Volume:", ImGui::Text(str(shape.getVolume()).c_str()));
	PROPERTY("Center of mass:", ImGui::Text(str(shape.getCenterOfMass()).c_str()));
	float min = 0.01f;
	PROPERTY_IF("Scale:", ImGui::DragVec3("HitboxScale", scale.data, 1, 0.01f, true, &min),
		component->setScale(scale);
	);

	TITLE("Bounding box", true);
	PROPERTY("Width:", ImGui::Text(str(shape.getWidth()).c_str()));
	PROPERTY("Height:", ImGui::Text(str(shape.getHeight()).c_str()));
	PROPERTY("Depth:", ImGui::Text(str(shape.getHeight()).c_str()));
	PROPERTY("Max radius:", ImGui::Text(str(shape.getMaxRadius()).c_str()));

	PROPERTY_FRAME_END;
}
	
void PropertiesFrame::onInit(Engine::Registry64& registry) {
	
}

void PropertiesFrame::onRender(Engine::Registry64& registry) {
	ImGui::Begin("Properties");
	
	if (screen.selectedEntity == Engine::Registry64::null_entity) {
		std::string label = "Select an entity to see properties";
		auto [wx, wy] = ImGui::GetContentRegionAvail();
		auto [tx, ty] = ImGui::CalcTextSize(label.c_str());
		auto [cx, cy] = ImGui::GetCursorPos();
		ImVec2 cursor = ImVec2(cx + (wx - tx) / 2.0f, cy + (wy - ty) / 2.0f);
		ImGui::SetCursorPos(cursor);
		ImGui::Text(label.c_str());

		ImGui::End();
		return;
	}

	// Dispatch component frames
	auto components = registry.getComponents(screen.selectedEntity);
	for (auto [index, component] : components) {
		ENTITY_DISPATCH_START(index);
		ENTITY_DISPATCH(index, Comp::Name, registry, component);
		ENTITY_DISPATCH(index, Comp::Transform, registry, component);
		ENTITY_DISPATCH(index, Comp::Collider, registry, component);
		ENTITY_DISPATCH(index, Comp::Mesh, registry, component);
		ENTITY_DISPATCH(index, Comp::Material, registry, component);
		ENTITY_DISPATCH(index, Comp::Light, registry, component);
		ENTITY_DISPATCH(index, Comp::Hitbox, registry, component);
		ENTITY_DISPATCH_END(index, registry, component);
	}

	// Add new Component
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

	// Component Settings
	if (ImGui::BeginPopup("ComponentSettings")) {
		if (ImGui::MenuItem("Delete component")) {
			if (deletedComponentIndex != static_cast<Engine::Registry64::component_type>(-1)) {
				if (deletedComponentIndex == registry.getComponentIndex<Comp::Transform>()) {
					errorModalMessage = "This component can not be deleted.";
					showErrorModal = true;
				} else {
					registry.remove(screen.selectedEntity, deletedComponentIndex);
				}
			}
		}
		if (ImGui::MenuItem("Component info")) {
			
		}
		ImGui::EndPopup();
	}

	// Error Modal
	if (showErrorModal) 
		ImGui::OpenPopup("Error##ErrorModal");
	if (ImGui::BeginPopupModal("Error##ErrorModal", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), errorModalMessage.c_str());
		if (ImGui::Button("Ok##ErrorModalClose", ImVec2(-1, 0))) {
			ImGui::CloseCurrentPopup();
			showErrorModal = false;
		}
		ImGui::EndPopup();
	}
	
	ImGui::End();
}
	
}
