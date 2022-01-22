#include "core.h"

#include "application.h"
#include "extendedPart.h"
#include "ecs/components.h"
#include "view/screen.h"
#include "../engine/ecs/registry.h"
#include <Physics3D/misc/serialization/serialization.h>
#include "../graphics/meshRegistry.h"

namespace P3D::Application {

ExtendedPart::ExtendedPart(Part&& part, const Graphics::Comp::Mesh& mesh, const std::string& name, const Entity& parent)
	: Part(std::move(part)) {
	if (this->entity == Engine::Registry64::null_entity)
		this->entity = screen.registry.create();

	if (parent != Engine::Registry64::null_entity)
		screen.registry.setParent(this->entity, parent);

	screen.registry.add<Graphics::Comp::Mesh>(this->entity, mesh);
	screen.registry.add<Comp::Hitbox>(this->entity, this);
	screen.registry.add<Comp::Transform>(this->entity, this);
	if (!name.empty())
		screen.registry.add<Comp::Name>(this->entity, name);
}

ExtendedPart::ExtendedPart(const Shape& hitbox,
                           const GlobalCFrame& position,
                           const PartProperties& properties,
                           const Graphics::Comp::Mesh& mesh,
                           const std::string& name,
                           const Entity& parent)
	: Part(hitbox, position, properties) {
	if (this->entity == Engine::Registry64::null_entity)
		this->entity = screen.registry.create();

	if (parent != Engine::Registry64::null_entity)
		screen.registry.setParent(this->entity, parent);

	screen.registry.add<Graphics::Comp::Mesh>(this->entity, mesh);
	screen.registry.add<Comp::Hitbox>(this->entity, this);
	screen.registry.add<Comp::Transform>(this->entity, this);
	if (!name.empty())
		screen.registry.add<Comp::Name>(this->entity, name);
}

ExtendedPart::ExtendedPart(Part&& part, const std::string& name, const Entity& parent)
	: Part(std::move(part)) {
	if (this->entity == Engine::Registry64::null_entity)
		this->entity = screen.registry.create();

	if (parent != Engine::Registry64::null_entity)
		screen.registry.setParent(this->entity, parent);

	screen.registry.add<Graphics::Comp::Mesh>(this->entity, Graphics::MeshRegistry::getMesh(part.hitbox.baseShape.get()));
	screen.registry.add<Comp::Hitbox>(this->entity, this);
	screen.registry.add<Comp::Transform>(this->entity, this);
	if (!name.empty())
		screen.registry.add<Comp::Name>(this->entity, name);
}

ExtendedPart::ExtendedPart(const Shape& hitbox,
                           const GlobalCFrame& position,
                           const PartProperties& properties,
                           const std::string& name,
                           const Entity& parent)
	: Part(hitbox, position, properties) {
	if (this->entity == Engine::Registry64::null_entity)
		this->entity = screen.registry.create();

	if (parent != Engine::Registry64::null_entity)
		screen.registry.setParent(this->entity, parent);

	screen.registry.add<Graphics::Comp::Mesh>(this->entity, Graphics::MeshRegistry::getMesh(hitbox.baseShape.get()));
	screen.registry.add<Comp::Hitbox>(this->entity, this);
	screen.registry.add<Comp::Transform>(this->entity, this);
	if (!name.empty())
		screen.registry.add<Comp::Name>(this->entity, name);
}

ExtendedPart::ExtendedPart(const Shape& hitbox,
                           ExtendedPart* attachTo,
                           const CFrame& attach,
                           const PartProperties& properties,
                           const std::string& name,
                           const Entity& parent)
	: Part(hitbox, *attachTo, attach, properties) {
	if (this->entity == Engine::Registry64::null_entity)
		this->entity = screen.registry.create();

	if (parent != Engine::Registry64::null_entity)
		screen.registry.setParent(this->entity, parent);

	screen.registry.add<Graphics::Comp::Mesh>(this->entity, Graphics::MeshRegistry::getMesh(hitbox.baseShape.get()));
	screen.registry.add<Comp::Hitbox>(this->entity, this);
	screen.registry.add<Comp::Transform>(this->entity, this);
	if (!name.empty())
		screen.registry.add<Comp::Name>(this->entity, name);
}

ExtendedPart::~ExtendedPart() {
	// have to do the same as Part's destructor here, because if I don't then PlayerWorld tries to update a deleted entity
	this->removeFromWorld();
	if (this->entity != Engine::Registry64::null_entity) 
		screen.registry.destroy(this->entity);
}

void ExtendedPart::setMaterial(const Comp::Material& material) {
	screen.registry.add<Comp::Material>(this->entity, material);
}

void ExtendedPart::setName(const std::string& name) {
	screen.registry.add<Comp::Name>(this->entity, name);
}

void ExtendedPart::setColor(const Graphics::Color& color) {
	screen.registry.getOrAdd<Comp::Material>(this->entity)->albedo = color;
}

Graphics::Color ExtendedPart::getColor() const {
	return screen.registry.get<Comp::Material>(this->entity)->albedo;
}

};
