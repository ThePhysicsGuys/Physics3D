#pragma once

#include "../quad.h"
#include "../font.h"
#include "../shaderProgram.h"
#include "../screen.h"
#include "../texture.h"


#include "../engine/math/vec3.h"
#include "../engine/math/vec4.h"
#include "../engine/math/mat4.h"

#include <string>

class Component;

namespace GUI {

	// Colors
	namespace COLOR {
		extern Vec4 ACCENT;
		extern Vec4 BACK;
		extern Vec4 ALPHA;

		extern Vec4 R;
		extern Vec4 G;
		extern Vec4 B;
		extern Vec4 A;

		extern Vec4 NAVY;
		extern Vec4 BLUE;
		extern Vec4 AQUA;
		extern Vec4 TEAL;
		extern Vec4 OLIVE;
		extern Vec4 GREEN;
		extern Vec4 LIME;
		extern Vec4 YELLOW;
		extern Vec4 ORANGE;
		extern Vec4 RED;
		extern Vec4 MAROON;
		extern Vec4 FUCHSIA;
		extern Vec4 PURPLE;
		extern Vec4 BLACK;
		extern Vec4 GRAY;
		extern Vec4 SILVER;
		extern Vec4 WHITE;
			
		// format: 0xRRGGBBAA
		Vec4 get(int hex, bool alpha);
		Vec4 get(int hex);
		Vec3 hsvToRgb(Vec3 hsv);
		Vec3 rgbToHsv(Vec3 rgb);
		Vec4 hsvaToRgba(Vec4 hsva);
		Vec4 rgbaToHsva(Vec4 rgba);
	};

	// Global
	extern Screen* screen;
	extern Component* intersectedComponent;
	extern Component* selectedComponent;
	extern Vec2 intersectedPoint;

	// Shader
	extern Quad* quad;
	extern QuadShader* shader;
	extern BlurShader* blurShader;
	
	// Default
	extern double padding;
	extern double margin;

	// Label
	extern Vec4 labelBackgroundColor;

	// Button
	extern Texture* closeButtonHoverTexture;
	extern Texture* closeButtonIdleTexture;
	extern Texture* closeButtonPressTexture;
	extern Texture* minimizeButtonHoverTexture;
	extern Texture* minimizeButtonIdleTexture;
	extern Texture* minimizeButtonPressTexture;
	extern Vec4 borderColor;
	extern double borderWidth;

	// Slider
	extern double sliderBarWidth;
	extern double sliderHandleWidth;
	extern double sliderBarHeight;
	extern double sliderHandleHeight;
	extern Vec4 sliderHandleColor;
	extern Vec4 sliderBackgroundColor;
	extern Vec4 sliderForegroundFilledColor;
	extern Vec4 sliderForegroundEmptyColor;

	// ColorPicker
	extern Texture* colorPickerCrosshairTexture;
	extern Texture* colorPickerHueTexture;
	extern Texture* colorPickerBrightnessTexture;
	extern Texture* colorPickerAlphaBrightnessTexture;
	extern Texture* colorPickerAlphaPatternTexture;
	extern Vec4 colorPickerBarBorderColor;
	extern double colorPickerBarWidth;
	extern double colorPickerHueSize;
	extern double colorPickerSpacing;
	extern double colorPickerCrosshairSize;
	extern double colorPickerBarBorderWidth;
	extern double colorPickerSelectorWidth;
	extern double colorPickerSelectorHeight;
	extern Vec4 colorPickerSelectorColor;

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
	extern Vec4 frameTitleBarColor;
	extern Vec4 frameBackgroundColor;

	// Font
	extern Font* font;
	extern Vec4 fontColor;
	extern double fontSize;

	void add(Component* component);
	void remove(Component* component);
	void select(Component* component);

	Component* superParent(Component* child);

	double clamp(double value, double min, double max);
	double map(double x, double minIn, double maxIn, double minOut, double maxOut);
	Vec2 map(Vec2 point);
	Vec2 unmap(Vec2 point);
	void intersect(Vec2 mouse);
	bool intersectsSquare(Vec2 point, Vec2 topleft, Vec2 dimension);

	void init(Screen* screen, QuadShader* shader, BlurShader* blurShader, Font* font);
	void update(Mat4f orthoMatrix);
	void render(Mat4f orthoMatrix);
};

#include "component.h"
