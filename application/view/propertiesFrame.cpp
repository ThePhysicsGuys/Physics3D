#include "core.h"

#include "propertiesFrame.h"

#include "screen.h"
#include "application.h"
#include "extendedPart.h"
#include "ecs/components.h"
#include "imgui/imgui.h"
#include "picker/tools/selectionTool.h"

#include <Physics3D/misc/toString.h>
#include <Physics3D/physical.h>
#include "Physics3D/hardconstraints/fixedConstraint.h"

#include "../graphics/gui/imgui/imguiExtension.h"
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

#define ENTITY_DISPATCH(registry, entity, index, type, component) \
	else if ((index) == (registry).getComponentIndex<type>()) \
		renderEntity(registry, entity, index, intrusive_cast<type>(component))

#define ENTITY_DISPATCH_END(registry, entity, index, component) \
	else \
		renderEntity(registry, entity, index, component)

#define ECS_PROPERTY_FRAME_START(registry, index) \
	if (_ecs_property_frame_start(registry, index)) { \
		ImGui::Columns(2)

void renderEntity(Engine::Registry64& registry, Engine::Registry64::entity_type entity, Engine::Registry64::component_type index, const IRef<RC>& component) {
	std::string label(registry.getComponentName(index));

	ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_Leaf);
}

void renderEntity(Engine::Registry64& registry, Engine::Registry64::entity_type entity, Engine::Registry64::component_type index, const IRef<Comp::Collider>& component) {
	ECS_PROPERTY_FRAME_START(registry, index);

	ExtendedPart* selectedPart = component->part;
	Motion motion = selectedPart->getMotion();
	Vec3f velocity = motion.getVelocity();
	Vec3f angularVelocity = motion.getAngularVelocity();

	float mass = static_cast<float>(selectedPart->getMass());
	float friction = static_cast<float>(selectedPart->getFriction());
	float density = static_cast<float>(selectedPart->getDensity());
	float bouncyness = static_cast<float>(selectedPart->getBouncyness());
	Vec3f conveyorEffect = selectedPart->getConveyorEffect();

	TITLE("Part Info", false);
	PROPERTY_IF_LOCK("Velocity:", ImGui::DragVec3("##Velocity", velocity.data, 0, nullptr, true), selectedPart->setVelocity(velocity););
	PROPERTY_IF_LOCK("Angular velocity:",
	            ImGui::DragVec3("##AngularVelocity", angularVelocity.data, 0, nullptr, true),
	            selectedPart->setAngularVelocity(angularVelocity););
	PROPERTY("Acceleration:", ImGui::Text(str(motion.getAcceleration()).c_str()));
	PROPERTY("Angular acceleration:", ImGui::Text(str(motion.getAngularAcceleration()).c_str()));

	PROPERTY_IF_LOCK("Mass:", ImGui::DragFloat("##Mass", &mass, 0.05f), selectedPart->setMass(mass););

	TITLE("Part Properties", true);
	PROPERTY_IF_LOCK("Friction:", ImGui::DragFloat("##Friction", &friction, 0.05f), selectedPart->setFriction(friction););
	PROPERTY_IF_LOCK("Density:", ImGui::DragFloat("##Density", &density, 0.05f), selectedPart->setDensity(density););
	PROPERTY_IF_LOCK("Bouncyness:", ImGui::DragFloat("##Bouncyness", &bouncyness, 0.05f), selectedPart->setBouncyness(bouncyness););
	PROPERTY_IF_LOCK("Conveyor effect:",
	            ImGui::DragVec3("##ConveyorEffect", conveyorEffect.data, 0, nullptr, true),
	            selectedPart->setConveyorEffect(conveyorEffect););

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
	if (sp != nullptr)
		sp = selectedPart;

	TITLE("Debug", true);

	PROPERTY_IF(
		"Debug part:", 
		ImGui::Button("Debug"), 
		Log::debug("Debugging part %d", reinterpret_cast<uint64_t>(sp)); P3D_DEBUGBREAK;
	);

	PROPERTY_FRAME_END;
}

void renderEntity(Engine::Registry64& registry, Engine::Registry64::entity_type entity, Engine::Registry64::component_type index, const IRef<Comp::Name>& component) {
	ECS_PROPERTY_FRAME_START(registry, index);

	PROPERTY("Name:", ImGui::Text(component->name.c_str()));

	PROPERTY_FRAME_END;
}

