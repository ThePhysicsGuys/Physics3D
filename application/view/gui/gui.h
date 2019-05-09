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
	extern Quad* defaultQuad;
	extern QuadShader* defaultShader;
	
	// Default
	extern double defaultPadding;
	extern double defaultMargin;

	// Label
	extern Vec4 defaultLabelBackgroundColor;

	// Panel
	extern double defaultPanelPadding;
	extern double defaultPanelMargin;
	extern Vec4 defaultPanelBackgroundColor;

	// Button
	extern Texture* defaultCloseButtonHoverTexture;
	extern Texture* defaultCloseButtonIdleTexture;
	extern Texture* defaultCloseButtonPressTexture;
	extern Texture* defaultMinimizeButtonHoverTexture;
	extern Texture* defaultMinimizeButtonIdleTexture;
	extern Texture* defaultMinimizeButtonPressTexture;

	// Slider
	extern double defaultSliderWidth;
	extern double defaultSliderHandleWidth;
	extern double defaultSliderHeight;
	extern double defaultSliderHandleHeight;
	extern Vec4 defaultSliderHandleColor;
	extern Vec4 defaultSliderBackgroundColor;
	extern Vec4 defaultSliderForegroundFilledColor;
	extern Vec4 defaultSliderForegroundEmptyColor;

	// CheckBox
	extern Texture* defaultCheckBoxUncheckedTexture;
	extern Texture* defaultCheckBoxCheckedTexture;
	extern Texture* defaultCheckBoxPressCheckedTexture;
	extern Texture* defaultCheckBoxPressUncheckedTexture;
	extern Texture* defaultCheckBoxHoverCheckedTexture;
	extern Texture* defaultCheckBoxHoverUncheckedTexture;
	extern double defaultCheckBoxOffset;
	extern double defaultCheckBoxSize;
	extern double defaultCheckBoxLabelOffset;

	// Frame
	extern double defaultFrameTitleBarHeight;
	extern double defaultFrameButtonOffset;
	extern Vec4 defaultFrameTitleBarColor;
	extern Vec4 defaultFrameBackgroundColor;

	// Font
	extern Font* defaultFont;
	extern Vec4 defaultFontColor;
	extern double defaultFontSize;

	void add(Component* component);
	void remove(Component* component);

	Vec2 map(Vec2 point);
	Vec2 unmap(Vec2 point);
	void intersect(Vec2 mouse);
	bool intersectsSquare(Vec2 point, Vec2 topleft, Vec2 dimension);

	void init(Screen* screen, QuadShader* shader, Font* font);
	void update(Mat4f orthoMatrix);
	void render(Mat4f orthoMatrix);
};

#include "component.h"
