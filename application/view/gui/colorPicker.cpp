#include "colorPicker.h"

#include "../shaderProgram.h"
#include "../texture.h"

#include "../mesh/primitive.h"

ColorPicker::ColorPicker(double x, double y, double size) : Component(x, y, size, size) {
	this->padding = GUI::padding;
	this->margin = GUI::margin;
	this->background = GUI::COLOR::BACK;

	dimension = Vec2(
		padding + 
		GUI::colorPickerBarWidth + 2 * GUI::colorPickerBarBorderWidth + 
		GUI::colorPickerSpacing + 
		GUI::colorPickerBarWidth + 2 * GUI::colorPickerBarBorderWidth +
		GUI::colorPickerSpacing + size + GUI::colorPickerSpacing + 
		GUI::colorPickerBarWidth + 2 * GUI::colorPickerBarBorderWidth + 
		padding, 
		
		padding + size + padding
	);

	setRgba(Vec4(1));
}

ColorPicker::ColorPicker(double x, double y) : Component(x, y) {
	this->background = GUI::COLOR::BACK;
	this->padding = GUI::padding;
	this->margin = GUI::margin;

	dimension = Vec2(
		padding + 
		GUI::colorPickerBarWidth + 2 * GUI::colorPickerBarBorderWidth + 
		GUI::colorPickerSpacing + GUI::colorPickerHueSize + GUI::colorPickerSpacing + 
		GUI::colorPickerBarWidth + 2 * GUI::colorPickerBarBorderWidth + 
		padding,
		
		padding + GUI::colorPickerHueSize + padding
	);

	setRgba(Vec4(1));
};

void ColorPicker::render() {
	if (visible) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		Vec4 blendColor = (disabled) ? GUI::COLOR::DISABLED : GUI::COLOR::WHITE;

		/*GUI::shader->updateColor(GUI::COLOR::blend(background, blendColor));
		GUI::quad->resize(position, dimension);
		GUI::quad->render();*/

		// Brightness
		Vec2 brightnessBorderPosition = position + Vec2(padding, -padding);
		Vec2 brightnessBorderDimension = Vec2(GUI::colorPickerBarWidth + 2 * GUI::colorPickerBarBorderWidth, height - 2 * padding);
		Shaders::quadShader.updateColor(GUI::COLOR::blend(GUI::colorPickerBarBorderColor, blendColor));
		GUI::quad->resize(brightnessBorderPosition, brightnessBorderDimension);
		GUI::quad->render();

		Vec2 brightnessPosition = brightnessBorderPosition + Vec2(GUI::colorPickerBarBorderWidth, -GUI::colorPickerBarBorderWidth);
		Vec2 brightnessDimension = Vec2(GUI::colorPickerBarWidth, brightnessBorderDimension.y - 2 * GUI::colorPickerBarBorderWidth);
		Shaders::quadShader.updateTexture(GUI::colorPickerBrightnessTexture, GUI::COLOR::blend(GUI::COLOR::hsvaToRgba(Vec4(hsva.x, hsva.y, 1, 1)), blendColor));
		GUI::quad->resize(brightnessPosition, brightnessDimension);
		GUI::quad->render();

		Vec2 brightnessSelectorPosition = brightnessPosition + Vec2((GUI::colorPickerBarWidth - GUI::colorPickerSelectorWidth) / 2, -(1 - hsva.z) * brightnessDimension.y + GUI::colorPickerSelectorHeight / 2);
		Vec2 brightnessSelectorDimension = Vec2(GUI::colorPickerSelectorWidth, GUI::colorPickerSelectorHeight);
		Shaders::quadShader.updateColor(GUI::COLOR::blend(GUI::colorPickerSelectorColor, blendColor));
		GUI::quad->resize(brightnessSelectorPosition, brightnessSelectorDimension);
		GUI::quad->render();

		// Alpha
		Vec2 alphaBorderPosition = brightnessBorderPosition + Vec2(brightnessBorderDimension.x + GUI::colorPickerSpacing, 0);
		Vec2 alphaBorderDimension = brightnessBorderDimension;
		Shaders::quadShader.updateColor(GUI::COLOR::blend(GUI::colorPickerBarBorderColor, blendColor));
		GUI::quad->resize(alphaBorderPosition, alphaBorderDimension);
		GUI::quad->render();

		Vec2 alphaPosition = alphaBorderPosition + Vec2(GUI::colorPickerBarBorderWidth, -GUI::colorPickerBarBorderWidth);
		Vec2 alphaDimension = brightnessDimension;
		GUI::quad->resize(alphaPosition, alphaDimension);
		Shaders::quadShader.updateTexture(GUI::colorPickerAlphaPatternTexture);
		GUI::quad->render();
		Shaders::quadShader.updateTexture(GUI::colorPickerAlphaBrightnessTexture, GUI::COLOR::blend(GUI::COLOR::hsvaToRgba(Vec4(hsva.x, hsva.y, hsva.z, 1)), blendColor));
		GUI::quad->render();

		Vec2 alphaSelectorPosition = alphaPosition + Vec2((GUI::colorPickerBarWidth - GUI::colorPickerSelectorWidth) / 2, -(1 - hsva.w) * brightnessDimension.y + GUI::colorPickerSelectorHeight / 2);
		Vec2 alphaSelectorDimension = brightnessSelectorDimension;
		Shaders::quadShader.updateColor(GUI::COLOR::blend(GUI::colorPickerSelectorColor, blendColor));
		GUI::quad->resize(alphaSelectorPosition, alphaSelectorDimension);
		GUI::quad->render();

		// Hue
		Vec2 huePosition = Vec2(alphaBorderPosition.x + alphaBorderDimension.x + GUI::colorPickerSpacing, alphaBorderPosition.y);
		Vec2 hueDimension = Vec2(alphaBorderDimension.y);
		Shaders::quadShader.updateTexture(GUI::colorPickerHueTexture, blendColor);
		GUI::quad->resize(huePosition, hueDimension);
		GUI::quad->render();

		Vec2 crosshairPosition = position + crosshair + Vec2(-GUI::colorPickerCrosshairSize, GUI::colorPickerCrosshairSize) / 2;
		Vec2 crosshairDimesion = Vec2(GUI::colorPickerCrosshairSize);
		Shaders::quadShader.updateTexture(GUI::colorPickerCrosshairTexture, blendColor);
		GUI::quad->resize(crosshairPosition, crosshairDimesion);
		GUI::quad->render();

		// Color
		Vec2 colorBorderPosition = Vec2(huePosition.x + hueDimension.x + GUI::colorPickerSpacing, brightnessBorderPosition.y);
		Vec2 colorBorderDimension = brightnessBorderDimension;
		Shaders::quadShader.updateColor(GUI::COLOR::blend(GUI::colorPickerBarBorderColor, blendColor));
		GUI::quad->resize(colorBorderPosition, colorBorderDimension);
		GUI::quad->render();

		Vec2 colorPosition = colorBorderPosition + Vec2(GUI::colorPickerBarBorderWidth, -GUI::colorPickerBarBorderWidth);
		Vec2 colorDimension = brightnessDimension;
		GUI::quad->resize(colorPosition, colorDimension);
		Shaders::quadShader.updateTexture(GUI::colorPickerAlphaPatternTexture);
		GUI::quad->render();
		Shaders::quadShader.updateColor(GUI::COLOR::blend(GUI::COLOR::hsvaToRgba(hsva), blendColor));
		GUI::quad->render();
	}
}

