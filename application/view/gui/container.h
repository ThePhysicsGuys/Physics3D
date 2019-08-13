#pragma once

#include "orderedVector.h"
#include "component.h"

#include "../engine/math/vec.h"

class Layout;

class Container : public Component {
public:
	/*
		Layout of the container's children
		FLOW:	The children are placed next to eachother until they fill the width of the container,
				much like lines of text in a paragraph
	*/
	Layout* layout;

	/*
		Children of this container, ordered by z-index
	*/
	OrderedVector<std::pair<Component*, Align>> children;

	/*
		Constructors
	*/
	Container(Vec2 position);
	Container(Vec2 position, Vec2 dimension);
	Container(double x, double y, double width, double height);
	Container(double x, double y);

	/*
		Adds the child to the end of the container
	*/
	void add(Component* child);

	/*
		Adds the child to the end of the container with a given alignment
	*/
	void add(Component* child, Align alignment);

	/*
		Returns the child with its alignment 
	*/
	std::pair<Component*, Align> get(Component* child);

	/*
		Removes the given child from the children
	*/
	void remove(Component* child);

	/*
		Returns the intersected component at the given point.
	*/
	virtual Component* intersect(Vec2 point);

	/*
		Renders the children of the container
	*/
	virtual void renderChildren();

	virtual void render() = 0;
	virtual Vec2 resize() = 0;
};