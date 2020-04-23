#pragma once

namespace Engine {

class Entity;

enum class ComponentType {
	Transform,
	Material,
	Mesh,
	Model,
	Light
};

#define DEFINE_COMPONENT(type, unique) \
	inline static Engine::Component* getDefaultComponent() { return defaultComponent; } \
	inline static Engine::ComponentType getStaticType() { return Engine::ComponentType::type; } \
	inline virtual Engine::ComponentType getType() const override { return getStaticType(); } \
	inline virtual std::string getTypeName() const override { return #type; } \
	inline virtual bool isUnique() const override { return unique; }

/*
	A class representing a Component of an Entity
*/
class Component {
protected:
	/*
		The default value of this Component
	*/
	static Component* defaultComponent;

private:
	friend Entity;

	/*
		The parent entity of this Component
	*/
	Entity* entity;

public:
	/*
		Returns the entity of this Component
	*/
	Entity* getEntity() const;

	/*
		Sets the entity of this Component to the given value
	*/
	void setEntity(Entity* entity);

	/*
		Returns the type of this Component
	*/
	virtual ComponentType getType() const = 0;
	
	/*
		Returns the typename of this Component
	*/
	virtual std::string getTypeName() const = 0;

	/*
		Returns whether an Entity can contain multiple instances of this Component
	*/
	virtual bool isUnique() const = 0;
};

};