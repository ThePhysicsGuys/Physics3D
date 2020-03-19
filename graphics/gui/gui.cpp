#include "core.h"

#include "gui.h"
#include "guiUtils.h"

#include "component.h"
#include "orderedVector.h"
#include "path/path.h"
#include "shader/shaders.h"

#include "font.h"
#include "texture.h"
#include "visualShape.h"
#include "renderer.h"

#include "../buffers/frameBuffer.h"
#include "../mesh/indexedMesh.h"
#include "../mesh/primitive.h"

#include "../util/resource/resourceManager.h"
#include "../resource/textureResource.h"
#include "../resource/fontResource.h"

namespace Graphics {

namespace GUI {

// Global
WindowInfo windowInfo;
OrderedVector<Component*> components;
Graphics::FrameBuffer* guiFrameBuffer = nullptr;
Component* intersectedComponent = nullptr;
Component* selectedComponent = nullptr;
Vec2 intersectedPoint;

// Defaults
double margin = 0.005;
double padding = 0.01;

// Quad
Quad* quad = nullptr;

// Label
Vec4f labelBackgroundColor = COLOR::WHITE;

// Button
Graphics::Texture* closeButtonHoverTexture;
Graphics::Texture* closeButtonIdleTexture;
Graphics::Texture* closeButtonPressTexture;
Graphics::Texture* minimizeButtonHoverTexture;
Graphics::Texture* minimizeButtonIdleTexture;
Graphics::Texture* minimizeButtonPressTexture;
Color borderColor = COLOR::SILVER;
double borderWidth = 0.004;

// Slider
double sliderBarWidth = 0.4;
double sliderHandleWidth = 0.02;
double sliderBarHeight = 0.004;
double sliderHandleHeight = 0.04;
Color sliderHandleColor = COLOR::BACK;
Color sliderBackgroundColor = COLOR::ALPHA;
Color sliderForegroundFilledColor = COLOR::ACCENT;
Color sliderForegroundEmptyColor = COLOR::GRAY;

// ColorPicker
Graphics::Texture* colorPickerCrosshairTexture;
Graphics::Texture* colorPickerHueTexture;
Graphics::Texture* colorPickerBrightnessTexture;
Graphics::Texture* colorPickerAlphaBrightnessTexture;
Graphics::Texture* colorPickerAlphaPatternTexture;
Color colorPickerBarBorderColor = COLOR::SILVER;
double colorPickerBarWidth = 0.05;
double colorPickerHueSize = 0.4;
double colorPickerCrosshairSize = 0.03;
double colorPickerSpacing = 0.06;
double colorPickerBarBorderWidth = 0.005;
double colorPickerSelectorWidth = 0.08;
double colorPickerSelectorHeight = 0.004;
Color colorPickerSelectorColor = COLOR::SILVER;

// CheckBox
Graphics::Texture* checkBoxUncheckedTexture;
Graphics::Texture* checkBoxCheckedTexture;
Graphics::Texture* checkBoxPressCheckedTexture;
Graphics::Texture* checkBoxPressUncheckedTexture;
Graphics::Texture* checkBoxHoverCheckedTexture;
Graphics::Texture* checkBoxHoverUncheckedTexture;
double checkBoxOffset = 0.006;
double checkBoxSize = 0.03;
double checkBoxLabelOffset = 0.02;

// Frame
double frameMinimumSize = 0.05;
double frameResizeHandleSize = 0.014;
double frameButtonOffset = 0.003;
double frameTitleBarHeight = 0.045;
Color frameTitleBarColor = COLOR::ACCENT;
Color frameBackgroundColor = COLOR::BACK;

// Font
Graphics::Font* font = nullptr;
Color fontColor = COLOR::SILVER;
double fontSize = 0.0007;

// Batch
Graphics::GuiBatch* batch;

void onInit(const WindowInfo& info, Graphics::FrameBuffer* screenFrameBuffer) {
	// Init
	GraphicsShaders::onInit();

	GUI::windowInfo = info;
	GUI::batch = new Graphics::GuiBatch();
	GUI::quad = new Quad();

	// font
	GUI::font = ResourceManager::add<FontResource>("font", "../res/fonts/droid.ttf");

	// closeButton
	GUI::closeButtonIdleTexture = ResourceManager::add<TextureResource>("closeButtonIdleTexture", "../res/textures/gui/close_idle.png");
	GUI::closeButtonHoverTexture = ResourceManager::add<TextureResource>("closeButtonHoverTexture", "../res/textures/gui/close_hover.png");
	GUI::closeButtonPressTexture = ResourceManager::add<TextureResource>("closeButtonPressTexture", "../res/textures/gui/close_press.png");

	// minimizeButton
	GUI::minimizeButtonIdleTexture = ResourceManager::add<TextureResource>("minimizeButtonIdleTexture", "../res/textures/gui/minimize_idle.png");
	GUI::minimizeButtonHoverTexture = ResourceManager::add<TextureResource>("minimizeButtonHoverTexture", "../res/textures/gui/minimize_hover.png");
	GUI::minimizeButtonPressTexture = ResourceManager::add<TextureResource>("minimizeButtonPressTexture", "../res/textures/gui/minimize_press.png");

	// Checkbox
	GUI::checkBoxUncheckedTexture = ResourceManager::add<TextureResource>("checkBoxUncheckedTexture", "../res/textures/gui/unchecked.png");
	GUI::checkBoxCheckedTexture = ResourceManager::add<TextureResource>("checkBoxCheckedTexture", "../res/textures/gui/checked.png");

	GUI::checkBoxPressCheckedTexture = GUI::checkBoxCheckedTexture->colored(Vec3(1.3));
	GUI::checkBoxPressUncheckedTexture = GUI::checkBoxUncheckedTexture->colored(Vec3(1.3));
	GUI::checkBoxHoverCheckedTexture = GUI::checkBoxCheckedTexture->colored(Vec3(0.999));
	GUI::checkBoxHoverUncheckedTexture = GUI::checkBoxUncheckedTexture->colored(Vec3(0.999));

	// ColorPicker
	GUI::colorPickerHueTexture = ResourceManager::add<TextureResource>("colorPickerHueTexture", "../res/textures/gui/hue.png");
	GUI::colorPickerAlphaBrightnessTexture = ResourceManager::add<TextureResource>("colorPickerAlphaBrightnessTexture", "../res/textures/gui/alphaBrightness.png");
	GUI::colorPickerAlphaPatternTexture = ResourceManager::add<TextureResource>("colorPickerAlphaPatternTexture", "../res/textures/gui/alphaPattern.png");
	GUI::colorPickerBrightnessTexture = ResourceManager::add<TextureResource>("colorPickerBrightnessTexture", "../res/textures/gui/brightness.png");
	GUI::colorPickerCrosshairTexture = ResourceManager::add<TextureResource>("colorPickerCrosshairTexture", "../res/textures/gui/crosshair.png");
}

void intersect(const Vec2& mouse) {
	Component* intersected = nullptr;
	for (Component* component : components) {
		// Skip hidden components
		if (!component->visible)
			continue;

		intersected = component->intersect(mouse);

		// Only update if intersection is found
		if (intersected) {
			// Skip already intersected component
			if (intersected == GUI::intersectedComponent)
				return;

			// Check nullpointer
			if (GUI::intersectedComponent)
				GUI::intersectedComponent->exit();

			// Update intersected
			GUI::intersectedComponent = intersected;
			GUI::intersectedComponent->enter();

			return;
		}
	}

	// No intersection found
	// Check nullpointer
	if (GUI::intersectedComponent)
		GUI::intersectedComponent->exit();

	// Reset intersected
	GUI::intersectedComponent = nullptr;
}

void select(Component* component) {
	components.select(component);
}

Component* superParent(Component* child) {
	Component* superParent = child;

	while (Component* newSuperParent = superParent->parent) {
		superParent = newSuperParent;
	}

	return superParent;
}

bool intersectsSquare(const Vec2& point, const Vec2& topleft, const Vec2& dimension) {
	Vec2 halfDimension = dimension / 2;
	Vec2 center = topleft + Vec2(halfDimension.x, -halfDimension.y);
	return fabs(point.x - center.x) < halfDimension.x && fabs(point.y - center.y) < halfDimension.y;
}

void add(Component* component) {
	components.add(component);
}

void remove(Component* component) {
	for (auto iterator = components.begin(); iterator != components.end(); iterator++) {
		if (component == *iterator) {
			components.remove(iterator);
			return;
		}
	}
}

bool onMouseMove(int newX, int newY) {
	if (GUI::intersectedComponent) {
		Vec2 guiCursorPosition = GUI::map(Vec2(newX, newY));
		GUI::intersectedComponent->hover(guiCursorPosition);
	}

	return false;
}

bool onMouseDrag(int oldX, int oldY, int newX, int newY) {
	if (GUI::selectedComponent) {
		Vec2 guiCursorPosition = GUI::map(Vec2(oldX, oldY));
		Vec2 newGuiCursorPosition = GUI::map(Vec2(newX, newY));

		GUI::selectedComponent->drag(newGuiCursorPosition, guiCursorPosition);

		return true;
	}

	return false;
}

bool onMouseRelease(int x, int y) {
	if (GUI::selectedComponent) {
		GUI::selectedComponent->release(GUI::map(Vec2(x, y)));
		GUI::selectedComponent = nullptr;
	}

	return false;
}

bool onMousePress(int x, int y) {
	GUI::selectedComponent = GUI::intersectedComponent;

	if (GUI::intersectedComponent) {
		GUI::select(GUI::superParent(GUI::intersectedComponent));
		GUI::intersectedComponent->press(GUI::map(Vec2(x, y)));
		GUI::intersectedPoint = GUI::map(Vec2(x, y)) - GUI::intersectedComponent->position;

		return true;
	}

	return false;
}

bool onWindowResize(const WindowInfo& info) {
	GUI::windowInfo = info;

	return false;
}

void onUpdate(Mat4f orthoMatrix) {
	GraphicsShaders::quadShader.updateProjection(orthoMatrix);
}

void onRender(Mat4f orthoMatrix) {

}

void onClose() {
	// Shaders
	GraphicsShaders::onClose();

	// Textures
	closeButtonHoverTexture->close();
	closeButtonIdleTexture->close();
	closeButtonPressTexture->close();
	minimizeButtonHoverTexture->close();
	minimizeButtonIdleTexture->close();
	minimizeButtonPressTexture->close();
	colorPickerCrosshairTexture->close();
	colorPickerHueTexture->close();
	colorPickerBrightnessTexture->close();
	colorPickerAlphaBrightnessTexture->close();
	colorPickerAlphaPatternTexture->close();
	checkBoxUncheckedTexture->close();
	checkBoxCheckedTexture->close();
	checkBoxPressCheckedTexture->close();
	checkBoxPressUncheckedTexture->close();
	checkBoxHoverCheckedTexture->close();
	checkBoxHoverUncheckedTexture->close();

	// Fonts
	font->close();
}
}

};