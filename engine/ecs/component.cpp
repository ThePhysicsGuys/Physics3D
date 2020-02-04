#include "core.h"
#include "component.h"

#include "../engine/ecs/entity.h"

namespace Engine {

Component* Component::defaultComponent;

Entity* Component::getEntity() const {
	return entity;
}

void Component::setEntity(Entity* entity) {
	if (this->entity != nullptr)
		this->entity->removeComponent(this);

	this->entity = entity;
}

};