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
		this->components.insert({ component->getType(), component });
}

#pragma endregion

#pragma region components

void Entity::addComponent(Component* component) {
	if (component == nullptr)
		return;

	component->setEntity(this);
	
	if (containsComponentOfType(component)) {
		if (component->isUnique()) 
			overwriteComponent(component);
		else
			components.insert({ component->getType(), component });
	} else {
		components.insert({ component->getType(), component });

		if (tree != nullptr)
			tree->onAddComponentToEntity(this, component);
	}
}

void Entity::removeComponent(Component* component) {
	if (component == nullptr)
		return;
	
	auto result = components.equal_range(component->getType());

	for (auto iterator = result.first; iterator != result.second; ++iterator) {
		if (iterator->second == component) {
			components.erase(iterator);
			component->entity = nullptr;
			break;
		}
	}

	if (tree != nullptr) {
		if (!containsComponentOfType(component))
			tree->onRemoveComponentFromEntity(this, component);
	}
}

bool Entity::containsComponent(Component* component) {
	if (component == nullptr)
		return false;

	auto result = components.equal_range(component->getType());

	for (auto iterator = result.first; iterator != result.second; ++iterator) {
		if (iterator->second == component) 
			return true;
	}

	return false;
}

bool Entity::containsComponentOfType(Component* component) {
	if (component == nullptr)
		return false;

	return containsComponentOfType(component->getType());
}

bool Entity::containsComponentOfType(const ComponentType& type) {
	return components.find(type) != components.end();
}

void Entity::overwriteComponent(Component* query) {
	if (query == nullptr)
		return;
	auto iterator = components.lower_bound(query->getType());
	
	if (iterator != components.end()) {
		Component* component = iterator->second;
		// todo remove component from memory?
		components.erase(iterator);
		component->entity = nullptr;
		addComponent(query);
	}
}

#pragma endregion

#pragma region virtual functions

void Entity::addChild(Node* child) {
	Node::addChild(child);
	// todo add constraint
}

void Entity::removeChild(Node* child) {
	Node::removeChild(child);
	// todo remove constraint
}

void Entity::setTree(ECSTree* tree) {
	Node::setTree(tree);

	if (this->tree != nullptr) {
		for (auto iterator : components) {
			tree->onAddComponentToEntity(this, iterator.second);
		}
	}
}

#pragma endregion


};