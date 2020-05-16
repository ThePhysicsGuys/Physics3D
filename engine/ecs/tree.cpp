#include "core.h"

#include "tree.h"

namespace Engine {

void ECSTree::onAddComponentToEntity(Entity* entity, Component* component) {
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

void ECSTree::onRemoveComponentFromEntity(Entity* entity, Component* component) {
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

ECSTree::ECSTree() {
	root = new Node("Tree");
	map = std::unordered_map<ComponentType, std::multimap<std::string, Entity*>>();
}

Node* ECSTree::getRoot() {
	return root;
}

Node* ECSTree::createGroup(Node* root, const std::string& name) {
	if (root == nullptr)
		return nullptr;

	Node* group = new Node(name);
	group->setTree(this);

	root->addChild(group);

	return group;
}

Entity* ECSTree::createEntity(Node* root, const std::string& name) {
	if (root == nullptr)
		return nullptr;

	Entity* entity = new Entity(name);
	entity->setTree(this);

	root->addChild(entity);

	return entity;
}

void ECSTree::addNode(Node* root, Node* node) {
	if (root == nullptr)
		return;

	node->setTree(this);

	root->addChild(node);
}

void ECSTree::removeNode(Node* node, bool deleteChildren) {
	node->setTree(nullptr);
		
	if (deleteChildren) {
		// todo implement
	} else {
		for (Node* child : node->getChildren())
			child->setParent(node->getParent());
	}

	node->getParent()->removeChild(node);

	// todo remove links in map
}

std::multimap<std::string, Entity*> ECSTree::getEntitiesWithComponent(const ComponentType& type) {
	auto list = map.find(type);

	if (list == map.end())
		return std::multimap<std::string, Entity*>();

	return list->second;
}

};