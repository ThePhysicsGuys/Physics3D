#pragma once

#include <string>

#include "layout.h"
#include "gui.h"

#include "../engine/math/linalg/vec.h"

class Component {
public:
	/* 
		Alignment of the component within its current layout
		FLOW:
			FILL:	The component fills up the remaining width of the parent container
			RELATIVE: The components are placed next to eachother, filling up the container
			CENTER: The same as fill but the filled components are centered
	*/
	Align align = Align::RELATIVE;

	/* 
		Parent of this component 
	*/
	Component* parent = nullptr;

	/* 
		Determines if the component has a fixed size 
	*/
	bool resizing = true;

	/*
		Determines if the component is disabled, the component will still be
		rendered, but will not be interactable.
	*/
	bool disabled = false;

	/* 
		The topleft edge of the container, margin not included, padding included
		This property can be altered by its parent
	*/
	union {
		struct { 
			double x; 
			double y;
		};
		Vec2 position;
	};

	/* 
		The size of the container, margin not included, padding included
		This property can be altered by its parent
	*/
	union {
		struct { 
			double width;
			double height; 
		};
		Vec2 dimension;
	};

	/* 
		Padding of this component 
	*/
	double padding = GUI::padding;

	/* 
		Margin of this component
	*/
	double margin = GUI::margin;

	/* 
		Determines if this component and its content should be rendered 
	*/
	bool visible = true;

	/*
		Debug
	*/
	bool debug = false;

	/* 
		Constructors
	*/
	Component(Vec2 position, Vec2 dimension) : position(position), dimension(dimension), resizing(false) {};
	Component(Vec2 position) : position(position), dimension(Vec2()), resizing(true) {};
	Component(double x, double y, double width, double height) : Component(Vec2(x, y), Vec2(width, height)) {};
	Component(double x, double y) : Component(Vec2(x, y)) {};
	
	
	/* 
		Returns this if the component contains the point 
	*/
	virtual Component* intersect(Vec2 point) {
		Vec2 halfDimension = dimension / 2;
		Vec2 center = position + Vec2(halfDimension.x, -halfDimension.y);
		if (fabs(point.x - center.x) < halfDimension.x && fabs(point.y - center.y) < halfDimension.y)
			return this;
		return nullptr;
	}

	/* 
		Returns the minimal size of the container, margin not included, padding included
	*/
	virtual Vec2 resize() = 0;
	
	/* 
		Renders the component, resizing may happen
	*/
	virtual void render() = 0;

	/*
		Disables the component and all its children
	*/
	virtual void disable() {
		disabled = true;
	};

	/*
		Enables the component and all its children
	*/
	virtual void enable() {
		disabled = false;
	};

	/* 
		Drag behaviour of this component 
	*/
	virtual void drag(Vec2 mxyNew, Vec2 mxyOld) {};

	/* 
		Hover behaviour of this component 
	*/
	virtual void hover(Vec2 mxy) {};

	/* 
		Press behaviour of this component 
	*/
	virtual void press(Vec2 mxy) {};

	/*
		Release behaviour of this component
	*/
	virtual void release(Vec2 mxy) {}

	/* 
		Enter behaviour of this component 
	*/
	virtual void enter() {};

	/* 
		Exit behaviour of this component 
	*/
	virtual void exit() {};
};