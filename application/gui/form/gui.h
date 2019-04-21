#pragma once

#include "../quad.h"
#include "../font.h"
#include "../shaderProgram.h"
#include "../screen.h"
#include "component.h"
#include "../texture.h"
#include "../../../engine/math/vec3.h"
#include "../../../engine/math/mat4.h"

namespace GUI {
	// Events
	extern Component* intersectedComponent;
	extern Component* selectedComponent;

	// Shader
	extern Quad* defaultQuad;
	extern GUIShader* defaultShader;
	
	// Label
	extern double defaultLabelPadding;
	extern double defaultLabelMargin;
	extern Vec4 defaultLabelBackgroundColor;

	// Panel
	extern double defaultPanelCloseButtonOffset;
	extern Texture* defaultPanelCloseTexture;
	extern double defaultPanelPadding;
	extern double defaultPanelMargin;
	extern double defaultPanelTitleBarHeight;
	extern Vec4 defaultPanelTitleBarColor;
	extern Vec4 defaultPanelBackgroundColor;

	// Font
	extern Font* defaultFont;
	extern Vec4 defaultFontColor;
	extern double defaultFontSize;

	void add(Component* component);
	void remove(Component* component);

	Vec2 map(Screen& screen, Vec2 point);
	void intersect(Vec2 mouse);
	bool intersectsSquare(Vec2 point, Vec2 center, Vec2 dimension);

	void init(GUIShader* shader, Font* font);
	void update(Mat4f orthoMatrix);
	void render(Mat4f orthoMatrix);
};
