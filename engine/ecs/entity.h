#pragma once
#include <typeinfo>

#include "component.h"

namespace Engine {

class Entity {
private:
	/*
		The parent of this entity
	*/
	Entity* parent;

	/*
		The children of this entity
	*/
	std::vector<Entity*> children;

	/*
		The components of this entity
	*/
	std::multimap<ComponentType, Component*> components;

public:
	/*
		Creates an empty entity
	*/
	Entity();

	/*
		Creates an entity with the given components
	*/
	Entity(std::initializer_list<Component*> components);

	/*
		Returns the parent of this entity
	*/
	Entity* getParent();

	/*
		Sets the parent of this entity to the given value
	*/
	void setParent(Entity* parent);

	/*
		Returns the children of this entity
	*/
	std::vector<Entity*> getChildren();

	/*
		Adds the given child to the children
	*/
	void addChild(Entity* child);

	/*
		Removes the given child from the children
	*/
	void removeChild(Entity* child);

	/*
		Returns whether the children recursively contain the given entity using depth first search
	*/
	bool containsChild(Entity* child);

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
		Returns the component of the given type in any of its children using depth first search if present, nullptr otherwise
	*/
	template<typename T, typename = std::enable_if<std::is_base_of<Component, T>::value>>
	T* getComponentInChildren() {
		for (Entity* child : children) {
			T* component = child->getComponent<T>();

			if (component != nullptr)
				return component;

			T* component = child->getComponentInChildren<T>();

			if (component != nullptr)
				return component;
		}

		return nullptr;
	}

	/*
		Returns the first component of the given type in the parent of this entity if present, nullptr otherwise
	*/
	template<typename T, typename = std::enable_if<std::is_base_of<Component, T>::value>>
	T* getComponentInParent() {
		if (parent == nullptr)
			return nullptr;

		return parent->getComponent<T>();
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

	/*
		Returns all components of the given type in any of its children using depth first search if present, nullptr otherwise
	*/
	template<typename T, typename = std::enable_if<std::is_base_of<Component, T>::value>>
	std::vector<T*> getComponentsInChildren() {
		std::vector<T*> results;

		for (Entity* child : children) {
			for (T* component : child->getComponents<T>())
				results.push_back(component);

			for (T* component : child->getComponentsInChildren<T>())
				results.push_back(component);
		}

		return results;
	}

	/*
		Returns all the components of the given type in the parent of this entity if present, nullptr otherwise
	*/
	template<typename T, typename = std::enable_if<std::is_base_of<Component, T>::value>>
	std::vector<T*> getComponentsInParent() {
		if (parent == nullptr)
			return std::vector<T*>();

		return parent->getComponents<T>();
	}
};

};