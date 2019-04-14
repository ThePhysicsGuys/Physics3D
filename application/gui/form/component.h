#pragma once

#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include "../engine/math/vec2.h"

class Component {
public:

	/*
		Layout of the component's children
		FLOW:	The children are placed next to eachother until they fill the width of the container,
				much like lines of text in a paragraph
	*/
	enum class Layout {
		FLOW = 0,
	};

	/* 
		Alignment of the component within its current layout
		FLOW:
			FILL:	The component fills up the remaining width of the parent container
	*/
	enum class Align {
		FILL = 0,
		RELATIVE = 1
	};

	/* Parent of this component */
	Component* parent;

	/* Layout of this component, this determines the positioning and dimensioning of the component */
	Layout layout;

	/* Determines if the component has a fixed size */
	bool resizing;

	/* 
		The topleft edge of the container, offsets not included
		This property can be altered by its parent
	*/
	Vec2 position;

	/* 
		The size of the container
		This property can be altered by its parent
	*/
	Vec2 dimension;

	Component(Vec2 position) : position(position), dimension(Vec2(0)), layout(Layout::FLOW), resizing(true) {};
	Component(Vec2 position, Vec2 dimension) : position(position), dimension(dimension), layout(Layout::FLOW), resizing(false) {};
	
	/* Returns the minimal size of the container */
	virtual Vec2 resize() = 0;
	
	/* Renders the component without resizing it */
	virtual void render() = 0;
};