#include "gui.h"
#include "component.h"
#include "container.h"

namespace GUI {
	// Global
	std::vector<Component*> components;
	Component* intersectedComponent;
	Component* selectedComponent;

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
	
	void update(Mat4f orthoMatrix) {
		GUI::defaultShader->update(orthoMatrix);
	}

	double map(double x, double in_min, double in_max, double out_min, double out_max) {
		return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}

	Vec2 map(Screen& screen, Vec2 point) {
		return Vec2(map(point.x, 0, screen.screenSize.x, -screen.aspect, screen.aspect), map(screen.screenSize.y - point.y, 0, screen.screenSize.y, -1, 1));
	}

	void intersect(Vec2 mouse) {
		for (Component* component : components) {
			Component* intersected = component->intersect(mouse);
			if (intersected) {
				intersectedComponent = intersected;
				return;
			}
		}
		intersectedComponent = nullptr;
	}

	bool intersectsSquare(Vec2 point, Vec2 topleft, Vec2 dimension) {
		Vec2 halfDimension = dimension / 2;
		Vec2 center = topleft + Vec2(halfDimension.x, -halfDimension.y);
		return fabs(point.x - center.x) < halfDimension.x && fabs(point.y - center.y) < halfDimension.y;
	}

	void add(Component* component) {
		components.push_back(component);
	}

	void remove(Component* component) {
		for (auto iterator = components.begin(); iterator != components.end(); iterator++) {
			if (component == *iterator) {
				components.erase(iterator);
				return;
			}
		}
	}

	void render(Mat4f orthoMatrix) {
		update(orthoMatrix);
		for (Component* component : components)
			component->render();
	}
}