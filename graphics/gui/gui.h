#pragma once

#include "../batch/guiBatch.h"
#include "../batch/batchConfig.h"
#include "color.h"

class Component;
class ColorPicker;
class Frame;
class Texture;
class Font;
class FrameBuffer;
class IndexedMesh;
struct Quad;

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

	// ColorPicker
	extern Frame* colorPickerFrame;
	extern ColorPicker* colorPicker;
	
	// Default
	extern double padding;
	extern double margin;

	// Label
	extern Vec4f labelBackgroundColor;

	// Button
	extern Texture* closeButtonHoverTexture;
	extern Texture* closeButtonIdleTexture;
	extern Texture* closeButtonPressTexture;
	extern Texture* minimizeButtonHoverTexture;
	extern Texture* minimizeButtonIdleTexture;
	extern Texture* minimizeButtonPressTexture;
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
	extern Texture* colorPickerCrosshairTexture;
	extern Texture* colorPickerHueTexture;
	extern Texture* colorPickerBrightnessTexture;
	extern Texture* colorPickerAlphaBrightnessTexture;
	extern Texture* colorPickerAlphaPatternTexture;
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
	extern Texture* checkBoxUncheckedTexture;
	extern Texture* checkBoxCheckedTexture;
	extern Texture* checkBoxPressCheckedTexture;
	extern Texture* checkBoxPressUncheckedTexture;
	extern Texture* checkBoxHoverCheckedTexture;
	extern Texture* checkBoxHoverUncheckedTexture;
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
	extern Font* font;
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