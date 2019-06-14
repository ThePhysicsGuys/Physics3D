#include "slider.h"

Slider::Slider(double x, double y) : Slider(x, y, 0, 1, 0) {}

Slider::Slider(double x, double y, double width, double height) : Slider(x, y, width, height, 0, 1, 0) {}

Slider::Slider(double x, double y, double min, double max, double value) : Component(x, y) {
	this->min = min;
	this->max = max;
	this->value = value;

	this->handleColor = GUI::sliderHandleColor;
	this->backgroundColor = GUI::sliderBackgroundColor;
	this->foregroundFilledColor = GUI::sliderForegroundFilledColor;
	this->foregroundEmptyColor = GUI::sliderForegroundEmptyColor;


	this->handleWidth = GUI::sliderHandleWidth;
	this->handleHeight = GUI::sliderHandleHeight;
	this->barWidth = GUI::sliderBarWidth;
	this->barHeight = GUI::sliderBarHeight;
}


Slider::Slider(double x, double y, double width, double height, double min, double max, double value) : Component(x, y, width, height) {
	this->min = min;
	this->max = max;
	this->value = value;

	handleColor = GUI::sliderHandleColor;
	backgroundColor = GUI::sliderBackgroundColor;
	foregroundFilledColor = GUI::sliderForegroundFilledColor;
	foregroundEmptyColor = GUI::sliderForegroundEmptyColor;

	this->padding = GUI::padding;
	this->margin = GUI::margin;

	handleWidth = GUI::sliderHandleWidth;
	handleHeight = height - 2 * padding;
	barWidth = width - 2 * padding - handleWidth;
	barHeight = GUI::sliderBarHeight;
}

void Slider::render() {
	if (visible) {
		resize();

		GUI::quad->resize(position, dimension);
		GUI::shader->update(backgroundColor);
		GUI::quad->render();
		
		double progress = (value - min) / (max - min);
		Vec2 sliderFilledPosition = position + Vec2(padding + handleWidth / 2, -height / 2 + barHeight / 2);
		Vec2 sliderFilledDimension = Vec2(barWidth * progress, barHeight);
		GUI::quad->resize(sliderFilledPosition, sliderFilledDimension);
		GUI::shader->update(foregroundFilledColor);
		GUI::quad->render();

		Vec2 sliderEmptyPosition = sliderFilledPosition + Vec2(sliderFilledDimension.x, 0);
		Vec2 sliderEmptyDimension = Vec2(barWidth * (1.0 - progress), barHeight);
		GUI::quad->resize(sliderEmptyPosition, sliderEmptyDimension);
		GUI::shader->update(foregroundEmptyColor);
		GUI::quad->render();

		Vec2 handlePosition = Vec2(sliderEmptyPosition.x - handleWidth / 2, position.y - height / 2 + handleHeight / 2);
		Vec2 handleDimension = Vec2(handleWidth, handleHeight);
		GUI::quad->resize(handlePosition, handleDimension);
		GUI::shader->update(handleColor);
		GUI::quad->render();
		
		GUI::shader->update(GUI::COLOR::ACCENT);
		GUI::quad->render(GL_LINE);

		if (debug) {
			GUI::quad->resize(position, dimension);
			GUI::shader->update(GUI::COLOR::RED);
			GUI::quad->render(GL_LINE);
		}
	}
}

Vec2 Slider::resize() {
	if (resizing) {
		dimension = Vec2(GUI::sliderBarWidth + GUI::sliderHandleWidth, GUI::sliderHandleHeight) + Vec2(padding) * 2;
	}
	return dimension;
}

void Slider::drag(Vec2 point) {
	double x = position.x + padding + handleWidth / 2;
	double w = dimension.x - 2 * padding - handleWidth;

	if (point.x < x)
		value = min;
	else if (point.x > x + w)
		value = max;
	else
		value = min + (max - min) * (point.x - x) / w;
	
	(*action)(this);
}

void Slider::press(Vec2 point) {
	double x = position.x + padding + handleWidth / 2;
	double w = dimension.x - 2 * padding - handleWidth;

	if (point.x < x || point.x > x + w)
		return;

	value = min + (max - min) * (point.x - x) / w;
	(*action)(this);
}
