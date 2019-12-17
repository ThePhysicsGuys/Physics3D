#pragma once

class Entity;

class Component {
private:
	Entity* entity;

public:
	/*
		Returns whether an entity can contain multiple instances of this component
	*/
	virtual bool isUnique() = 0;

	/*
		Returns the entity of this component
	*/
	Entity* getEntity();

	/*
		Sets the entity of this component to the given value
	*/
	void setEntity(Entity* entity);
};

