#include "slider.h"

Slider::Slider(double x, double y) : Slider(x, y, 0, 1, 0) {}

Slider::Slider(double x, double y, double width, double height) : Slider(x, y, width, height, 0, 1, 0) {}

Slider::Slider(double x, double y, double min, double max, double value) : Component(x, y) {
	this->min = min;
	this->max = max;
	this->value = value;

	handleColor = GUI::defaultSliderHandleColor;
	backgroundColor = GUI::defaultSliderBackgroundColor;
	foregroundFilledColor = GUI::defaultSliderForegroundFilledColor;
	foregroundEmptyColor = GUI::defaultSliderForegroundEmptyColor;

	padding = GUI::defaultPadding;

	handleWidth = GUI::defaultSliderHandleWidth;
	handleHeight = GUI::defaultSliderHandleHeight;
	sliderWidth = GUI::defaultSliderWidth;
	sliderHeight = GUI::defaultSliderHeight;
}


Slider::Slider(double x, double y, double width, double height, double min, double max, double value) : Component(x, y, width, height) {
	this->min = min;
	this->max = max;
	this->value = value;

	handleColor = GUI::defaultSliderHandleColor;
	backgroundColor = GUI::defaultSliderBackgroundColor;
	foregroundFilledColor = GUI::defaultSliderForegroundFilledColor;
	foregroundEmptyColor = GUI::defaultSliderForegroundEmptyColor;

	handleWidth = GUI::defaultSliderHandleWidth;
	handleHeight = height - 2 * padding;
	sliderWidth = width - 2 * padding;
	sliderHeight = GUI::defaultSliderHeight;
}

void Slider::render() {
	if (visible) {
		GUI::defaultQuad->resize(position, dimension);
		GUI::defaultShader->update(backgroundColor);
		GUI::defaultQuad->render();
		
		double progress = value / (max - min);
		Vec2 sliderFilledPosition = position + Vec2(padding + handleWidth / 2, -height / 2 + sliderHeight / 2);
		Vec2 sliderFilledDimension = Vec2((width - 2 * padding - handleWidth) * progress, sliderHeight);
		GUI::defaultQuad->resize(sliderFilledPosition, sliderFilledDimension);
		GUI::defaultShader->update(foregroundFilledColor);
		GUI::defaultQuad->render();

		Vec2 sliderEmptyPosition = sliderFilledPosition + Vec2(sliderFilledDimension.x, 0);
		Vec2 sliderEmptyDimension = Vec2((width - 2 * padding - handleWidth) * (1.0 - progress), sliderHeight);
		GUI::defaultQuad->resize(sliderEmptyPosition, sliderEmptyDimension);
		GUI::defaultShader->update(foregroundEmptyColor);
		GUI::defaultQuad->render();

		Vec2 handlePosition = Vec2(sliderFilledPosition.x + sliderFilledDimension.x - handleWidth / 2, position.y - height / 2 + handleHeight / 2);
		Vec2 handleDimension = Vec2(handleWidth, handleHeight);
		GUI::defaultQuad->resize(handlePosition, handleDimension);
		GUI::defaultShader->update(handleColor);
		GUI::defaultQuad->render();
		GUI::defaultShader->update(GUI::COLOR::ACCENT);
		GUI::defaultQuad->render(GL_LINE);

		GUI::defaultQuad->resize(position, dimension);
		GUI::defaultShader->update(GUI::COLOR::RED);
		GUI::defaultQuad->render(GL_LINE);
	}
}

Vec2 Slider::resize() {
	if (resizing)
		dimension = Vec2(GUI::defaultSliderWidth + 2 * padding, GUI::defaultSliderHandleHeight + 2 * padding);
	return dimension;
}

void Slider::drag(Vec2 point) {

	Log::debug("%f, %f, %f", min, max, value);

	if (point.x < position.x)
		value = min;
	else if (point.x > (position.x + dimension.x))
		value = max;
	else
		value = min + (max - min) * (point.x - position.x) / dimension.x;

	(*action)(this);
}

void Slider::press(Vec2 point) {
	value = min + (max - min) * (point.x - position.x) / dimension.x;
	(*action)(this);
}
