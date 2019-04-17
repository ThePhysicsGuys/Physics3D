#pragma once

#include <vector>

#include "../engine/math/vec2.h"
#include "component.h"

class Container : public Component {
public:
	std::vector<std::pair<Component*, Align>> children;

	Container(Vec2 position) : Component(position) {};
	Container(Vec2 position, Vec2 dimension) : Component(position, dimension) {};

	void add(Component* child) {
		add(child, Align::RELATIVE);
	}

	void add(Component* child, Align alignment) {
		child->parent = this;
		children.push_back(std::make_pair(child, alignment));
	}

	void remove(Component* child) {
		for (auto iterator = children.begin(); iterator != children.end(); iterator++) {
			if (child == iterator->first) {
				children.erase(iterator);
				return;
			}
		}
	}

	virtual Component* intersect(Vec2 point) {
		for (auto iterator = children.begin(); iterator != children.end(); iterator++) {
			if (iterator->first->intersect(point))
				return iterator->first;
		}
		return this->Component::intersect(point);
	}

	virtual void renderChildren() {
		for (auto child : children)
			child.first->render();
	}

	virtual void render() = 0;
	virtual Vec2 resize() = 0;
};