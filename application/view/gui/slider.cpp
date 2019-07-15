#include "slider.h"

#include "../shaderProgram.h"
#include "../renderUtils.h"

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

		Vec4 blendColor = (disabled) ? GUI::COLOR::DISABLED : GUI::COLOR::WHITE;

		resize();

		GUI::quad->resize(position, dimension);
		Shaders::quadShader.updateColor(GUI::COLOR::blend(backgroundColor, blendColor));
		GUI::quad->render();
		
		double progress = (value - min) / (max - min);
		Vec2 sliderFilledPosition = position + Vec2(padding + handleWidth / 2, -height / 2 + barHeight / 2);
		Vec2 sliderFilledDimension = Vec2(barWidth * progress, barHeight);
		GUI::quad->resize(sliderFilledPosition, sliderFilledDimension);
		Shaders::quadShader.updateColor(GUI::COLOR::blend(foregroundFilledColor, blendColor));
		GUI::quad->render();

		Vec2 sliderEmptyPosition = sliderFilledPosition + Vec2(sliderFilledDimension.x, 0);
		Vec2 sliderEmptyDimension = Vec2(barWidth * (1.0 - progress), barHeight);
		GUI::quad->resize(sliderEmptyPosition, sliderEmptyDimension);
		Shaders::quadShader.updateColor(GUI::COLOR::blend(foregroundEmptyColor, blendColor));
		GUI::quad->render();

		Vec2 handlePosition = Vec2(sliderEmptyPosition.x - handleWidth / 2, position.y - height / 2 + handleHeight / 2);
		Vec2 handleDimension = Vec2(handleWidth, handleHeight);
		GUI::quad->resize(handlePosition, handleDimension);
		Shaders::quadShader.updateColor(GUI::COLOR::blend(handleColor, blendColor));
		GUI::quad->render();
		
		Shaders::quadShader.updateColor(GUI::COLOR::blend(GUI::COLOR::ACCENT, blendColor));
		GUI::quad->render(Renderer::WIREFRAME);

		if (debug) {
			GUI::quad->resize(position, dimension);
			Shaders::quadShader.updateColor(GUI::COLOR::RED);
			GUI::quad->render(Renderer::WIREFRAME);
		}
	}
}

Vec2 Slider::resize() {
	if (resizing) {
		dimension = Vec2(GUI::sliderBarWidth + GUI::sliderHandleWidth, GUI::sliderHandleHeight) + Vec2(padding) * 2;
	}
	return dimension;
}

void Slider::drag(Vec2 newPoint, Vec2 oldPoint) {
	if (disabled)
		return;

	press(newPoint);
}

void Slider::press(Vec2 point) {
	if (disabled)
		return;

	double x = position.x + padding + handleWidth / 2;
	double w = dimension.x - 2 * padding - handleWidth;

	if (point.x < x || point.x > x + w)
		return;

	value = min + (max - min) * (point.x - x) / w;
	(*action)(this);
}
