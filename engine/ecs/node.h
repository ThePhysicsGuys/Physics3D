#pragma once

#include "component.h"

namespace Engine {

class ECSTree;

class Node {
protected:
	/*
		The tree that this node belongs to
	*/
	ECSTree* tree;

	/*
		The parent of this node
	*/
	Node* parent;

	/*
		The name of this node
	*/
	std::string name;

	/*
		The children of this entity
	*/
	std::vector<Node*> children;

public:
	/*
		Creates a new node with the given name
	*/
	Node(const std::string& name);

	/*
		Returns the children of this node
	*/
	std::vector<Node*> getChildren();

	/*
		Returns the parent of this node
	*/
	Node* getParent();

	/*
		Sets the parent of this node to the given value
	*/
	void setParent(Node* parent);

	/*
		Returns the name of this node
	*/
	std::string getName();

	/*
		Sets the name of this node to the given value
	*/
	void setName(const std::string& name);

	/*
		Gets the tree of this node
	*/
	ECSTree* getTree();

	/*
		Sets the tree of this node to the given value
	*/
	void setTree(ECSTree* tree);

	/*
		Adds the given child to the children
	*/
	virtual void addChild(Node* child);

	/*
		Removes the given child from the children
	*/
	virtual void removeChild(Node* child);

	/*
		Returns whether the children recursively contain the given node using depth first search
	*/
	bool containsChild(Node* child);

};

};

	/*
		Returns the first component of the given type in the parent of this entity if present, nullptr otherwise
	*/
	/*template<typename T, typename = std::enable_if<std::is_base_of<Component, T>::value>>
	T* getComponentInParent() {
		if (parent == nullptr)
			return nullptr;

		return parent->getComponent<T>();
	}*/

	/*
		Returns all the components of the given type in the parent of this entity if present, nullptr otherwise
	*/
	/*template<typename T, typename = std::enable_if<std::is_base_of<Component, T>::value>>
	std::vector<T*> getComponentsInParent() {
		if (parent == nullptr)
			return std::vector<T*>();

		return parent->getComponents<T>();
	}*/

	//	/*
	//		Returns the component of the given type in any of its children using depth first search if present, nullptr otherwise
	//	*/
	//template<typename T, typename = std::enable_if<std::is_base_of<Component, T>::value>>
	//T* getComponentInChildren() {
	//	for (Entity* child : children) {
	//		T* component = child->getComponent<T>();
	//
	//		if (component != nullptr)
	//			return component;
	//
	//		T* component = child->getComponentInChildren<T>();
	//
	//		if (component != nullptr)
	//			return component;
	//	}
	//
	//	return nullptr;
	//}

	///*
	//		Returns all components of the given type in any of its children using depth first search if present, nullptr otherwise
	//	*/
	//template<typename T, typename = std::enable_if<std::is_base_of<Component, T>::value>>
	//std::vector<T*> getComponentsInChildren() {
	//	std::vector<T*> results;
	//
	//	for (Entity* child : children) {
	//		for (T* component : child->getComponents<T>())
	//			results.push_back(component);
	//
	//		for (T* component : child->getComponentsInChildren<T>())
	//			results.push_back(component);
	//	}
	//
	//	return results;
	//}