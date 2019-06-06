#include "colorPicker.h"

#include "gui.h"

ColorPicker::ColorPicker(double x, double y, double size) : Component(x, y, size, size) {
	dimension = Vec2(padding + GUI::colorPickerBrightnessWidth + 2 * GUI::colorPickerBrightnessBorderWidth + GUI::colorPickerSpacing + size + GUI::colorPickerSpacing + GUI::colorPickerBrightnessWidth + 2 * GUI::colorPickerBrightnessBorderWidth + padding, padding + size + padding);
	background = GUI::COLOR::BACK;
	setColor(Vec4(1));
}

ColorPicker::ColorPicker(double x, double y) : Component(x, y) {
	dimension = Vec2(padding + GUI::colorPickerBrightnessWidth + 2 * GUI::colorPickerBrightnessBorderWidth + GUI::colorPickerSpacing + GUI::colorPickerHueSize + GUI::colorPickerSpacing + GUI::colorPickerBrightnessWidth + 2 * GUI::colorPickerBrightnessBorderWidth + padding, padding + GUI::colorPickerHueSize + padding);
	background = GUI::COLOR::BACK;
	setColor(Vec4(1));
};

void ColorPicker::render() {
	if (visible) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		GUI::shader->update(background);
		GUI::quad->resize(position, dimension);
		GUI::quad->render();

		Vec2 brightnessBorderPosition = position + Vec2(padding, -padding);
		Vec2 brightnessBorderDimension = Vec2(GUI::colorPickerBrightnessWidth + 2 * GUI::colorPickerBrightnessBorderWidth, height - 2 * padding);
		GUI::shader->update(GUI::colorPickerBrightnessBorderColor);
		GUI::quad->resize(brightnessBorderPosition, brightnessBorderDimension);
		GUI::quad->render();

		Vec2 brightnessPosition = brightnessBorderPosition + Vec2(GUI::colorPickerBrightnessBorderWidth, -GUI::colorPickerBrightnessBorderWidth);
		Vec2 brightnessDimension = Vec2(GUI::colorPickerBrightnessWidth, brightnessBorderDimension.y - 2 * GUI::colorPickerBrightnessBorderWidth);
		GUI::shader->update(GUI::colorPickerBrightnessTexture, GUI::COLOR::hsvaToRgba(Vec4(color.x, color.y, 1, 1)));
		GUI::quad->resize(brightnessPosition, brightnessDimension);
		GUI::quad->render();

		Vec2 brightnessSelectorPosition = brightnessPosition + Vec2((GUI::colorPickerBrightnessWidth - GUI::colorPickerBrightnessSelectorWidth) / 2, -(1 - brightness) * brightnessDimension.y + GUI::colorPickerBrightnessSelectorHeight / 2);
		Vec2 brightnessSeletorDimension = Vec2(GUI::colorPickerBrightnessSelectorWidth, GUI::colorPickerBrightnessSelectorHeight);
		GUI::shader->update(GUI::colorPickerBrightnessSelectorColor);
		GUI::quad->resize(brightnessSelectorPosition, brightnessSeletorDimension);
		GUI::quad->render();

		Vec2 huePosition = position + Vec2(padding + 2 * GUI::colorPickerBrightnessBorderWidth + GUI::colorPickerBrightnessWidth  + GUI::colorPickerSpacing, -padding);
		Vec2 hueDimension = Vec2(height - 2 * padding);
		GUI::shader->update(GUI::colorPickerHueTexture);
		GUI::quad->resize(huePosition, hueDimension);
		GUI::quad->render();

		Vec2 crosshairPosition = position + crosshair + Vec2(-GUI::colorPickerPointerCrosshairSize, GUI::colorPickerPointerCrosshairSize) / 2;
		Vec2 crosshairDimesion = Vec2(GUI::colorPickerPointerCrosshairSize);
		GUI::shader->update(GUI::colorPickerCrosshairTexture);
		GUI::quad->resize(crosshairPosition, crosshairDimesion);
		GUI::quad->render();

		Vec2 colorBorderPosition = Vec2(huePosition.x + hueDimension.x + GUI::colorPickerSpacing, brightnessBorderPosition.y);
		Vec2 colorBorderDimension = brightnessBorderDimension;
		GUI::shader->update(GUI::colorPickerBrightnessBorderColor);
		GUI::quad->resize(colorBorderPosition, colorBorderDimension);
		GUI::quad->render();

		Vec2 colorPosition = colorBorderPosition + Vec2(GUI::colorPickerBrightnessBorderWidth, -GUI::colorPickerBrightnessBorderWidth);
		Vec2 colorDimension = brightnessDimension;
		GUI::shader->update(GUI::COLOR::hsvaToRgba(color));
		GUI::quad->resize(colorPosition, colorDimension);
		GUI::quad->render();
	}
}

