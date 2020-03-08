#pragma once

#include "component.h"
#include "entity.h"
#include "node.h"

namespace Engine {
	
class ECSTree {
	friend Entity;
private:
	/*
		The invisible root of the tree
	*/
	Node* root;

	/*
		The mapping from type to a multimap of entities containing a component of that type
	*/
	std::unordered_map<ComponentType, std::multimap<std::string, Entity*>> map;

	/*
		Adds a mapping from the given component type to the given entity
	*/
	void onAddComponentToEntity(Entity* entity, Component* component);

	/*
		Removes a mapping from the given component type to the given entity
	*/
	void onRemoveComponentFromEntity(Entity* entity, Component* component);

public:
	/*
		Creates a tree with an empty root
	*/
	ECSTree();

	/*
		Returns the root of this tree
	*/
	Node* getRoot();

	/*
		Adds an empty group with the given name to the given root
	*/
	Node* addGroup(Node* root, const std::string& name);

	/*
		Adds an empty entity with the given name to the given root
	*/
	Entity* addEntity(Node* root, const std::string& name);

	/*
		Adds the given node to the given root
	*/
	void addNode(Node* root, Node* node);

	/*
		Returns all entities with a component of the given type
	*/
	std::multimap<std::string, Entity*> getEntitiesWithComponent(const ComponentType& type);
};

};