void renderEntity(Engine::Registry64& registry, Engine::Registry64::entity_type entity, Engine::Registry64::component_type index, const IRef<Graphics::Comp::Mesh>& component) {
	using namespace Graphics::Comp;

	ECS_PROPERTY_FRAME_START(registry, index);

	bool normals = component->flags & Mesh::Flags_Normal;
	bool uvs = component->flags & Mesh::Flags_UV;
	bool tangents = component->flags & Mesh::Flags_Tangent;
	bool bitangents = component->flags & Mesh::Flags_Bitangent;

	PROPERTY("ID:", ImGui::Text(str(component->id).c_str()));
	PROPERTY("Visible:", ImGui::Checkbox("##Visible", &component->visible));

	TITLE("Flags", true);
	PROPERTY("Normals:", ImGui::Checkbox("##Normals", &normals));
	PROPERTY("UVs:", ImGui::Checkbox("##UVs", &uvs));
	PROPERTY("Tangents:", ImGui::Checkbox("##Tangents", &tangents));
	PROPERTY("Bitangents:", ImGui::Checkbox("##Bitangents", &bitangents));

	PROPERTY_FRAME_END;
}

void renderEntity(Engine::Registry64& registry, Engine::Registry64::entity_type entity, Engine::Registry64::component_type index, const IRef<Graphics::Comp::Material>& component) {
	ECS_PROPERTY_FRAME_START(registry, index);

	PROPERTY("Albedo", ImGui::ColorEdit4("##Albedo", component->albedo.data, ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_AlphaBar));
	PROPERTY("Metalness", ImGui::SliderFloat("##Metalness", &component->metalness, 0, 1));
	PROPERTY("Roughness", ImGui::SliderFloat("##Roughness", &component->roughness, 0, 1));
	PROPERTY("Ambient occlusion", ImGui::SliderFloat("##AO", &component->ao, 0, 1));

	TITLE("Textures:", true);
	float size = ImGui::GetTextLineHeightWithSpacing();
	if (component->has(Graphics::Comp::Material::Map_Albedo)) 
		PROPERTY(
			"Albedo",
			ImGui::Image(reinterpret_cast<ImTextureID>(component->get(Graphics::Comp::Material::Map_Albedo)->getID()), ImVec2(size, size))
		);


	if (component->has(Graphics::Comp::Material::Map_Normal))
		PROPERTY(
			"Normal",
			ImGui::Image(reinterpret_cast<ImTextureID>(component->get(Graphics::Comp::Material::Map_Normal)->getID()), ImVec2(size, size))
		);


	if (component->has(Graphics::Comp::Material::Map_Metalness))
		PROPERTY(
			"Metalness",
			ImGui::Image(reinterpret_cast<ImTextureID>(component->get(Graphics::Comp::Material::Map_Metalness)->getID()), ImVec2(size, size))
		);


	if (component->has(Graphics::Comp::Material::Map_Roughness))
		PROPERTY(
			"Roughness",
			ImGui::Image(reinterpret_cast<ImTextureID>(component->get(Graphics::Comp::Material::Map_Roughness)->getID()), ImVec2(size, size))
		);


	if (component->has(Graphics::Comp::Material::Map_AO))
		PROPERTY("Ambient occlusion",
			ImGui::Image(reinterpret_cast<ImTextureID>(component->get(Graphics::Comp::Material::Map_AO)->getID()), ImVec2(size, size))
		);


	if (component->has(Graphics::Comp::Material::Map_Gloss))
		PROPERTY("Gloss",
			ImGui::Image(reinterpret_cast<ImTextureID>(component->get(Graphics::Comp::Material::Map_Gloss)->getID()), ImVec2(size, size))
		);


	if (component->has(Graphics::Comp::Material::Map_Specular))
		PROPERTY(
			"Specular",
			ImGui::Image(reinterpret_cast<ImTextureID>(component->get(Graphics::Comp::Material::Map_Specular)->getID()), ImVec2(size, size))
		);


	if (component->has(Graphics::Comp::Material::Map_Displacement))
		PROPERTY(
			"Displacement",
			ImGui::Image(reinterpret_cast<ImTextureID>(component->get(Graphics::Comp::Material::Map_Displacement)->getID()), ImVec2(size, size))
		);
	

	PROPERTY_FRAME_END;
}

