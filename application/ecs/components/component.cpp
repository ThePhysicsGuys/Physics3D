#include "core.h"
#include "component.h"

#include "../entity.h"

namespace Application {

Entity* Component::getEntity() const {
	return entity;
}

void Component::setEntity(Entity* entity) {
	if (this->entity != nullptr)
		this->entity->removeComponent(this);

	this->entity = entity;
}

};