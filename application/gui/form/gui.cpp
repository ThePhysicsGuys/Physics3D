#include "gui.h"
#include "component.h"

namespace GUI {
	// Shader
	GUIShader* defaultShader = nullptr;
	Quad* defaultQuad = nullptr;

	// Label
	double defaultLabelPadding = 0.01;
	double defaultLabelMargin = 0.01;
	Vec4 defaultLabelBackgroundColor = Vec4(0,1,0,1);

	// Panel
	double defaultPanelCloseButtonOffset = 0.004;
	Texture* defaultPanelCloseTexture;
	double defaultPanelPadding = 0.01;
	double defaultPanelMargin = 0.01;
	double defaultPanelTitleBarHeight = 0.05;
	Vec4 defaultPanelTitleBarColor = Vec4(0.12, 0.4, 0.47, 1);
	Vec4 defaultPanelBackgroundColor = Vec4(0.3, 0.3, 0.3, 1);

	// Font
	Font* defaultFont = nullptr;
	Vec4 defaultFontColor = Vec4(0, 0, 0, 1);
	double defaultFontSize = 0.001;

	void init(GUIShader* shader, Font* font) {
		GUI::defaultFont = font;
		GUI::defaultShader = shader;
		GUI::defaultQuad = new Quad();

		GUI::defaultPanelCloseTexture = load("../res/textures/gui/close.png");
	}

	void update(Mat4 orthoMatrix) {
		GUI::defaultShader->update(orthoMatrix);
	}
}