void renderEntity(Engine::Registry64& registry, Engine::Registry64::entity_type entity, Engine::Registry64::component_type index, const IRef<Comp::Light>& component) {
	ECS_PROPERTY_FRAME_START(registry, index);

	PROPERTY("Color", ImGui::ColorEdit3("##Color", component->color.data, ImGuiColorEditFlags_PickerHueWheel));
	PROPERTY("Intensity", ImGui::DragFloat("##Intensity", &component->intensity));

	TITLE("Attenuation", true);
	PROPERTY("Constant", ImGui::SliderFloat("##Constant", &component->attenuation.constant, 0, 2));
	PROPERTY("Linear", ImGui::SliderFloat("##Linear", &component->attenuation.linear, 0, 2));
	PROPERTY("Exponent", ImGui::SliderFloat("##Exponent", &component->attenuation.exponent, 0, 2));

	PROPERTY_FRAME_END;
}

void renderEntity(Engine::Registry64& registry, Engine::Registry64::entity_type entity, Engine::Registry64::component_type index, const IRef<Comp::Transform>& component) {

	ECS_PROPERTY_FRAME_START(registry, index);

	Vec3f position = castPositionToVec3f(component->getPosition());
	Vec3f rotation = component->getRotation().asRotationVector();
	DiagonalMat3f scale = component->getScale();
	bool standalone = component->isRootPart();

	PROPERTY_DESC("Standalone", "Whether the transform and scale is coming from the part", ImGui::Checkbox("##TransformHitbox", &standalone));

	if (component->hasOffset()) {
		TITLE("Global CFrame", true);
	}

	PROPERTY_IF_LOCK(
		"Position:",
		ImGui::DragVec3("##TransformPosition", position.data, 0.0f, nullptr, true),
		component->setPosition(castVec3fToPosition(position));
	);

	float rotationSpeeds[] = { 0.02f, 0.02f, 0.02f };
	PROPERTY_IF_LOCK(
		"Rotation:",
		ImGui::DragVec3("##TransformRotation", rotation.data, 0.0f, rotationSpeeds, true),
		component->setRotation(Rotation::fromRotationVector(rotation));
	);

	float min = 0.01f;
	float* mins[] = { &min, &min, &min };
	float scaleSpeeds[] = { 0.01f, 0.01f, 0.01f };
	PROPERTY_IF_LOCK(
		"Scale:", 
		ImGui::DragVec3("##TransformScale", scale.data, 1.0f, scaleSpeeds, true, mins),
		component->setScale(scale);
	);

	if (component->hasOffset()) {
		CFrame relativeCFrame = component->getOffsetCFrame();
		Vec3f relativePosition = relativeCFrame.getPosition();
		Vec3f relativeRotation = relativeCFrame.getRotation().asRotationVector();

		TITLE("Relative CFrame", true);
		PROPERTY_IF_LOCK(
			"Position:",
			ImGui::DragVec3("##TransformRelativePosition", relativePosition.data, 0.0f, nullptr, true),
			component->setPosition(castVec3fToPosition(relativePosition));
		);

		PROPERTY_IF_LOCK(
			"Rotation:",
			ImGui::DragVec3("##TransformRelativeRotation", relativeRotation.data, 0.0f, rotationSpeeds, true),
			component->setRotation(Rotation::fromRotationVector(relativeRotation));
		);
	}

	PROPERTY_FRAME_END;
}

void renderEntity(Engine::Registry64& registry, Engine::Registry64::entity_type entity, Engine::Registry64::component_type index, const IRef<Comp::Hitbox>& component) {
	ECS_PROPERTY_FRAME_START(registry, index);

	bool standalone = component->isPartAttached();
	Shape shape = component->getShape();
	DiagonalMat3f scale = shape.scale;

	TITLE("Hitbox", false);
	PROPERTY_DESC("Standalone", "Whether the hitbox is coming from the part", ImGui::Checkbox("##HitboxStandalone", &standalone));
	PROPERTY("Volume:", ImGui::Text(str(shape.getVolume()).c_str()));
	PROPERTY("Center of mass:", ImGui::Text(str(shape.getCenterOfMass()).c_str()));

	float min = 0.01f;
	float* mins[] = { &min, &min, &min };
	PROPERTY_IF_LOCK("Scale:", ImGui::DragVec3("HitboxScale", scale.data, 1, nullptr, true, mins), component->setScale(scale););

	TITLE("Bounding box", true);
	PROPERTY("Width:", ImGui::Text(str(shape.getWidth()).c_str()));
	PROPERTY("Height:", ImGui::Text(str(shape.getHeight()).c_str()));
	PROPERTY("Depth:", ImGui::Text(str(shape.getHeight()).c_str()));
	PROPERTY("Max radius:", ImGui::Text(str(shape.getMaxRadius()).c_str()));

	PROPERTY_FRAME_END;
}

