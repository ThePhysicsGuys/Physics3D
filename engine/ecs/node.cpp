#include "core.h"

#include "node.h"
#include "tree.h"

namespace Engine {

Node::Node(const std::string& name) {
	this->name = name;
}

std::vector<Node*> Node::getChildren() {
	return children;
}

Node* Node::getParent() {
	return parent;
}

void Node::setParent(Node* parent) {
	if (parent == nullptr) {
		if (getParent() != nullptr) {
			getParent()->removeChild(this);
			this->parent = nullptr;
		}
	} else {
		parent->addChild(this);
	}
}

std::string Node::getName() {
	return name;
}

void Node::setName(const std::string& name) {
	// todo update tree maybe?
	this->name = name;
}

void Node::addChild(Node* child) {
	if (getParent() == child)
		return;

	if (containsChild(child))
		return;

	children.push_back(child);
}

void Node::removeChild(Node* child) {
	for (auto iterator = children.begin(); iterator != children.end(); ++iterator) {
		if (*iterator == child) {
			// todo update internal parent
			// todo update constraints
			children.erase(iterator);
			child->setParent(nullptr);
			return;
		}
	}
}

bool Node::containsChild(Node* query) {
	for (Node* child : children) {
		if (child == query)
			return true;

		if (child->containsChild(query))
			return true;
	}

	return false;
}

};