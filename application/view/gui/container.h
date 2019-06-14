#pragma once

#include <vector>

#include "../engine/math/vec2.h"

#include "orderedVector.h"
#include "component.h"
#include "layout.h"

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
	Container(Vec2 position) : Component(position), layout(new FlowLayout()) {};
	Container(Vec2 position, Vec2 dimension) : Component(position, dimension), layout(new FlowLayout()) {};
	Container(double x, double y, double width, double height) : Container(Vec2(x, y), Vec2(width, height)) {};
	Container(double x, double y) : Container(Vec2(x, y)) {};

	/*
		Adds the child to the end of the container
	*/
	void add(Component* child) {
		add(child, Align::RELATIVE);
	}

	/*
		Adds the child to the end of the container with a given alignment
	*/
	void add(Component* child, Align alignment) {
		child->parent = this;
		children.add(std::make_pair(child, alignment));
	}

	/*
		Returns the child with its alignment 
	*/
	std::pair<Component*, Align> get(Component* child) {
		for (auto iterator = children.begin(); iterator != children.end(); ++iterator) {
			if (child == iterator->first)
				return *iterator;
		}
	}

	// Needs to be optimized
	void remove(Component* child) {
		for (auto iterator = children.begin(); iterator != children.end(); ++iterator) {
			if (child == iterator->first) {
				children.remove(iterator);
				return;
			}
		}
	}

	virtual Component* intersect(Vec2 point) {
		for (auto iterator = children.begin(); iterator != children.end(); ++iterator) {
			if (iterator->first->intersect(point))
				return iterator->first;
		}
		return this->Component::intersect(point);
	}

	virtual void renderChildren() {
		for (auto child : children) {
			child.first->render();
			if (debug) {
				GUI::quad->resize(child.first->position + Vec2(-margin, margin), child.first->dimension + Vec2(margin * 2));
				GUI::shader->update(GUI::COLOR::R);
				GUI::quad->render(GL_LINE);
				GUI::quad->resize(child.first->position, child.first->dimension);
				GUI::shader->update(GUI::COLOR::G);
				GUI::quad->render(GL_LINE);
				GUI::quad->resize(child.first->position + Vec2(padding, -padding), child.first->dimension + Vec2(-padding * 2));
				GUI::shader->update(GUI::COLOR::B);
				GUI::quad->render(GL_LINE);
			}
		}
	}

	virtual void render() = 0;
	virtual Vec2 resize() = 0;
};