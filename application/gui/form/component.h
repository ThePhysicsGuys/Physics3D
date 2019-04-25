#pragma once

#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include <cmath>
#include "../engine/math/vec2.h"

#include "layout.h"

class Component {
public:
	/* 
		Alignment of the component within its current layout
		FLOW:
			FILL:	The component fills up the remaining width of the parent container
	*/
	Align align;

	/* Parent of this component */
	Component* parent;

	/* Determines if the component has a fixed size */
	bool resizing;

	/* 
		The topleft edge of the container, padding not included, margin included
		This property can be altered by its parent
	*/
	Vec2 position;

	/* 
		The size of the container, padding not included, margin included
		This property can be altered by its parent
	*/
	Vec2 dimension;

	/* Padding of this component */
	double padding;

	/* Margin of this component */
	double margin;

	/* Determines if this component and its content should be rendered */
	bool visible;

	/* Constructors */
	Component(Vec2 position) : position(position), dimension(Vec2(0)), resizing(true), visible(true) {};
	
	Component(Vec2 position, Vec2 dimension) : position(position), dimension(dimension), resizing(false), visible(true) {};
	
	/* Returns this if the component contains the point */
	virtual Component* intersect(Vec2 point) {
		Vec2 halfDimension = dimension / 2;
		Vec2 center = position + Vec2(halfDimension.x, -halfDimension.y);
		if (fabs(point.x - center.x) < halfDimension.x && fabs(point.y - center.y) < halfDimension.y)
			return this;
		return nullptr;
	}

	/* Returns the minimal size of the container */
	virtual Vec2 resize() = 0;
	
	/* Renders the component without resizing it */
	virtual void render() = 0;

	/* Drag behaviour of this component */
	virtual void drag(Vec2 dmxy) {};

	/* Hover behaviour of this component */
	virtual void hover(Vec2 mxy) {};

	/* Click behaviour of this component */
	virtual void click(Vec2 mxy) {};

	/* Enter behaviour of this component */
	virtual void enter() {};

	/* Exit behaviour of this component */
	virtual void exit() {};
};