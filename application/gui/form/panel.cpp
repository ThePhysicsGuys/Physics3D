#include "panel.h"

#include "gui.h"

#include "../util/log.h"
#include "../engine/math/mathUtil.h"

Panel::Panel(double x, double y) : Component(Vec2(x, y)) {
	this->backgroundColor = GUI::defaultPanelBackgroundColor;
	this->titleBarHeight = GUI::defaultPanelTitleBarHeight;
	this->titleBarColor = GUI::defaultPanelTitleBarColor;
	this->offset = GUI::defaultPanelOffset;
};

Panel::Panel(double x, double y, double width, double height) : Component(Vec2(x, y), Vec2(width, height)) {
	this->backgroundColor = GUI::defaultPanelBackgroundColor;
	this->titleBarHeight = GUI::defaultPanelTitleBarHeight;
	this->titleBarColor = GUI::defaultPanelTitleBarColor;
	this->offset = GUI::defaultPanelOffset;
};

void Panel::add(Component* component) {
	component->parent = this;
	children.push_back(component);
}

void Panel::remove(Component* component) {
	component->parent = nullptr;
	for (std::vector<Component*>::iterator iterator = children.begin(); iterator != children.end(); iterator++) 
		if (component == *iterator) {
			children.erase(iterator);
			return;
		}
	}

void Panel::renderChildren() {
	for (Component* component : children)
		component->render();
}

Vec2 Panel::resize() {
	if (resizing) {
		double minX = 0;
		double minY = 0;
		double maxX = 0;
		double maxY = 0;

		for (Component* component : children) {
			component->resize();
			minX = fmin(minX, component->position.x);
			minY = fmin(minY, component->position.y);
			maxX = fmax(maxX, component->position.x + component->dimension.x);
			maxY = fmax(maxY, component->position.y + component->dimension.y);
		}
		dimension = Vec2(maxX - minX, maxY - minY);
	}
	return dimension;
}

void Panel::render() {
	Vec2 contentSize = resize();
	Vec2 contentPosition = position;
	Vec2 panelSize = contentSize + 2 * offset;
	Vec2 panelPosition = Vec2(contentPosition.x - offset, contentPosition.y + offset);
	Vec2 titleBarPosition = Vec2(contentPosition.x - offset, contentPosition.y + titleBarHeight + offset);
	Vec2 titleBarSize = Vec2(panelSize.x, titleBarHeight);

	// TitleBar
	GUI::defaultShader->update(titleBarColor);
	GUI::defaultQuad->resize(titleBarPosition, titleBarSize);
	GUI::defaultQuad->render();

	// Background
	GUI::defaultShader->update(backgroundColor - 0.1);
	GUI::defaultQuad->resize(panelPosition, panelSize);
	GUI::defaultQuad->render();

	// Contentbackground
	GUI::defaultShader->update(backgroundColor);
	GUI::defaultQuad->resize(contentPosition, contentSize);
	GUI::defaultQuad->render();

	renderChildren();
}
