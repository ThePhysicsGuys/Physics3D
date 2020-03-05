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

// Blur framebuffer
Graphics::FrameBuffer* blurFrameBuffer;
Graphics::FrameBuffer* screenFrameBuffer;

// Batch
Graphics::GuiBatch* batch;

void onInit(const WindowInfo& info, Graphics::FrameBuffer* screenFrameBuffer) {
	// Init
	GraphicsShaders::onInit();

	GUI::windowInfo = info;
	GUI::batch = new Graphics::GuiBatch();
	GUI::quad = new Quad();
	GUI::guiFrameBuffer = new Graphics::FrameBuffer(windowInfo.dimension.x, windowInfo.dimension.y);
	GUI::blurFrameBuffer = new Graphics::FrameBuffer(windowInfo.dimension.x, windowInfo.dimension.y);
	GUI::screenFrameBuffer = screenFrameBuffer;

	// font
	ResourceManager::add<FontResource>("font", "../res/fonts/droid.ttf");
	GUI::font = ResourceManager::get<FontResource>("font");

	// closeButton
	ResourceManager::add<TextureResource>("closeButtonIdleTexture", "../res/textures/gui/close_idle.png");
	ResourceManager::add<TextureResource>("closeButtonHoverTexture", "../res/textures/gui/close_hover.png");
	ResourceManager::add<TextureResource>("closeButtonPressTexture", "../res/textures/gui/close_press.png");
	GUI::closeButtonIdleTexture = ResourceManager::get<TextureResource>("closeButtonIdleTexture");
	GUI::closeButtonHoverTexture = ResourceManager::get<TextureResource>("closeButtonHoverTexture");
	GUI::closeButtonPressTexture = ResourceManager::get<TextureResource>("closeButtonPressTexture");

	// minimizeButton
	ResourceManager::add<TextureResource>("minimizeButtonIdleTexture", "../res/textures/gui/minimize_idle.png");
	ResourceManager::add<TextureResource>("minimizeButtonHoverTexture", "../res/textures/gui/minimize_hover.png");
	ResourceManager::add<TextureResource>("minimizeButtonPressTexture", "../res/textures/gui/minimize_press.png");
	GUI::minimizeButtonIdleTexture = ResourceManager::get<TextureResource>("minimizeButtonIdleTexture");
	GUI::minimizeButtonHoverTexture = ResourceManager::get<TextureResource>("minimizeButtonHoverTexture");
	GUI::minimizeButtonPressTexture = ResourceManager::get<TextureResource>("minimizeButtonPressTexture");

	// Checkbox
	ResourceManager::add<TextureResource>("checkBoxUncheckedTexture", "../res/textures/gui/unchecked.png");
	ResourceManager::add<TextureResource>("checkBoxCheckedTexture", "../res/textures/gui/checked.png");
	GUI::checkBoxUncheckedTexture = ResourceManager::get<TextureResource>("checkBoxUncheckedTexture");
	GUI::checkBoxCheckedTexture = ResourceManager::get<TextureResource>("checkBoxCheckedTexture");
	GUI::checkBoxPressCheckedTexture = GUI::checkBoxCheckedTexture->colored(Vec3(1.3));
	GUI::checkBoxPressUncheckedTexture = GUI::checkBoxUncheckedTexture->colored(Vec3(1.3));
	GUI::checkBoxHoverCheckedTexture = GUI::checkBoxCheckedTexture->colored(Vec3(0.999));
	GUI::checkBoxHoverUncheckedTexture = GUI::checkBoxUncheckedTexture->colored(Vec3(0.999));

	// ColorPicker
	ResourceManager::add<TextureResource>("colorPickerHueTexture", "../res/textures/gui/hue.png");
	ResourceManager::add<TextureResource>("colorPickerAlphaBrightnessTexture", "../res/textures/gui/alphaBrightness.png");
	ResourceManager::add<TextureResource>("colorPickerAlphaPatternTexture", "../res/textures/gui/alphaPattern.png");
	ResourceManager::add<TextureResource>("colorPickerBrightnessTexture", "../res/textures/gui/brightness.png");
	ResourceManager::add<TextureResource>("colorPickerCrosshairTexture", "../res/textures/gui/crosshair.png");
	GUI::colorPickerHueTexture = ResourceManager::get<TextureResource>("colorPickerHueTexture");
	GUI::colorPickerAlphaBrightnessTexture = ResourceManager::get<TextureResource>("colorPickerAlphaBrightnessTexture");
	GUI::colorPickerAlphaPatternTexture = ResourceManager::get<TextureResource>("colorPickerAlphaPatternTexture");
	GUI::colorPickerBrightnessTexture = ResourceManager::get<TextureResource>("colorPickerBrightnessTexture");
	GUI::colorPickerCrosshairTexture = ResourceManager::get<TextureResource>("colorPickerCrosshairTexture");
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

	GUI::guiFrameBuffer->resize(info.dimension);
	GUI::blurFrameBuffer->resize(info.dimension);

	return false;
}

void onUpdate(Mat4f orthoMatrix) {
	GraphicsShaders::quadShader.updateProjection(orthoMatrix);
}

void onRender(Mat4f orthoMatrix) {

	// Render gui
	GUI::blurFrameBuffer->bind();
	GraphicsShaders::quadShader.updateProjection(orthoMatrix);
	GraphicsShaders::quadShader.updateTexture(screenFrameBuffer->texture);
	quad->render();
	GraphicsShaders::horizontalBlurShader.updateTexture(blurFrameBuffer->texture);
	GraphicsShaders::horizontalBlurShader.updateWidth(windowInfo.dimension.x);
	quad->render();
	GraphicsShaders::verticalBlurShader.updateTexture(blurFrameBuffer->texture);
	GraphicsShaders::verticalBlurShader.updateWidth(windowInfo.dimension.y);
	quad->render();
	blurFrameBuffer->unbind();
}

void onClose() {
	// Shaders
	GraphicsShaders::onClose();

	// Framebuffers
	guiFrameBuffer->close();
	blurFrameBuffer->close();

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