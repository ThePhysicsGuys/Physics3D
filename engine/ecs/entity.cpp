#include "core.h"

#include "entity.h"
#include "tree.h"

namespace Engine {

#pragma region constructors

Entity::Entity(const std::string& name) : Node(name) {
	parent = nullptr;
}

Entity::Entity(const std::string& name, std::initializer_list<Component*> components) : Entity(name) {
	for (Component* component : components)
		this->components.insert(std::pair<ComponentType, Component*>(component->getType(), component));
}

#pragma endregion

#pragma region components

void Entity::addComponent(Component* component) {
	if (component == nullptr)
		return;

	component->setEntity(this);
	
	if (containsComponentOfType(component)) {
		if (component->isUnique()) {
			overwriteComponent(component);
		}
	} else {
		components.insert({ component->getType(), component });

		if (tree != nullptr)
			tree->onAddComponentToEntity(this, component);
	}
}

void Entity::removeComponent(Component* component) {
	auto result = components.equal_range(component->getType());

	for (auto iterator = result.first; iterator != result.second; ++iterator) {
		if (iterator->second == component) {
			components.erase(iterator);
			component->setEntity(nullptr);
			break;
		}
	}

	if (!containsComponentOfType(component))
		if (tree != nullptr)
			tree->onRemoveComponentFromEntity(this, component);
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

#pragma endregion

#pragma region children

void Entity::addChild(Node* child) {
	Node::addChild(child);
	// todo add constraint
}

void Entity::removeChild(Node* child) {
	Node::removeChild(child);
	// todo remove constraint
}

#pragma endregion


};