void renderEntity(Engine::Registry64& registry, Engine::Registry64::entity_type entity, Engine::Registry64::component_type index, const IRef<Comp::Attachment>& component) {
	ECS_PROPERTY_FRAME_START(registry, index);

	bool isAttachmentToMainPart = !component->isAttachmentToMainPart;
	Vec3f attachmentPosition = component->attachment->position;
	Vec3f attachmentRotation = component->attachment->rotation.asRotationVector();
	bool isEditingFrame = registry.has<Comp::Transform>(entity);

	PROPERTY(
		"Is attachment to the main part:",
		ImGui::Checkbox("##ToMainPart", &isAttachmentToMainPart);
	);

	if (isEditingFrame) {
		PROPERTY_DESC(
			"Position:", 
			"Use the tranform component to edit the position", 
			ImGui::Text("(%f, %f, %f)", attachmentPosition.x, attachmentPosition.y, attachmentPosition.z)
		);
		PROPERTY_DESC(
			"Rotation:",
			"Use the tranform component to edit the rotation",
			ImGui::Text("(%f, %f, %f)", attachmentRotation.x, attachmentRotation.y, attachmentRotation.z);
		);
	} else {
		PROPERTY_IF_LOCK(
			"Position:",
			ImGui::DragVec3("##AttachmentPosition", attachmentPosition.data, 0.0f, nullptr, true),
			component->setAttachment(CFrame(attachmentPosition, Rotation::fromRotationVector(attachmentRotation)));
		);

		float rotationSpeeds[] = { 0.02f, 0.02f, 0.02f };
		PROPERTY_IF_LOCK(
			"Rotation:",
			ImGui::DragVec3("##AttachmentRotation", attachmentRotation.data, 0.0f, rotationSpeeds, true),
			component->setAttachment(CFrame(attachmentPosition, Rotation::fromRotationVector(attachmentRotation)));
		);

		PROPERTY_IF(
			"",
			ImGui::Button("Edit"),
			registry.add<Comp::Transform>(entity, component->getMainPart(), component->attachment)->addCallback([] () {Log::debug("Edit"); });
		);
	}

	PROPERTY_FRAME_END;
}

void renderSoftlink(Engine::Registry64& registry, Engine::Registry64::entity_type entity, Comp::SoftLink* component) {
	ExtendedPart* partA = reinterpret_cast<ExtendedPart*>(component->link->attachedPartA.part);
	ExtendedPart* partB = reinterpret_cast<ExtendedPart*>(component->link->attachedPartB.part);
	if (partA == nullptr || partB == nullptr)
		return;

	Vec3f positionA = component->getPositionA();
	Vec3f positionB = component->getPositionB();
	IRef<Comp::Name> nameA = registry.get<Comp::Name>(partA->entity);
	IRef<Comp::Name> nameB = registry.get<Comp::Name>(partB->entity);
	IRef<Comp::Transform> transform = registry.get<Comp::Transform>(entity);

	bool isEditingFrame = transform.valid() && transform->isOffsetStoredRemote();
	bool isEditingFrameA = isEditingFrame && std::get<CFrame*>(transform->offset) == &component->link->attachedPartA.attachment;
	bool isEditingFrameB = isEditingFrame && std::get<CFrame*>(transform->offset) == &component->link->attachedPartB.attachment;
	
	TITLE("Attachment A", false);
	PROPERTY("Part", ImGui::Text(nameA.valid() ? nameA->name.c_str() : "Part A"));
	if (isEditingFrameA) {
		PROPERTY_DESC(
			"Position:",
			"Use the tranform component to edit the position",
			ImGui::Text("(%f, %f, %f)", positionA.x, positionA.y, positionA.z)
		);
	} else {
		PROPERTY_IF_LOCK("Position", ImGui::DragVec3("##TransformPositionA", positionA.data, 0, nullptr, true), component->setPositionA(positionA););
		PROPERTY_IF(
			"",
			ImGui::Button("Edit##EditCFrameA"),
			registry.add<Comp::Transform>(entity, partA, &component->link->attachedPartA.attachment); SelectionTool::selection.recalculateSelection();
		);
	}

	TITLE("Attachment B", true);
	PROPERTY("Part", ImGui::Text(nameB.valid() ? nameB->name.c_str() : "Part B"));
	if (isEditingFrameB) {
		PROPERTY_DESC(
			"Position:",
			"Use the tranform component to edit the position",
			ImGui::Text("(%f, %f, %f)", positionB.x, positionB.y, positionB.z)
		);
	} else {
		PROPERTY_IF_LOCK("Position", ImGui::DragVec3("##TransformPositionB", positionB.data, 0, nullptr, true), component->setPositionB(positionB););
		PROPERTY_IF(
			"",
			ImGui::Button("Edit##EditCFrameB"),
			registry.add<Comp::Transform>(entity, partB, &component->link->attachedPartB.attachment); SelectionTool::selection.recalculateSelection();
		);
	}
}

