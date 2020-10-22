#include "core.h"

#include "application.h"
#include "extendedPart.h"
#include "ecs/components.h"
#include "view/screen.h"
#include "../engine/ecs/registry.h"
#include "../physics/misc/serialization.h"
#include "../engine/meshRegistry.h"

namespace P3D::Application {

ExtendedPart::ExtendedPart(Part&& part, const VisualData& visualData, const std::string& name, const Entity& parent) : Part(std::move(part)) {
	if (this->entity == screen.registry.null_entity)
		this->entity = screen.registry.create();

	if (parent != screen.registry.null_entity)
		screen.registry.setParent(this->entity, parent);
	
	screen.registry.add<Comp::Mesh>(this->entity, visualData);
	screen.registry.add<Comp::Model>(this->entity, this);
	screen.registry.add<Comp::Transform>(this->entity, this);
	if (!name.empty()) 
		screen.registry.add<Comp::Tag>(this->entity, name);
}

ExtendedPart::ExtendedPart(const Shape& hitbox, const GlobalCFrame& position, const PartProperties& properties, const VisualData& visualData, const std::string& name, const Entity& parent) : Part(hitbox, position, properties) {
	if (this->entity == screen.registry.null_entity)
		this->entity = screen.registry.create();

	if (parent != screen.registry.null_entity)
		screen.registry.setParent(this->entity, parent);
	
	screen.registry.add<Comp::Mesh>(this->entity, visualData);
	screen.registry.add<Comp::Model>(this->entity, this);
	screen.registry.add<Comp::Transform>(this->entity, this);
	if (!name.empty())
		screen.registry.add<Comp::Tag>(this->entity, name);
}

ExtendedPart::ExtendedPart(Part&& part, const std::string& name, const Entity& parent) : Part(std::move(part)) {
	if (this->entity == screen.registry.null_entity)
		this->entity = screen.registry.create();

	if (parent != screen.registry.null_entity)
		screen.registry.setParent(this->entity, parent);
	
	screen.registry.add<Comp::Mesh>(this->entity, Engine::MeshRegistry::getOrCreateMeshFor(part.hitbox.baseShape));
	screen.registry.add<Comp::Model>(this->entity, this);
	screen.registry.add<Comp::Transform>(this->entity, this);
	if (!name.empty())
		screen.registry.add<Comp::Tag>(this->entity, name);
}
ExtendedPart::ExtendedPart(const Shape& hitbox, const GlobalCFrame& position, const PartProperties& properties, const std::string& name, const Entity& parent) : Part(hitbox, position, properties) {
	if (this->entity == screen.registry.null_entity)
		this->entity = screen.registry.create();

	if (parent != screen.registry.null_entity)
		screen.registry.setParent(this->entity, parent);
	
	screen.registry.add<Comp::Mesh>(this->entity, Engine::MeshRegistry::getOrCreateMeshFor(hitbox.baseShape));
	screen.registry.add<Comp::Model>(this->entity, this);
	screen.registry.add<Comp::Transform>(this->entity, this);
	if (!name.empty())
		screen.registry.add<Comp::Tag>(this->entity, name);
}

ExtendedPart::ExtendedPart(const Shape& hitbox, ExtendedPart* attachTo, const CFrame& attach, const PartProperties& properties, const std::string& name, const Entity& parent) : Part(hitbox, *attachTo, attach, properties) {
	if (this->entity == screen.registry.null_entity)
		this->entity = screen.registry.create();

	if (parent != screen.registry.null_entity)
		screen.registry.setParent(this->entity, parent);

	screen.registry.add<Comp::Mesh>(this->entity, Engine::MeshRegistry::getOrCreateMeshFor(hitbox.baseShape));
	screen.registry.add<Comp::Model>(this->entity, this);
	screen.registry.add<Comp::Transform>(this->entity, this);
	if (!name.empty())
		screen.registry.add<Comp::Tag>(this->entity, name);
}

void ExtendedPart::setMaterial(const Comp::Material& material) {
	screen.registry.add<Comp::Material>(this->entity, material);
}

void ExtendedPart::setName(const std::string& name) {
	screen.registry.add<Comp::Tag>(this->entity, name);
}
	
};