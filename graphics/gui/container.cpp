#include "core.h"

#include "container.h"

#include "layout.h"
#include "path/path.h"
#include "mesh/primitive.h"

namespace Graphics {

Container::Container(Vec2 position) : Component(position), layout(new FlowLayout()) {
};

Container::Container(Vec2 position, Vec2 dimension) : Component(position, dimension), layout(new FlowLayout()) {

};

Container::Container(double x, double y, double width, double height) : Container(Vec2(x, y), Vec2(width, height)) {
};

Container::Container(double x, double y) : Container(Vec2(x, y)) {

};

void Container::add(Component* child) {
	add(child, Align::RELATIVE);
}

void Container::add(Component* child, Align alignment) {
	child->parent = this;
	children.add(std::make_pair(child, alignment));
}

std::pair<Component*, Align> Container::get(Component* child) {
	for (auto iterator = children.begin(); iterator != children.end(); ++iterator) {
		if (child == iterator->first)
			return *iterator;
	}
}

void Container::remove(Component* child) {
	for (auto iterator = children.begin(); iterator != children.end(); ++iterator) {
		if (child == iterator->first) {
			children.remove(iterator);
			return;
		}
	}
}

Component* Container::intersect(Vec2 point) {
	for (auto iterator = children.begin(); iterator != children.end(); ++iterator) {
		if (iterator->first->intersect(point))
			return iterator->first;
	}
	return this->Component::intersect(point);
}

void Container::renderChildren() {
	for (auto child : children) {
		child.first->render();
		if (debug) {
			Path::rect(child.first->position + Vec2(-margin, margin), child.first->dimension + Vec2(margin * 2), 0, COLOR::R);
			Path::rect(child.first->position, child.first->dimension, 0, COLOR::G);
			Path::rect(child.first->position + Vec2(padding, -padding), child.first->dimension + Vec2(-padding * 2), 0, COLOR::B);
		}
	}
}

};