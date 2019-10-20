#pragma once

#include "../batch/guiBatch.h"
#include "../batch/batchConfig.h"

class Component;
class ColorPicker;
class Frame;
class Texture;
class Font;
class FrameBuffer;
class IndexedMesh;
class Event;
struct Quad;

namespace GUI {

	// Colors
	namespace COLOR {
		extern Vec4f DISABLED;
		extern Vec4f ACCENT;
		extern Vec4f BACK;
		extern Vec4f ALPHA;

		extern Vec4f R;
		extern Vec4f G;
		extern Vec4f B;
		extern Vec4f A;

		extern Vec4f NAVY;
		extern Vec4f BLUE;
		extern Vec4f AQUA;
		extern Vec4f TEAL;
		extern Vec4f OLIVE;
		extern Vec4f GREEN;
		extern Vec4f LIME;
		extern Vec4f YELLOW;
		extern Vec4f ORANGE;
		extern Vec4f RED;
		extern Vec4f MAROON;
		extern Vec4f FUCHSIA;
		extern Vec4f PURPLE;
		extern Vec4f BLACK;
		extern Vec4f GRAY;
		extern Vec4f SILVER;
		extern Vec4f WHITE;
			
		// format: 0xRRGGBBAA
		Vec4f get(int hex, bool alpha);
		Vec4f get(int hex);
		
		Vec3f hsvToRgb(Vec3f hsv);
		Vec3f rgbToHsv(Vec3f rgb);
		Vec4f hsvaToRgba(Vec4f hsva);
		Vec4f rgbaToHsva(Vec4f rgba);

		Vec4f blend(Vec4f color1, Vec4f color2);
	};

	// temp
	struct WindowInfo {
		Vec2i dimension;
		float aspect;
	};

	extern WindowInfo windowInfo;

	// Global
	extern Component* intersectedComponent;
	extern Component* selectedComponent;
	extern Vec2 intersectedPoint;
	extern FrameBuffer* guiFrameBuffer;

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
	extern Vec4f borderColor;
	extern double borderWidth;

	// Slider
	extern double sliderBarWidth;
	extern double sliderHandleWidth;
	extern double sliderBarHeight;
	extern double sliderHandleHeight;
	extern Vec4f sliderHandleColor;
	extern Vec4f sliderBackgroundColor;
	extern Vec4f sliderForegroundFilledColor;
	extern Vec4f sliderForegroundEmptyColor;

	// ColorPicker
	extern Texture* colorPickerCrosshairTexture;
	extern Texture* colorPickerHueTexture;
	extern Texture* colorPickerBrightnessTexture;
	extern Texture* colorPickerAlphaBrightnessTexture;
	extern Texture* colorPickerAlphaPatternTexture;
	extern Vec4f colorPickerBarBorderColor;
	extern double colorPickerBarWidth;
	extern double colorPickerHueSize;
	extern double colorPickerSpacing;
	extern double colorPickerCrosshairSize;
	extern double colorPickerBarBorderWidth;
	extern double colorPickerSelectorWidth;
	extern double colorPickerSelectorHeight;
	extern Vec4f colorPickerSelectorColor;

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
	extern double frameTitleBarHeight;
	extern double frameButtonOffset;
	extern Vec4f frameTitleBarColor;
	extern Vec4f frameBackgroundColor;

	// Font
	extern Font* font;
	extern Vec4f fontColor;
	extern double fontSize;

	// Batch
	extern GuiBatch* batch;

	// Container functions
	void add(Component* component);
	void remove(Component* component);
	void select(Component* component);
	Component* superParent(Component* child);

	// Event functions
	void intersect(Vec2 mouse);
	bool intersectsSquare(Vec2 point, Vec2 topleft, Vec2 dimension);

	// State function
	void onInit(const WindowInfo& info);
	void onEvent(Event& event);
	void onUpdate(const WindowInfo& info, Mat4f orthoMatrix);
	void onRender(Mat4f orthoMatrix);
	void onClose();
};

#include "component.h"