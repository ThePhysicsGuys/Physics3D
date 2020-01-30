#include "core.h"
#include "entity.h"

namespace Application {


Entity::Entity() {
	parent = nullptr;
}


Entity::Entity(std::initializer_list<Component*> components) : Entity() {
	for (Component* component : components)
		this->components.insert(std::pair<ComponentType, Component*>(component->getType(), component));
}

void Entity::addComponent(Component* component) {
	if (component == nullptr)
		return;

	if (component->isUnique()) {
		if (containsComponentOfType(component)) {
			overwriteComponent(component);
		}
	}

	component->setEntity(this);
	components.insert(std::pair<ComponentType, Component*>(component->getType(), component));
}

void Entity::removeComponent(Component* component) {
	auto result = components.equal_range(component->getType());

	for (auto iterator = result.first; iterator != result.second; ++iterator) {
		if (iterator->second == component) {
			components.erase(iterator);
			component->setEntity(nullptr);
			return;
		}
	}
}

bool Entity::containsComponent(Component* component) {
	auto result = components.equal_range(component->getType());

	for (auto iterator = result.first; iterator != result.second; ++iterator) {
		if (iterator->second == component) 
			return true;
	}

	return false;
}

bool Entity::containsComponentOfType(Component* query) {
	if (query == nullptr)
		return false;

	return containsComponentOfType(query->getType());
}

bool Entity::containsComponentOfType(const ComponentType& type) {
	return components.find(type) != components.end();
}

void Entity::overwriteComponent(Component* query) {
	auto iterator = components.lower_bound(query->getType());
	
	if (iterator != components.end()) {
		Component* component = iterator->second;
		components.erase(iterator);
		component->setEntity(nullptr);
		addComponent(query);
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

};