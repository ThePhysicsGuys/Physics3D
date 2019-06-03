#include "colorPicker.h"

#include "gui.h"

ColorPicker::ColorPicker(double x, double y, double size) : Component(x, y, size, size) {
	dimension = Vec2(padding + GUI::colorPickerBrightnessWidth + GUI::colorPickerSpacing + size + padding, padding + size + padding);
	background = GUI::COLOR::BACK;
	setColor(Vec4(1));
}

ColorPicker::ColorPicker(double x, double y) : Component(x, y) {
	dimension = Vec2(padding + GUI::colorPickerBrightnessWidth + GUI::colorPickerSpacing + GUI::colorPickerHueSize + padding, padding + GUI::colorPickerHueSize + padding);
	background = GUI::COLOR::BACK;
	setColor(Vec4(1));
};

void ColorPicker::render() {
	if (visible) {
		GUI::shader->update(background);
		GUI::quad->resize(position, dimension);
		GUI::quad->render();

		Vec2 brightnessPosition = position + Vec2(padding, -padding);
		Vec2 brightnessDimension = Vec2(GUI::colorPickerBrightnessWidth, height - 2 * padding);
		GUI::shader->update(GUI::colorPickerBrightnessTexture, color);
		GUI::quad->resize(brightnessPosition, brightnessDimension);
		GUI::quad->render();

		Vec2 huePosition = position + Vec2(padding + GUI::colorPickerBrightnessWidth + GUI::colorPickerSpacing, -padding);
		Vec2 hueDimension = Vec2(height - 2 * padding);
		GUI::shader->update(GUI::colorPickerHueTexture);
		GUI::quad->resize(huePosition, hueDimension);
		GUI::quad->render();

		Vec2 crosshairPosition = position + crosshair + Vec2(-GUI::colorPickerPointerCrosshairSize, GUI::colorPickerPointerCrosshairSize) / 2;
		Vec2 crosshairDimesion = Vec2(GUI::colorPickerPointerCrosshairSize);
		GUI::shader->update(GUI::colorPickerCrosshairTexture);
		GUI::quad->resize(crosshairPosition, crosshairDimesion);
		GUI::quad->render();
	}
}

Vec2 ColorPicker::resize() {
	return dimension;
}

void ColorPicker::setColor(Vec4 rgba) {
	color = rgba;

	Vec4 hsva = GUI::COLOR::rgbaToHsva(rgba);
	double radius = height / 2 - padding;

	Vec2 relativeCenter = Vec2(padding + GUI::colorPickerBrightnessWidth + GUI::colorPickerSpacing + radius, -height / 2);
	double angle = hsva.x * 2 * 3.14159265359;
	crosshair = relativeCenter + Vec2(cos(angle), sin(angle)) * (1 - hsva.y);
}

void ColorPicker::press(Vec2 point) {
	double radius = height / 2 - padding;

	Vec2 center = position + Vec2(padding + GUI::colorPickerBrightnessWidth + GUI::colorPickerSpacing + radius, -height / 2);
	Vec2 pointer = (point - center);

	double length = pointer.length();

	if (length > radius) {
		pointer = pointer / length * radius;
		point = center + pointer;
		length = radius;
	}

	crosshair = point - position;

	double h = atan2(pointer.y, pointer.x) / (2 * 3.14159265359) + 0.5;
	double s = length / radius;
	color = GUI::COLOR::hsvaToRgba(Vec4(h, s, 1, 1));
}

void ColorPicker::drag(Vec2 point) {
	press(point);
}