void renderEntity(Engine::Registry64& registry, Engine::Registry64::entity_type entity, Engine::Registry64::component_type index, const IRef<Comp::MagneticLink>& component) {
	ECS_PROPERTY_FRAME_START(registry, index);

	renderSoftlink(registry, entity, component.get());

	MagneticLink* link = dynamic_cast<MagneticLink*>(component->link);
	float magneticStrength = static_cast<float>(link->magneticStrength);

	TITLE("Properties", true);
	PROPERTY_IF_LOCK(
		"Magnetic strength",
		ImGui::DragFloat("##MagneticStrength", &magneticStrength, 0, 0.1f, true),
		link->magneticStrength = static_cast<double>(magneticStrength);
	);

	PROPERTY_FRAME_END;
}

void renderEntity(Engine::Registry64& registry, Engine::Registry64::entity_type entity, Engine::Registry64::component_type index, const IRef<Comp::ElasticLink>& component) {
	ECS_PROPERTY_FRAME_START(registry, index);

	renderSoftlink(registry, entity, component.get());

	ElasticLink* link = dynamic_cast<ElasticLink*>(component->link);
	float restLenght = static_cast<float>(link->restLength);
	float stiffness = static_cast<float>(link->stiffness);

	TITLE("Properties", true);
	PROPERTY_IF_LOCK("Length", ImGui::DragFloat("##RestLength", &restLenght, 0, 0.1f, true), link->restLength = static_cast<double>(restLenght););
	PROPERTY_IF_LOCK("Stiffness", ImGui::DragFloat("##Stiffness", &stiffness, 0, 0.1f, true), link->stiffness = static_cast<double>(stiffness););

	PROPERTY_FRAME_END;
}

void renderEntity(Engine::Registry64& registry, Engine::Registry64::entity_type entity, Engine::Registry64::component_type index, const IRef<Comp::SpringLink>& component) {
	ECS_PROPERTY_FRAME_START(registry, index);

	renderSoftlink(registry, entity, component.get());

	SpringLink* link = dynamic_cast<SpringLink*>(component->link);
	float restLenght = static_cast<float>(link->restLength);
	float stiffness = static_cast<float>(link->stiffness);

	TITLE("Properties", true);
	PROPERTY_IF_LOCK("Length", ImGui::DragFloat("##RestLength", &restLenght, 0, 0.1f, true), link->restLength = static_cast<double>(restLenght););
	PROPERTY_IF_LOCK("Stiffness", ImGui::DragFloat("##Stiffness", &stiffness, 0, 0.1f, true), link->stiffness = static_cast<double>(stiffness););

	PROPERTY_FRAME_END;
}

void renderEntity(Engine::Registry64& registry, Engine::Registry64::entity_type entity, Engine::Registry64::component_type index, const IRef<Comp::AlignmentLink>& component) {
	ECS_PROPERTY_FRAME_START(registry, index);

	renderSoftlink(registry, entity, component.get());

	AlignmentLink* link = dynamic_cast<AlignmentLink*>(component->link);

	PROPERTY_FRAME_END;
}

