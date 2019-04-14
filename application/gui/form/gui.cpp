#include "gui.h"
#include "component.h"

namespace GUI {
	// Shader
	GUIShader* defaultShader = nullptr;
	Quad* defaultQuad = nullptr;

	// Label
	double defaultLabelOffset = 0.1;
	Vec4 defaultLabelBackgroundColor = Vec4(0,1,0,1);

	// Panel
	double defaultPanelOffset = 0.01;
	double defaultPanelTitleBarHeight = 0.08;
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
	}

	void update(Mat4 orthoMatrix) {
		GUI::defaultShader->update(orthoMatrix);
	}
}