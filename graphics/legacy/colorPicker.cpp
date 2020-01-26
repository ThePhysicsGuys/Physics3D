#include "core.h"

#include "colorPicker.h"

#include "guiUtils.h"
#include "texture.h"
#include "path/path.h"
#include "mesh/primitive.h"

ColorPicker::ColorPicker(double x, double y, double size) : Component(x, y, size, size) {
	this->padding = GUI::padding;
	this->margin = GUI::margin;
	this->background = COLOR::BACK;

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
	this->background = COLOR::BACK;
	this->padding = GUI::padding;
	this->margin = GUI::margin;

	dimension = Vec2(
		padding + 
		GUI::colorPickerBarWidth + 2 * GUI::colorPickerBarBorderWidth +
		GUI::colorPickerSpacing +
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

		Vec4 blendColor = (disabled) ? COLOR::DISABLED : COLOR::WHITE;

		// Brightness
		Vec2f brightnessBorderPosition = position + Vec2(padding, -padding);
		Vec2f brightnessBorderDimension = Vec2(GUI::colorPickerBarWidth + 2 * GUI::colorPickerBarBorderWidth, height - 2 * padding);
		Path::rectFilled(brightnessBorderPosition, brightnessBorderDimension, 0.0f, COLOR::blend(GUI::colorPickerBarBorderColor, blendColor));

		Vec2f brightnessPosition = brightnessBorderPosition + Vec2(GUI::colorPickerBarBorderWidth, -GUI::colorPickerBarBorderWidth);
		Vec2f brightnessDimension = Vec2(GUI::colorPickerBarWidth, brightnessBorderDimension.y - 2 * GUI::colorPickerBarBorderWidth);
		Path::rectUV(GUI::colorPickerBrightnessTexture->getID(), brightnessPosition, brightnessDimension, Vec2f(0), Vec2f(1), COLOR::blend(COLOR::hsvaToRgba(Vec4(hsva.x, hsva.y, 1, 1)), blendColor));

		Vec2f brightnessSelectorPosition = brightnessPosition + Vec2((GUI::colorPickerBarWidth - GUI::colorPickerSelectorWidth) / 2, -(1 - hsva.z) * brightnessDimension.y + GUI::colorPickerSelectorHeight / 2);
		Vec2f brightnessSelectorDimension = Vec2(GUI::colorPickerSelectorWidth, GUI::colorPickerSelectorHeight);
		Path::rectFilled(brightnessSelectorPosition, brightnessSelectorDimension, 0.0f, COLOR::blend(GUI::colorPickerSelectorColor, blendColor));

		// Alpha
		Vec2f alphaBorderPosition = brightnessBorderPosition + Vec2(brightnessBorderDimension.x + GUI::colorPickerSpacing, 0);
		Vec2f alphaBorderDimension = brightnessBorderDimension;
		Path::rectFilled(alphaBorderPosition, alphaBorderDimension, 0.0f, COLOR::blend(GUI::colorPickerBarBorderColor, blendColor));;

		Vec2f alphaPosition = alphaBorderPosition + Vec2(GUI::colorPickerBarBorderWidth, -GUI::colorPickerBarBorderWidth);
		Vec2f alphaDimension = brightnessDimension;
		Path::rectUV(GUI::colorPickerAlphaPatternTexture->getID(), alphaPosition, alphaDimension);
		Path::rectUV(GUI::colorPickerAlphaBrightnessTexture->getID(), alphaPosition, alphaDimension, Vec2f(0), Vec2f(1), COLOR::blend(COLOR::hsvaToRgba(Vec4(hsva.x, hsva.y, hsva.z, 1)), blendColor));

		Vec2f alphaSelectorPosition = alphaPosition + Vec2((GUI::colorPickerBarWidth - GUI::colorPickerSelectorWidth) / 2, -(1 - hsva.w) * brightnessDimension.y + GUI::colorPickerSelectorHeight / 2);
		Vec2f alphaSelectorDimension = brightnessSelectorDimension;
		Path::rectFilled(alphaSelectorPosition, alphaSelectorDimension, 0.0f, COLOR::blend(GUI::colorPickerSelectorColor, blendColor));

		// Hue
		Vec2f huePosition = Vec2(alphaBorderPosition.x + alphaBorderDimension.x + GUI::colorPickerSpacing, alphaBorderPosition.y);
		Vec2f hueDimension = Vec2(alphaBorderDimension.y);
		Path::rectUV(GUI::colorPickerHueTexture->getID(), huePosition, hueDimension, Vec2f(0), Vec2f(1), blendColor);

		Vec2f crosshairPosition = position + crosshair + Vec2(-GUI::colorPickerCrosshairSize, GUI::colorPickerCrosshairSize) / 2;
		Vec2f crosshairDimension = Vec2(GUI::colorPickerCrosshairSize);
		Path::rectUV(GUI::colorPickerCrosshairTexture->getID(), crosshairPosition, crosshairDimension, Vec2f(0), Vec2f(1), blendColor);

		// Color
		Vec2f colorBorderPosition = Vec2(huePosition.x + hueDimension.x + GUI::colorPickerSpacing, brightnessBorderPosition.y);
		Vec2f colorBorderDimension = brightnessBorderDimension;
		Path::rectFilled(colorBorderPosition, colorBorderDimension, 0.0f, COLOR::blend(GUI::colorPickerBarBorderColor, blendColor));

		Vec2f colorPosition = colorBorderPosition + Vec2(GUI::colorPickerBarBorderWidth, -GUI::colorPickerBarBorderWidth);
		Vec2f colorDimension = brightnessDimension;
		Path::rectUV(GUI::colorPickerAlphaPatternTexture->getID(), colorPosition, colorDimension);
		Path::rectFilled(colorPosition, colorDimension, 0.0f, COLOR::blend(COLOR::hsvaToRgba(hsva), blendColor));
	}
}

Vec2 ColorPicker::resize() {
	return dimension;
}

void ColorPicker::setRgba(Color rgba) {
	hsva = COLOR::rgbaToHsva(rgba);
	double radius = height / 2 - padding;

	Vec2 relativeCenter = Vec2(padding + 2 * GUI::colorPickerBarWidth + 2 * GUI::colorPickerSpacing + 2 * 2 * GUI::colorPickerBarBorderWidth + radius, -height / 2);
	double angle = -(hsva.x - 0.25) * 2 * 3.14159265359;
	crosshair = relativeCenter + Vec2(cos(angle), sin(angle)) * hsva.y * radius;
}

Color ColorPicker::getRgba() {
	return COLOR::hsvaToRgba(hsva);
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
	double l = length(pointer);

	if (!colorPicking && !brightnessPicking && !alphaPicking) {
		colorPicking = l <= radius;
	} 

	if (colorPicking) {
		if (l > radius) {
			pointer = pointer / l * radius;
			point = center + pointer;
			l = radius;
		}

		crosshair = point - position;

		double hue = fmod(atan2(-pointer.y, pointer.x) / (2 * 3.14159265359) + 1.25, 1);
		double saturation = l / radius;
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