void renderEntity(Engine::Registry64& registry, Engine::Registry64::entity_type entity, Engine::Registry64::component_type index, const IRef<Comp::FixedConstraint>& component) {
	ECS_PROPERTY_FRAME_START(registry, index);

	FixedConstraint* link = dynamic_cast<FixedConstraint*>(component->hardConstraint->constraintWithParent.get());

	CFrame* parentCFrame = component->getParentAttachment();
	Vec3f parentPosition = parentCFrame->position;
	Vec3f parentRotation = parentCFrame->rotation.asRotationVector();

	CFrame* childCFrame = component->getChildAttachment();
	Vec3f childPosition = childCFrame->position;
	Vec3f childRotation = childCFrame->rotation.asRotationVector();

	//IRef<Comp::Name> nameA = registry.get<Comp::Name>(component->hardConstraint->constraintWithParent->);
	//IRef<Comp::Name> nameB = registry.get<Comp::Name>(partB->entity);

	TITLE("Parent", true);
	PROPERTY("Part", ImGui::Text("Part A"));
	PROPERTY_IF_LOCK(
		"Position",
		ImGui::DragVec3("##ParentPosition", parentPosition.data, 0, nullptr, true),
		parentCFrame->position = parentPosition;
	);
	PROPERTY_IF_LOCK(
		"Rotation",
		ImGui::DragVec3("##ParentRotation", parentRotation.data, 0, nullptr, true),
		parentCFrame->rotation = Rotation::fromRotationVector(parentRotation);
	);

	TITLE("Child", true);
	PROPERTY("Part", ImGui::Text("Part B"));
	PROPERTY_IF_LOCK(
		"Position",
		ImGui::DragVec3("##ChildPosition", childPosition.data, 0, nullptr, true),
		childCFrame->position = childPosition;
	);
	PROPERTY_IF_LOCK(
		"Rotation",
		ImGui::DragVec3("##ChildRotation", childRotation.data, 0, nullptr, true),
		childCFrame->rotation = Rotation::fromRotationVector(childRotation);
	);

	PROPERTY_FRAME_END;
}


void PropertiesFrame::onInit(Engine::Registry64& registry) { }

void PropertiesFrame::onRender(Engine::Registry64& registry) {
	ImGui::Begin("Properties");

	if (SelectionTool::selection.empty()) {
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

	if (SelectionTool::selection.isMultiSelection()) {
		std::string label = "Multiple entities selected";
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
	Engine::Registry64::entity_type selectedEntity = SelectionTool::selection.first().value();
	auto components = registry.getComponents(selectedEntity);
	for (auto [index, component] : components) {
		ENTITY_DISPATCH_START(index);
		ENTITY_DISPATCH(registry, selectedEntity, index, Comp::Name, component);
		ENTITY_DISPATCH(registry, selectedEntity, index, Comp::Transform, component);
		ENTITY_DISPATCH(registry, selectedEntity, index, Comp::Collider, component);
		ENTITY_DISPATCH(registry, selectedEntity, index, Graphics::Comp::Mesh, component);
		ENTITY_DISPATCH(registry, selectedEntity, index, Graphics::Comp::Material, component);
		ENTITY_DISPATCH(registry, selectedEntity, index, Comp::Light, component);
		ENTITY_DISPATCH(registry, selectedEntity, index, Comp::Hitbox, component);
		ENTITY_DISPATCH(registry, selectedEntity, index, Comp::Attachment, component);
		ENTITY_DISPATCH(registry, selectedEntity, index, Comp::MagneticLink, component);
		ENTITY_DISPATCH(registry, selectedEntity, index, Comp::SpringLink, component);
		ENTITY_DISPATCH(registry, selectedEntity, index, Comp::ElasticLink, component);
		ENTITY_DISPATCH(registry, selectedEntity, index, Comp::AlignmentLink, component);
		ENTITY_DISPATCH(registry, selectedEntity, index, Comp::FixedConstraint, component);
		ENTITY_DISPATCH_END(registry, selectedEntity, index, component);
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
					registry.remove(selectedEntity, deletedComponentIndex);
				}
			}
		}
		if (ImGui::MenuItem("Component info")) { }
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
