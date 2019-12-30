#pragma once

namespace Application {

class Entity;

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
		Returns whether an Entity can contain multiple instances of this Component
	*/
	virtual bool isUnique() const = 0;
};

};