Vec2 ColorPicker::resize() {
	return dimension;
}

void ColorPicker::setRgba(Vec4 rgba) {
	hsva = GUI::COLOR::rgbaToHsva(rgba);
	double radius = height / 2 - padding;

	Vec2 relativeCenter = Vec2(padding + 2 * GUI::colorPickerBarWidth + 2 * GUI::colorPickerSpacing + 2 * 2 * GUI::colorPickerBarBorderWidth + radius, -height / 2);
	double angle = -(hsva.x - 0.25) * 2 * 3.14159265359;
	crosshair = relativeCenter + Vec2(cos(angle), sin(angle)) * hsva.y * radius;
}

Vec4 ColorPicker::getRgba() {
	return GUI::COLOR::hsvaToRgba(hsva);
}

void ColorPicker::press(Vec2 point) {
	if (disabled)
		return;

	// BrightnessPicker
	Vec2 brightnessPosition = position + Vec2(padding + GUI::colorPickerBarBorderWidth, -GUI::colorPickerBarBorderWidth - padding);
	Vec2 brightnessDimension = Vec2(GUI::colorPickerBarWidth, height - 2 * padding - 2 * GUI::colorPickerBarBorderWidth);

	if (!colorPicking && !brightnessPicking && !alphaPicking) {
		brightnessPicking = GUI::intersectsSquare(point, brightnessPosition, brightnessDimension);
	} 

	if (brightnessPicking) {
		double leftBottomY = brightnessPosition.y - brightnessDimension.y;
		double relativeY = point.y - leftBottomY;
		hsva.z = GUI::clamp(relativeY / brightnessDimension.y, 0, 1);
		(*action)(this);
	}

	// AlphaPicker
	Vec2 alphaPosition = brightnessPosition + Vec2(brightnessDimension.x + GUI::colorPickerSpacing + GUI::colorPickerBarBorderWidth, 0);
	Vec2 alphaDimension = brightnessDimension;

	if (!colorPicking && !brightnessPicking && !alphaPicking) {
		alphaPicking = GUI::intersectsSquare(point, alphaPosition, alphaDimension);
	}

	if (alphaPicking) {
		double leftBottomY = alphaPosition.y - alphaDimension.y;
		double relativeY = point.y - leftBottomY;
		hsva.w = GUI::clamp(relativeY / alphaDimension.y, 0, 1);
		(*action)(this);
	}

	// ColorPicker
	double radius = height / 2 - padding;
	Vec2 center = position + Vec2(padding + 2 * GUI::colorPickerBarWidth + 2 * GUI::colorPickerSpacing + 2 * 2 * GUI::colorPickerBarBorderWidth + radius, -height / 2);
	Vec2 pointer = (point - center);
	double length = pointer.length();

	if (!colorPicking && !brightnessPicking && !alphaPicking) {
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
		hsva = Vec4(hue, saturation, hsva.z, hsva.w);
		(*action)(this);
	}
}

void ColorPicker::drag(Vec2 newPoint, Vec2 oldPoint) {
	if (disabled)
		return;

	press(newPoint);
}

void ColorPicker::release(Vec2 point) {
	if (disabled)
		return;

	colorPicking = false;
	brightnessPicking = false;
	alphaPicking = false;
}