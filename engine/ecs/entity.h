#pragma once

#include "node.h"
#include "component.h"

namespace P3D::Engine {

class Entity : public Node {
protected:
	/*
		The components of this entity
	*/
	std::multimap<ComponentType, Component*> components;

public:
#pragma region constructors
	/*
		Creates an empty entity
	*/
	Entity(const std::string& name);

	/*
		Creates an entity with the given components
	*/
	Entity(const std::string& name, std::initializer_list<Component*> components);
#pragma endregion

#pragma region components
	/*
		Adds the given component to this entity, if the component is unique and already is part of this entity, the component will overwrite the existing one
	*/
	void addComponent(Component* component);

	/*
		Removes the given component from this entity
	*/
	void removeComponent(Component* component);

	/*
		Returns whether the given component is part of this entity
	*/
	bool containsComponent(Component* component);

	/*
		Returns whether this entity has a component of the same type as the given component
	*/
	bool containsComponentOfType(Component* component);

	/*
		Returns whether this entity has a component of the given type
	*/
	bool containsComponentOfType(const ComponentType& type);

	/*
		Gets the component of this entity which has the same type as the given component and overwrites the value
	*/
	void overwriteComponent(Component* component);

	/*
		Returns the first component of the given type in this entity if present, nullptr otherwise
	*/
	template<typename T, typename = std::enable_if<std::is_base_of<Component, T>::value>>
	T* getComponent() {
		auto iterator = components.find(T::getStaticType());

		return (iterator != components.end()) ? static_cast<T*>(iterator->second) : nullptr;
	}

	/*
		Returns the first component of the given type in this entity if present, default value otherwise
	*/
	template<typename T, typename = std::enable_if<std::is_base_of<Component, T>::value>>
	T* getComponentOrDefault() {
		auto iterator = components.find(T::getStaticType());

		return (iterator != components.end()) ? static_cast<T*>(iterator->second) : static_cast<T*>(T::getDefaultComponent());
	}

	/*
		Returns all components of the given type in this entity if present, nullptr otherwise
	*/
	template<typename T, typename = std::enable_if<std::is_base_of<Component, T>::value>>
	std::vector<T*> getComponents() {
		std::vector<T*> results;

		auto result = components.equal_range(T::getStaticType());
		for (auto iterator = result.first; iterator != result.second; ++iterator)
			results.push_back(iterator->second);
		
		return results;
	}
#pragma endregion

#pragma region virtual functions
	/*
		Adds the given child to the children
	*/
	virtual void addChild(Node* child) override;

	/*
		Removes the given child from the children
	*/
	virtual void removeChild(Node* child) override;

	/*
		Sets the tree of this node to the given value
	*/
	virtual void setTree(ECSTree* tree) override;
#pragma endregion
};

};