#include "gui.h"
#include "component.h"
#include "container.h"
#include "orderedVector.h"

namespace GUI {

	namespace COLOR {
		Vec4 get(int hex, bool alpha) {
			Vec4 color;
			if (!alpha) hex = (hex << 8) | 0xFF;
			color.x = ((hex >> 24) & 0xFF) / 255.0;
			color.y = ((hex >> 16) & 0xFF) / 255.0;
			color.z = ((hex >> 8) & 0xFF) / 255.0;
			color.w = (hex & 0xFF) / 255.0;
			return color;
		}

		Vec4 get(int hex) {
			return get(hex, false);
		}

		Vec4 NAVY = get(0x001F3F);
		Vec4 BLUE = get(0x0074D9);
		Vec4 AQUA = get(0x7FDBFF);
		Vec4 TEAL = get(0x39CCCC);
		Vec4 OLIVE = get(0x3D9970);
		Vec4 GREEN = get(0x2ECC40);
		Vec4 LIME = get(0x01FF70);
		Vec4 YELLOW	= get(0xFFDC00);
		Vec4 ORANGE	= get(0xFF851B);
		Vec4 RED = get(0xFF4136);
		Vec4 MAROON	= get(0x85144b);
		Vec4 FUCHSIA = get(0xF012BE);
		Vec4 PURPLE	= get(0xB10DC9);
		Vec4 BLACK = get(0x111111);
		Vec4 GRAY = get(0xAAAAAA);
		Vec4 SILVER	= get(0xDDDDDD);
		Vec4 WHITE = get(0xFFFFFF);
	};

	// Global
	OrderedVector<Component*> components;
	Component* intersectedComponent;
	Component* selectedComponent;

	// Shader
	QuadShader* defaultShader = nullptr;
	Quad* defaultQuad = nullptr;

	// Label
	double defaultLabelPadding = 0.01;
	double defaultLabelMargin = 0.01;
	Vec4 defaultLabelBackgroundColor = Vec4(0,1,0,1);

	// Panel
	double defaultPanelPadding = 0.01;
	double defaultPanelMargin = 0.01;
	Vec4 defaultPanelBackgroundColor = Vec4(0.3, 0.3, 0.3, 1);

	// Frame
	Texture* defaultFrameCloseTexture;
	double defaultFrameCloseButtonOffset = 0.01;
	double defaultFrameTitleBarHeight = 0.06;
	double defaultFramePadding = 0.01;
	double defaultFrameMargin = 0.01;
	Vec4 defaultFrameTitleBarColor = Vec4(0.12, 0.4, 0.47, 1);
	Vec4 defaultFrameBackgroundColor = Vec4(0.3, 0.3, 0.3, 1);

	// Font
	Font* defaultFont = nullptr;
	Vec4 defaultFontColor = Vec4(0, 0, 0, 1);
	double defaultFontSize = 0.001;

	void init(QuadShader* shader, Font* font) {
		GUI::defaultFont = font;
		GUI::defaultShader = shader;
		GUI::defaultQuad = new Quad();

		GUI::defaultFrameCloseTexture = load("../res/textures/gui/close.png");
	}
	
	void update(Mat4f orthoMatrix) {
		GUI::defaultShader->update(orthoMatrix);
	}

	double map(double x, double in_min, double in_max, double out_min, double out_max) {
		return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}

	Vec2 map(Screen& screen, Vec2 point) {
		return Vec2(map(point.x, 0, screen.dimension.x, -screen.aspect, screen.aspect), map(screen.dimension.y - point.y, 0, screen.dimension.y, -1, 1));
	}

	void intersect(Vec2 mouse) {
		Component* intersected = nullptr;
		for (Component* component : components) {
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

	bool intersectsSquare(Vec2 point, Vec2 topleft, Vec2 dimension) {
		Vec2 halfDimension = dimension / 2;
		Vec2 center = topleft + Vec2(halfDimension.x, -halfDimension.y);
		return fabs(point.x - center.x) < halfDimension.x && fabs(point.y - center.y) < halfDimension.y;
	}

	void add(Component* component) {
		components.add(component);
	}

	// Needs to be optimized
	void remove(Component* component) {
		for (auto iterator = components.begin(); iterator != components.end(); iterator++) {
			if (component == *iterator) {
				components.remove(iterator);
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
