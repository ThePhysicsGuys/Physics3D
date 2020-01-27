#pragma once

namespace Application {

class Entity;

enum class ComponentType {
	Transform,
	Material,
	Mesh,
	Model,
	Light
};

#define DEFINE_COMPONENT(type, unique) \
	inline static ComponentType getStaticType() { return ComponentType::type; } \
	inline virtual ComponentType getType() const override { return getStaticType(); } \
	inline virtual std::string getTypeName() const override { return #type; } \
	inline virtual bool isUnique() const override { return unique; }

/*
	A class representing a Component of an Entity
*/
class Component {
private:
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
	inline virtual ComponentType getType() const = 0;
	
	/*
		Returns the typename of this Component
	*/
	inline virtual std::string getTypeName() const = 0;

	/*
		Returns whether an Entity can contain multiple instances of this Component
	*/
	inline virtual bool isUnique() const = 0;
};

};