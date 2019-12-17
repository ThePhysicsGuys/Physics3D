#include "core.h"
#include "entity.h"

#include "component.h"

void Entity::addComponent(Component* component) {
	if (component == nullptr)
		return;

	if (component->isUnique()) {
		if (containsComponentOfType(component)) {
			overwriteComponent(component);
		}
	}

	component->setEntity(this);
	components.push_back(component);

}

void Entity::removeComponent(Component* component) {
	for (auto iterator = components.begin(); iterator != components.end(); ++iterator) {
		if (*iterator == component) {
			components.erase(iterator);
			component->setEntity(nullptr);
			return;
		}
	}
}

bool Entity::containsComponent(Component* query) {
	for (Component* component : components) {
		if (component == query)
			return true;
	}

	return false;
}

bool Entity::containsComponentOfType(Component* query) {
	for (Component* component : components) {
		if (typeid(component) == typeid(query))
			return true;
	}
}

void Entity::overwriteComponent(Component* query) {
	for (Component* component : components) {
		if (typeid(component) == typeid(query)) {
			removeComponent(component);
			addComponent(query);
		}
	}
}

Entity* Entity::getParent() {
	return parent;
}

void Entity::setParent(Entity* parent) {
	if (parent != nullptr && containsChild(parent))
		return;

	if (this->parent != nullptr) 
		this->parent->removeChild(this);

	this->parent = parent;
}

std::vector<Entity*> Entity::getChildren() {
	return children;
}

void Entity::addChild(Entity* child) {
	if (parent == child)
		return;

	if (containsChild(child))
		return;

	children.push_back(child);
}

void Entity::removeChild(Entity* child) {
	for (auto iterator = children.begin(); iterator != children.end(); ++iterator) {
		if (*iterator == child) {
			children.erase(iterator);
			child->setParent(nullptr);
			return;
		}
	}
}

bool Entity::containsChild(Entity* query) {
	for (Entity* child : children) {
		if (child == query)
			return true;

		if (child->containsChild(query))
			return true;
	}

	return false;
}
