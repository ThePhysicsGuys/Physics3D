#include "core.h"

#include "tree.h"

namespace Engine {

void Tree::onAddComponentToEntity(Entity* entity, Component* component) {
	if (component == nullptr || entity == nullptr)
		return;

	auto iterator = map.find(component->getType());

	if (iterator == map.end()) {
		std::multimap<std::string, Entity*> entities = {{ entity->getName(), entity }};
		map.insert({ component->getType(), entities });
	} else {
		iterator->second.insert({ entity->getName(), entity });
	}
}

void Tree::onRemoveComponentFromEntity(Entity* entity, Component* component) {
	if (component == nullptr || entity == nullptr)
		return;

	auto iterator = map.find(component->getType());
	if (iterator == map.end()) {
		return;
	} else {
		auto& list = iterator->second;
		auto range = list.equal_range(entity->getName());
		
		for (auto element = range.first; element != range.second; ++element) {
			if (element->second == entity) {
				list.erase(element);
				return;
			}
		}
	}
}

Tree::Tree() {
	root = new Node("Tree");
}

Node* Tree::getRoot() {
	return root;
}

Node* Tree::addGroup(Node* root, const std::string& name) {
	if (root == nullptr)
		return nullptr;

	Node* group = new Node(name);
	root->addChild(group);
}

Entity* Tree::addEntity(Node* root, const std::string& name) {
	if (root == nullptr)
		return nullptr;

	Entity* entity = new Entity(name);
	root->addChild(entity);
}

std::multimap<std::string, Entity*> Tree::getEntitiesWithComponent(const ComponentType& type) {
	auto list = map.find(type);

	if (list == map.end())
		return std::multimap<std::string, Entity*>();

	return list->second;
}

};