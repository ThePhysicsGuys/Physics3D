#include "core.h"

#include "node.h"
#include "tree.h"

namespace P3D::Engine {

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
		if (getParent() != nullptr) 
			getParent()->removeChild(this);
	} else {
		parent->addChild(this);
	}
}

std::string Node::getName() {
	return name;
}

void Node::setName(const std::string& name) {
	// todo update tree
	this->name = name;
}

ECSTree* Node::getTree() {
	return tree;
}

void Node::setTree(ECSTree* tree) {
	// todo remove node from other tree
	this->tree = tree;
}

void Node::addChild(Node* child) {
	if (getParent() == child)
		return;

	if (containsDeepChild(child))
		return;
	// todo check for previous parent
	child->parent = this;
	children.push_back(child);
}

void Node::removeChild(Node* child) {
	for (auto iterator = children.begin(); iterator != children.end(); ++iterator) {
		if (*iterator == child) {
			// todo update internal parent
			// todo update constraints
			children.erase(iterator);
			child->parent = nullptr;
			return;
		}
	}
}

bool Node::containsDeepChild(Node* query) {
	for (Node* child : children) {
		if (child == query)
			return true;

		if (child->containsDeepChild(query))
			return true;
	}

	return false;
}

bool Node::containsChild(Node* query) {
	for (Node* child : children) {
		if (child == query)
			return true;
	}

	return false;
}

};