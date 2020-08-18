#pragma once

#include "component.h"
#include "entity.h"
#include "node.h"

namespace P3D::Engine {
	
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
	Node* createGroup(Node* root, const std::string& name = "Empty folder");

	/*
		Adds an empty entity with the given name to the given root
	*/
	Entity* createEntity(Node* root, const std::string& name = "Empty entity");

	/*
		Adds the given node to the given root
	*/
	void addNode(Node* root, Node* node);

	/*
		Removes the given node from the tree
	*/
	void removeNode(Node* node, bool deleteChildren = true);

	/*
		Returns all entities with a component of the given type
	*/
	std::multimap<std::string, Entity*> getEntitiesWithComponent(const ComponentType& type);
};

};