Vec2 ColorPicker::resize() {
	return dimension;
}

void ColorPicker::setColor(Vec4 rgba) {
	Vec4 hsva = GUI::COLOR::rgbaToHsva(rgba);
	double radius = height / 2 - padding;

	Vec2 relativeCenter = Vec2(padding + GUI::colorPickerBrightnessWidth + GUI::colorPickerSpacing + radius, -height / 2);
	double angle = (hsva.x * 2 + 0.25) * 3.14159265359;
	crosshair = relativeCenter + Vec2(cos(angle), sin(angle)) * (1 - hsva.y);

	color = hsva;
}

void ColorPicker::press(Vec2 point) {

	// BrightnessPicker
	Vec2 brightnessPosition = position + Vec2(GUI::colorPickerBrightnessBorderWidth + padding, -GUI::colorPickerBrightnessBorderWidth - padding);
	Vec2 brightnessDimension = Vec2(GUI::colorPickerBrightnessWidth, height - 2 * padding - 2 * GUI::colorPickerBrightnessBorderWidth);

	if (!colorPicking && !brightnessPicking) {
		brightnessPicking = GUI::intersectsSquare(point, brightnessPosition, brightnessDimension);
	} 

	if (brightnessPicking) {
		double leftBottomY = brightnessPosition.y - brightnessDimension.y;
		double relativeY = point.y - leftBottomY;
		brightness = GUI::clamp(relativeY / brightnessDimension.y, 0, 1);
		color.z = brightness;
	}

	// ColorPicker
	double radius = height / 2 - padding;
	Vec2 center = position + Vec2(padding + GUI::colorPickerBrightnessWidth + GUI::colorPickerSpacing + radius, -height / 2);
	Vec2 pointer = (point - center);
	double length = pointer.length();

	if (!colorPicking && ! brightnessPicking) {
		colorPicking = length <= radius;
	} 

	if (colorPicking) {
		if (length > radius) {
			pointer = pointer / length * radius;
			point = center + pointer;
			length = radius;
		}

		crosshair = point - position;

		double hue = fmod(atan2(-pointer.y, pointer.x) / (2 * 3.14159265359) + 1.25, 1);
		double saturation = length / radius;
		color = Vec4(hue, saturation, color.z, color.w);
	}


	// ColorPicker
	/*double radius = height / 2 - padding;

	Vec2 center = position + Vec2(padding + GUI::colorPickerBrightnessWidth + GUI::colorPickerSpacing + radius, -height / 2);
	Vec2 pointer = (point - center);

	double length = pointer.length();

	if (length > radius) {
		if (colorPicking) {
			pointer = pointer / length * radius;
			point = center + pointer;
			length = radius;
		} 	
	}

	crosshair = point - position;

	double h = atan2(pointer.y, pointer.x) / (2 * 3.14159265359) + 0.5;
	double s = length / radius;
	color = GUI::COLOR::hsvaToRgba(Vec4(h, s, 1, 1));*/
}

void ColorPicker::drag(Vec2 point) {
	press(point);
}

void ColorPicker::release(Vec2 point) {
	colorPicking = false;
	brightnessPicking = false;
}