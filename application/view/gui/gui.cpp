#include "gui.h"

#include "component.h"
#include "container.h"
#include "orderedVector.h"
#include "colorPicker.h"
#include "frame.h"
#include "../path/path.h"

#include "../../visualShape.h"
#include "../../io/import.h"

#include "../font.h"
#include "../texture.h"
#include "../screen.h"
#include "../shaderProgram.h"
#include "../renderUtils.h"

#include "../buffers/frameBuffer.h"
#include "../mesh/indexedMesh.h"
#include "../mesh/primitive.h"

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

		Vec3 hsvToRgb(Vec3 hsv) {
			double h = hsv.x * 360.0;
			double s = hsv.y;
			double v = hsv.z;

			if (s == 0.0)
				return Vec3(v, v, v);
				
			int hi = (int)(h / 60.0) % 6;
			double f = (h / 60.0) - hi;
			double p = v * (1.0 - s);
			double q = v * (1.0 - s * f);
			double t = v * (1.0 - s * (1.0 - f));
			
			switch (hi) {
				case 0:
					return Vec3(v, t, p);
					break;
				case 1:
					return Vec3(q, v, p);
					break;
				case 2:
					return Vec3(p, v, t);
					break;
				case 3:
					return Vec3(p, q, v);
					break;
				case 4:
					return Vec3(t, p, v);
					break;
				case 5:
					return Vec3(v, p, q);
					break;
			}

			return Vec3();
		}

		Vec4 hsvaToRgba(Vec4 hsva) {
			Vec3 color = hsvToRgb(Vec3(hsva.x, hsva.y, hsva.z));
			return Vec4(color.x, color.y, color.z, hsva.w);
		}

		Vec4 rgbaToHsva(Vec4 rgba) {
			Vec3 color = rgbToHsv(Vec3(rgba.x, rgba.y, rgba.z));
			return Vec4(color.x, color.y, color.z, rgba.w);
		}

		Vec3 rgbToHsv(Vec3 rgb) {
			double r = rgb.x;
			double g = rgb.y;
			double b = rgb.z;

			double h = 0;
			double s = 0;
			double v = 0;

			double min = fmin(fmin(r, g), b);
			double max = fmax(fmax(r, g), b);

			double d = max - min;
			v = max;

			if (d == 0) {
				h = 0;
				s = 0;
			} else {
				s = d / max;

				double dr = (((max - r) / 6.0) + (max / 2.0)) / d;
				double dg = (((max - g) / 6.0) + (max / 2.0)) / d;
				double db = (((max - b) / 6.0) + (max / 2.0)) / d;

				if (r == max)
					h = db - dg;
				else if (g == max)
					h = 1.0 / 3.0 + dr - db;
				else if (b == max)
					h = 2.0 / 3.0 + dg - dr;

				if (h < 0)
					h += 1;
				if (h > 1)
					h -= 1;
			
			}

			return Vec3(h, s, v);
		}

		Vec4 blend(Vec4 color1, Vec4 color2) {
			return Vec4(color1.x * color2.x, color1.y * color2.y, color1.z * color2.z, color1.w * color2.w);
		}

		Vec4 DISABLED = get(0xA0A0A0);
		Vec4 ACCENT = get(0x1F6678);
		Vec4 BACK = get(0x4D4D4D);
		Vec4 ALPHA = get(0x0, true);

		Vec4 R = get(0xFF0000);
		Vec4 G = get(0x00FF00);
		Vec4 B = get(0x0000FF);
		Vec4 A = get(0x0, true);

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
	Screen* screen;
	OrderedVector<Component*> components;
	FrameBuffer* guiFrameBuffer = nullptr;
	Component* intersectedComponent = nullptr;
	Component* selectedComponent = nullptr;
	Vec2 intersectedPoint;

	// Defaults
	double margin = 0.01;
	double padding = 0.01;

	// ColorPicker
	Frame* colorPickerFrame = nullptr;
	ColorPicker* colorPicker = nullptr;

	// Quad
	Quad* quad = nullptr;

	// Label
	Vec4 labelBackgroundColor = COLOR::WHITE;

	// Button
	Texture* closeButtonHoverTexture;
	Texture* closeButtonIdleTexture;
	Texture* closeButtonPressTexture;
	Texture* minimizeButtonHoverTexture;
	Texture* minimizeButtonIdleTexture;
	Texture* minimizeButtonPressTexture;
	Vec4 borderColor = COLOR::SILVER;
	double borderWidth = 0.004;

	// Slider
	double sliderBarWidth = 0.4;
	double sliderHandleWidth = 0.02;
	double sliderBarHeight = 0.004;
	double sliderHandleHeight = 0.04;
	Vec4 sliderHandleColor = COLOR::BACK;
	Vec4 sliderBackgroundColor = COLOR::ALPHA;
	Vec4 sliderForegroundFilledColor = COLOR::ACCENT;
	Vec4 sliderForegroundEmptyColor = COLOR::GRAY;
	
	// ColorPicker
	Texture* colorPickerCrosshairTexture;
	Texture* colorPickerHueTexture;
	Texture* colorPickerBrightnessTexture;
	Texture* colorPickerAlphaBrightnessTexture;
	Texture* colorPickerAlphaPatternTexture;
	Vec4 colorPickerBarBorderColor = COLOR::SILVER;
	double colorPickerBarWidth = 0.05;
	double colorPickerHueSize = 0.4;
	double colorPickerCrosshairSize = 0.03;	
	double colorPickerSpacing = 0.06;
	double colorPickerBarBorderWidth = 0.005;
	double colorPickerSelectorWidth = 0.08;
	double colorPickerSelectorHeight = 0.004;
	Vec4 colorPickerSelectorColor = COLOR::SILVER;
	
	// CheckBox
	Texture* checkBoxUncheckedTexture;
	Texture* checkBoxCheckedTexture;
	Texture* checkBoxPressCheckedTexture;
	Texture* checkBoxPressUncheckedTexture;
	Texture* checkBoxHoverCheckedTexture;
	Texture* checkBoxHoverUncheckedTexture;
	double checkBoxOffset = 0.006;
	double checkBoxSize = 0.03;
	double checkBoxLabelOffset = 0.02;

	// Frame
	double frameButtonOffset = 0.003;
	double frameTitleBarHeight = 0.06;
	Vec4 frameTitleBarColor = COLOR::ACCENT;
	Vec4 frameBackgroundColor = COLOR::BACK;

	// Font
	Font* font = nullptr;
	Vec4 fontColor = COLOR::SILVER;
	double fontSize = 0.0009;

	GuiBatch* batch;

	void init(Screen* screen, Font* font) {

		GUI::batch = new GuiBatch();

		GUI::screen = screen;
		GUI::font = font;
		GUI::guiFrameBuffer = new FrameBuffer(screen->dimension.x, screen->dimension.y);
		GUI::quad = new Quad();

		// closeButton
		GUI::closeButtonIdleTexture = load("../res/textures/gui/close_idle.png");
		GUI::closeButtonHoverTexture = load("../res/textures/gui/close_hover.png");
		GUI::closeButtonPressTexture = load("../res/textures/gui/close_press.png");

		// minimizeButton
		GUI::minimizeButtonIdleTexture = load("../res/textures/gui/minimize_idle.png");
		GUI::minimizeButtonHoverTexture = load("../res/textures/gui/minimize_hover.png");
		GUI::minimizeButtonPressTexture = load("../res/textures/gui/minimize_press.png");

		// Checkbox
		GUI::checkBoxUncheckedTexture = load("../res/textures/gui/unchecked.png");
		GUI::checkBoxCheckedTexture = load("../res/textures/gui/checked.png");
		GUI::checkBoxPressCheckedTexture = GUI::checkBoxCheckedTexture->colored(Vec3(1.3));
		GUI::checkBoxPressUncheckedTexture = GUI::checkBoxUncheckedTexture->colored(Vec3(1.3));
		GUI::checkBoxHoverCheckedTexture = GUI::checkBoxCheckedTexture->colored(Vec3(0.999));
		GUI::checkBoxHoverUncheckedTexture = GUI::checkBoxUncheckedTexture->colored(Vec3(0.999));

		// ColorPicker
		GUI::colorPickerHueTexture = load("../res/textures/gui/hue.png");
		GUI::colorPickerAlphaBrightnessTexture = load("../res/textures/gui/alphaBrightness.png");
		GUI::colorPickerAlphaPatternTexture = load("../res/textures/gui/alphaPattern.png");
		GUI::colorPickerBrightnessTexture = load("../res/textures/gui/brightness.png");
		GUI::colorPickerCrosshairTexture = load("../res/textures/gui/crosshair.png");

		GUI::colorPicker = new ColorPicker(0, 0);
		GUI::colorPickerFrame = new Frame(0, 0, "Color");
		GUI::colorPickerFrame->add(colorPicker);
		GUI::colorPickerFrame->visible = false;
		GUI::add(colorPickerFrame);
	}
	
	void intersect(Vec2 mouse) {
		Component* intersected = nullptr;
		for (Component* component : components) {
			// Skip hidden components
			if (!component->visible) 
				continue;

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

	void select(Component* component) {
		components.select(component);
	}

	Component* superParent(Component* child) {
		Component* superParent = child;
		
		while (Component* newSuperParent = superParent->parent) {
			superParent = newSuperParent;
		}

		return superParent;
	}

	bool intersectsSquare(Vec2 point, Vec2 topleft, Vec2 dimension) {
		Vec2 halfDimension = dimension / 2;
		Vec2 center = topleft + Vec2(halfDimension.x, -halfDimension.y);
		return fabs(point.x - center.x) < halfDimension.x && fabs(point.y - center.y) < halfDimension.y;
	}

	void add(Component* component) {
		components.add(component);
	}

	void remove(Component* component) {
		for (auto iterator = components.begin(); iterator != components.end(); iterator++) {
			if (component == *iterator) {
				components.remove(iterator);
				return;
			}
		}
	}

	void update(Mat4f orthoMatrix) {
		Shaders::quadShader.updateProjection(orthoMatrix);
	}

	void render(Mat4f orthoMatrix) {
		update(orthoMatrix);

		screen->blurFrameBuffer->bind();
		Shaders::quadShader.updateTexture(screen->screenFrameBuffer->texture);
		screen->quad->render();
		Shaders::blurShader.updateTexture(screen->blurFrameBuffer->texture);
		Shaders::blurShader.updateType(BlurShader::BlurType::HORIZONTAL);
		screen->quad->render();
		Shaders::blurShader.updateType(BlurShader::BlurType::VERTICAL);
		screen->quad->render();
		screen->blurFrameBuffer->unbind();

		Path::bind(GUI::batch);
		for (auto iterator = components.rbegin(); iterator != components.rend(); ++iterator) {
			(*iterator)->render();
		}

		Renderer::disableCulling();
		Renderer::enableBlending();
		Shaders::guiShader.init(screen->camera.orthoMatrix);
		GUI::batch->submit();
	}

	void close() {
		// Framebuffers
		guiFrameBuffer->close();

		// Textures
		closeButtonHoverTexture->close();
		closeButtonIdleTexture->close();
		closeButtonPressTexture->close();
		minimizeButtonHoverTexture->close();
		minimizeButtonIdleTexture->close();
		minimizeButtonPressTexture->close();
		colorPickerCrosshairTexture->close();
		colorPickerHueTexture->close();
		colorPickerBrightnessTexture->close();
		colorPickerAlphaBrightnessTexture->close();
		colorPickerAlphaPatternTexture->close();
		checkBoxUncheckedTexture->close();
		checkBoxCheckedTexture->close();
		checkBoxPressCheckedTexture->close();
		checkBoxPressUncheckedTexture->close();
		checkBoxHoverCheckedTexture->close();
		checkBoxHoverUncheckedTexture->close();
		
		// Fonts
		font->close();
	}
}