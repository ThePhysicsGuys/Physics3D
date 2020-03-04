#pragma once

#include "../batch/guiBatch.h"
#include "../batch/batchConfig.h"
#include "color.h"

class Component;
class ColorPicker;
class Frame;
class FrameBuffer;
class IndexedMesh;
struct Quad;
namespace Graphics {
class Font;
class Texture;
};

namespace GUI {

	// Window
	struct WindowInfo {
		Vec2i dimension;
		float aspect;
	};

	// Global
	extern WindowInfo windowInfo;
	extern Component* intersectedComponent;
	extern Component* selectedComponent;
	extern Vec2 intersectedPoint;
	extern FrameBuffer* guiFrameBuffer;
	extern FrameBuffer* blurFrameBuffer;

	// Default
	extern double padding;
	extern double margin;

	// Label
	extern Vec4f labelBackgroundColor;

	// Button
	extern Graphics::Texture* closeButtonHoverTexture;
	extern Graphics::Texture* closeButtonIdleTexture;
	extern Graphics::Texture* closeButtonPressTexture;
	extern Graphics::Texture* minimizeButtonHoverTexture;
	extern Graphics::Texture* minimizeButtonIdleTexture;
	extern Graphics::Texture* minimizeButtonPressTexture;
	extern Color borderColor;
	extern double borderWidth;

	// Slider
	extern double sliderBarWidth;
	extern double sliderHandleWidth;
	extern double sliderBarHeight;
	extern double sliderHandleHeight;
	extern Color sliderHandleColor;
	extern Color sliderBackgroundColor;
	extern Color sliderForegroundFilledColor;
	extern Color sliderForegroundEmptyColor;

	// ColorPicker
	extern Graphics::Texture* colorPickerCrosshairTexture;
	extern Graphics::Texture* colorPickerHueTexture;
	extern Graphics::Texture* colorPickerBrightnessTexture;
	extern Graphics::Texture* colorPickerAlphaBrightnessTexture;
	extern Graphics::Texture* colorPickerAlphaPatternTexture;
	extern Color colorPickerBarBorderColor;
	extern double colorPickerBarWidth;
	extern double colorPickerHueSize;
	extern double colorPickerSpacing;
	extern double colorPickerCrosshairSize;
	extern double colorPickerBarBorderWidth;
	extern double colorPickerSelectorWidth;
	extern double colorPickerSelectorHeight;
	extern Color colorPickerSelectorColor;

	// CheckBox
	extern Graphics::Texture* checkBoxUncheckedTexture;
	extern Graphics::Texture* checkBoxCheckedTexture;
	extern Graphics::Texture* checkBoxPressCheckedTexture;
	extern Graphics::Texture* checkBoxPressUncheckedTexture;
	extern Graphics::Texture* checkBoxHoverCheckedTexture;
	extern Graphics::Texture* checkBoxHoverUncheckedTexture;
	extern double checkBoxOffset;
	extern double checkBoxSize;
	extern double checkBoxLabelOffset;

	// Frame
	extern double frameMinimumSize;
	extern double frameResizeHandleSize;
	extern double frameTitleBarHeight;
	extern double frameButtonOffset;
	extern Color frameTitleBarColor;
	extern Color frameBackgroundColor;

	// Font
	extern Graphics::Font* font;
	extern Color fontColor;
	extern double fontSize;

	// Batch
	extern GuiBatch* batch;

	// Container functions
	void add(Component* component);
	void remove(Component* component);
	void select(Component* component);
	Component* superParent(Component* child);

	// Event functions
	void intersect(const Vec2& mouse);
	bool intersectsSquare(const Vec2& point, const Vec2& topleft, const Vec2& dimension);

	// State function
	void onInit(const WindowInfo& info, FrameBuffer* screenFrameBuffer);
	void onUpdate(Mat4f orthoMatrix);
	void onRender(Mat4f orthoMatrix);
	void onClose();

	// Events
	bool onMouseMove(int newX, int newY);
	bool onMouseDrag(int oldX, int oldY, int newX, int newY);
	bool onMouseRelease(int x, int y);
	bool onMousePress(int x, int y);
	bool onWindowResize(const WindowInfo& info);
};

#include "component.h"