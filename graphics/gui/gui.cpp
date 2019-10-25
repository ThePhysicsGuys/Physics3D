#include "core.h"

#include "gui.h"
#include "guiUtils.h"

#include "component.h"
#include "container.h"
#include "orderedVector.h"
#include "colorPicker.h"
#include "frame.h"
#include "path/path.h"
#include "shader/shaders.h"

#include "../engine/event/event.h"
#include "../engine/event/mouseEvent.h"
#include "../engine/input/mouse.h"
#include "../engine/io/import.h"

#include "font.h"
#include "texture.h"
#include "visualShape.h"
#include "renderUtils.h"
#include "shader/shaderProgram.h"

#include "../buffers/frameBuffer.h"
#include "../mesh/indexedMesh.h"
#include "../mesh/primitive.h"

#include "../engine/resource/resourceManager.h"
#include "../engine/resource/textureResource.h"
#include "../engine/resource/fontResource.h"

namespace GUI {
	namespace COLOR {
		Vec4f get(int hex, bool alpha) {
			Vec4f color;
			if (!alpha) hex = (hex << 8) | 0xFF;
			color.x = ((hex >> 24) & 0xFF) / 255.0f;
			color.y = ((hex >> 16) & 0xFF) / 255.0f;
			color.z = ((hex >> 8) & 0xFF) / 255.0f;
			color.w = (hex & 0xFF) / 255.0f;
			return color;
		}

		Vec4f get(int hex) {
			return get(hex, false);
		}

		Vec3f hsvToRgb(Vec3f hsv) {
			float h = hsv.x * 360.0f;
			float s = hsv.y;
			float v = hsv.z;

			if (s == 0.0)
				return Vec3f(v, v, v);
				
			int hi = (int) (h / 60.0f) % 6;
			float f = (h / 60.0f) - hi;
			float p = v * (1.0f - s);
			float q = v * (1.0f - s * f);
			float t = v * (1.0f - s * (1.0f - f));
			
			switch (hi) {
				case 0:
					return Vec3f(v, t, p);
					break;
				case 1:
					return Vec3f(q, v, p);
					break;
				case 2:
					return Vec3f(p, v, t);
					break;
				case 3:
					return Vec3f(p, q, v);
					break;
				case 4:
					return Vec3f(t, p, v);
					break;
				case 5:
					return Vec3f(v, p, q);
					break;
			}

			return Vec3f();
		}

		Vec4f hsvaToRgba(Vec4f hsva) {
			Vec3f color = hsvToRgb(Vec3f(hsva.x, hsva.y, hsva.z));
			return Vec4f(color.x, color.y, color.z, hsva.w);
		}

		Vec4f rgbaToHsva(Vec4f rgba) {
			Vec3f color = rgbToHsv(Vec3f(rgba.x, rgba.y, rgba.z));
			return Vec4f(color.x, color.y, color.z, rgba.w);
		}

		Vec3f rgbToHsv(Vec3f rgb) {
			float r = rgb.x;
			float g = rgb.y;
			float b = rgb.z;
			
			float h = 0.0f;
			float s = 0.0f;
			float v = 0.0f;
			
			float min = fmin(fmin(r, g), b);
			float max = fmax(fmax(r, g), b);
			
			float d = max - min;
			v = max;

			if (d == 0.0f) {
				h = 0.0f;
				s = 0.0f;
			} else {
				s = d / max;

				float dr = (((max - r) / 6.0f) + (max / 2.0f)) / d;
				float dg = (((max - g) / 6.0f) + (max / 2.0f)) / d;
				float db = (((max - b) / 6.0f) + (max / 2.0f)) / d;

				if (r == max)
					h = db - dg;
				else if (g == max)
					h = 1.0f / 3.0f + dr - db;
				else if (b == max)
					h = 2.0f / 3.0f + dg - dr;

				if (h < 0.0f)
					h += 1.0f;
				if (h > 1.0f)
					h -= 1.0f;
			
			}

			return Vec3f(h, s, v);
		}

		Vec4f blend(Vec4f color1, Vec4f color2) {
			return Vec4f(color1.x * color2.x, color1.y * color2.y, color1.z * color2.z, color1.w * color2.w);
		}

		Vec4f DISABLED = get(0xA0A0A0);
		Vec4f ACCENT   = get(0x1F6678);
		Vec4f BACK     = get(0x4D4D4D);
		Vec4f ALPHA    = get(0x0, true);

		Vec4f R        = get(0xFF0000);
		Vec4f G        = get(0x00FF00);
		Vec4f B        = get(0x0000FF);
		Vec4f A        = get(0x0, true);

		Vec4f NAVY     = get(0x001F3F);
		Vec4f BLUE     = get(0x0074D9);
		Vec4f AQUA     = get(0x7FDBFF);
		Vec4f TEAL     = get(0x39CCCC);
		Vec4f OLIVE    = get(0x3D9970);
		Vec4f GREEN    = get(0x2ECC40);
		Vec4f LIME     = get(0x01FF70);
		Vec4f YELLOW   = get(0xFFDC00);
		Vec4f ORANGE   = get(0xFF851B);
		Vec4f RED      = get(0xFF4136);
		Vec4f MAROON   = get(0x85144b);
		Vec4f FUCHSIA  = get(0xF012BE);
		Vec4f PURPLE   = get(0xB10DC9);
		Vec4f BLACK    = get(0x111111);
		Vec4f GRAY     = get(0xAAAAAA);
		Vec4f SILVER   = get(0xDDDDDD);
		Vec4f WHITE    = get(0xFFFFFF);
	};

	// Temp
	WindowInfo windowInfo;

	// Global
	OrderedVector<Component*> components;
	FrameBuffer* guiFrameBuffer = nullptr;
	Component* intersectedComponent = nullptr;
	Component* selectedComponent = nullptr;
	Vec2 intersectedPoint;

	// Defaults
	double margin = 0.005;
	double padding = 0.01;

	// ColorPicker
	Frame* colorPickerFrame = nullptr;
	ColorPicker* colorPicker = nullptr;

	// Quad
	Quad* quad = nullptr;

	// Label
	Vec4f labelBackgroundColor = COLOR::WHITE;

	// Button
	Texture* closeButtonHoverTexture;
	Texture* closeButtonIdleTexture;
	Texture* closeButtonPressTexture;
	Texture* minimizeButtonHoverTexture;
	Texture* minimizeButtonIdleTexture;
	Texture* minimizeButtonPressTexture;
	Vec4f borderColor = COLOR::SILVER;
	double borderWidth = 0.004;

	// Slider
	double sliderBarWidth = 0.4;
	double sliderHandleWidth = 0.02;
	double sliderBarHeight = 0.004;
	double sliderHandleHeight = 0.04;
	Vec4f sliderHandleColor = COLOR::BACK;
	Vec4f sliderBackgroundColor = COLOR::ALPHA;
	Vec4f sliderForegroundFilledColor = COLOR::ACCENT;
	Vec4f sliderForegroundEmptyColor = COLOR::GRAY;
	
	// ColorPicker
	Texture* colorPickerCrosshairTexture;
	Texture* colorPickerHueTexture;
	Texture* colorPickerBrightnessTexture;
	Texture* colorPickerAlphaBrightnessTexture;
	Texture* colorPickerAlphaPatternTexture;
	Vec4f colorPickerBarBorderColor = COLOR::SILVER;
	double colorPickerBarWidth = 0.05;
	double colorPickerHueSize = 0.4;
	double colorPickerCrosshairSize = 0.03;	
	double colorPickerSpacing = 0.06;
	double colorPickerBarBorderWidth = 0.005;
	double colorPickerSelectorWidth = 0.08;
	double colorPickerSelectorHeight = 0.004;
	Vec4f colorPickerSelectorColor = COLOR::SILVER;
	
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
	double frameTitleBarHeight = 0.045;
	Vec4f frameTitleBarColor = COLOR::ACCENT;
	Vec4f frameBackgroundColor = COLOR::BACK;

	// Font
	Font* font = nullptr;
	Vec4f fontColor = COLOR::SILVER;
	double fontSize = 0.0007;

	// Blur framebuffer
	FrameBuffer* blurFrameBuffer;
	FrameBuffer* screenFrameBuffer;

	// Batch
	GuiBatch* batch;

	void onInit(const WindowInfo& info, FrameBuffer* screenFrameBuffer) {
		// Init
		GraphicsShaders::onInit();

		GUI::windowInfo = info;
		GUI::batch = new GuiBatch();
		GUI::quad = new Quad();
		GUI::guiFrameBuffer = new FrameBuffer(windowInfo.dimension.x, windowInfo.dimension.y);
		GUI::blurFrameBuffer = new FrameBuffer(windowInfo.dimension.x, windowInfo.dimension.y);
		GUI::screenFrameBuffer = screenFrameBuffer;

		// font
		ResourceManager::add<FontResource>("font", "../res/fonts/droid.ttf");
		GUI::font = ResourceManager::get<FontResource>("font");

		// closeButton
		ResourceManager::add<TextureResource>("closeButtonIdleTexture", "../res/textures/gui/close_idle.png");
		ResourceManager::add<TextureResource>("closeButtonHoverTexture", "../res/textures/gui/close_hover.png");
		ResourceManager::add<TextureResource>("closeButtonPressTexture", "../res/textures/gui/close_press.png");
		GUI::closeButtonIdleTexture = ResourceManager::get<TextureResource>("closeButtonIdleTexture");
		GUI::closeButtonHoverTexture = ResourceManager::get<TextureResource>("closeButtonHoverTexture");
		GUI::closeButtonPressTexture = ResourceManager::get<TextureResource>("closeButtonPressTexture");

		// minimizeButton
		ResourceManager::add<TextureResource>("minimizeButtonIdleTexture", "../res/textures/gui/minimize_idle.png");
		ResourceManager::add<TextureResource>("minimizeButtonHoverTexture", "../res/textures/gui/minimize_hover.png");
		ResourceManager::add<TextureResource>("minimizeButtonPressTexture", "../res/textures/gui/minimize_press.png");
		GUI::minimizeButtonIdleTexture = ResourceManager::get<TextureResource>("minimizeButtonIdleTexture");
		GUI::minimizeButtonHoverTexture = ResourceManager::get<TextureResource>("minimizeButtonHoverTexture");
		GUI::minimizeButtonPressTexture = ResourceManager::get<TextureResource>("minimizeButtonPressTexture");

		// Checkbox
		ResourceManager::add<TextureResource>("checkBoxUncheckedTexture", "../res/textures/gui/unchecked.png");
		ResourceManager::add<TextureResource>("checkBoxCheckedTexture", "../res/textures/gui/checked.png");
		GUI::checkBoxUncheckedTexture = ResourceManager::get<TextureResource>("checkBoxUncheckedTexture");
		GUI::checkBoxCheckedTexture = ResourceManager::get<TextureResource>("checkBoxCheckedTexture");
		GUI::checkBoxPressCheckedTexture = GUI::checkBoxCheckedTexture->colored(Vec3(1.3));
		GUI::checkBoxPressUncheckedTexture = GUI::checkBoxUncheckedTexture->colored(Vec3(1.3));
		GUI::checkBoxHoverCheckedTexture = GUI::checkBoxCheckedTexture->colored(Vec3(0.999));
		GUI::checkBoxHoverUncheckedTexture = GUI::checkBoxUncheckedTexture->colored(Vec3(0.999));

		// ColorPicker
		ResourceManager::add<TextureResource>("colorPickerHueTexture", "../res/textures/gui/hue.png");
		ResourceManager::add<TextureResource>("colorPickerAlphaBrightnessTexture", "../res/textures/gui/alphaBrightness.png");
		ResourceManager::add<TextureResource>("colorPickerAlphaPatternTexture", "../res/textures/gui/alphaPattern.png");
		ResourceManager::add<TextureResource>("colorPickerBrightnessTexture", "../res/textures/gui/brightness.png");
		ResourceManager::add<TextureResource>("colorPickerCrosshairTexture", "../res/textures/gui/crosshair.png");
		GUI::colorPickerHueTexture = ResourceManager::get<TextureResource>("colorPickerHueTexture");
		GUI::colorPickerAlphaBrightnessTexture = ResourceManager::get<TextureResource>("colorPickerAlphaBrightnessTexture");
		GUI::colorPickerAlphaPatternTexture = ResourceManager::get<TextureResource>("colorPickerAlphaPatternTexture");
		GUI::colorPickerBrightnessTexture = ResourceManager::get<TextureResource>("colorPickerBrightnessTexture");
		GUI::colorPickerCrosshairTexture = ResourceManager::get<TextureResource>("colorPickerCrosshairTexture");

		GUI::colorPicker = new ColorPicker(0, 0);
		GUI::colorPickerFrame = new Frame(0, 0, "Color");
		GUI::colorPickerFrame->add(colorPicker);
		GUI::colorPickerFrame->visible = false;
		GUI::add(colorPickerFrame);
	}
	
	void intersect(const Vec2& mouse) {
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

	bool intersectsSquare(const Vec2& point, const Vec2& topleft, const Vec2& dimension) {
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

	bool onMouseMove(MouseMoveEvent& event) {
		if (GUI::intersectedComponent) {
			Vec2 guiCursorPosition = GUI::map(Vec2(event.getNewX(), event.getNewY()));
			GUI::intersectedComponent->hover(guiCursorPosition);
		}

		return false;
	}

	bool onMouseDrag(MouseDragEvent& event) {
		if (GUI::selectedComponent) {
			Vec2 guiCursorPosition = GUI::map(Vec2(event.getOldX(), event.getOldY()));
			Vec2 newGuiCursorPosition = GUI::map(Vec2(event.getNewX(), event.getNewY()));

			GUI::selectedComponent->drag(newGuiCursorPosition, guiCursorPosition);

			return true;
		}

		return false;
	}

	bool onMouseRelease(MouseReleaseEvent& event) {
		if (Mouse::LEFT == event.getButton()) {
			if (GUI::selectedComponent) {
				GUI::selectedComponent->release(GUI::map(Vec2(event.getX(), event.getY())));
			}
		}
		return false;
	}

	bool onMousePress(MousePressEvent& event) {
		if (Mouse::LEFT == event.getButton()) {
			GUI::selectedComponent = GUI::intersectedComponent;

			if (GUI::intersectedComponent) {
				GUI::select(GUI::superParent(GUI::intersectedComponent));
				GUI::intersectedComponent->press(GUI::map(Vec2(event.getX(), event.getY())));
				GUI::intersectedPoint = GUI::map(Vec2(event.getX(), event.getY())) - GUI::intersectedComponent->position;

				return true;
			}
		}

		return false;
	}

	void updateWindowInfo(const WindowInfo& info) {
		GUI::windowInfo = info;
		
		guiFrameBuffer->resize(info.dimension);
		blurFrameBuffer->resize(info.dimension);
	}

	void onUpdate(Mat4f orthoMatrix) {
		GraphicsShaders::quadShader.updateProjection(orthoMatrix);
	}

	void onEvent(Event& event) {
		EventDispatcher dispatcher(event);
		dispatcher.dispatch<MousePressEvent>(onMousePress);
		dispatcher.dispatch<MouseReleaseEvent>(onMouseRelease);
		dispatcher.dispatch<MouseDragEvent>(onMouseDrag);
	}

	void onRender(Mat4f orthoMatrix) {	
		GUI::blurFrameBuffer->bind();
		GraphicsShaders::quadShader.updateProjection(orthoMatrix);
		GraphicsShaders::quadShader.updateTexture(screenFrameBuffer->texture);
		quad->render();
		GraphicsShaders::blurShader.updateTexture(blurFrameBuffer->texture);
		GraphicsShaders::blurShader.updateType(BlurShader::BlurType::HORIZONTAL);
		quad->render();
		GraphicsShaders::blurShader.updateType(BlurShader::BlurType::VERTICAL);
		quad->render();
		blurFrameBuffer->unbind();

		Path::bind(GUI::batch);
		for (auto iterator = components.rbegin(); iterator != components.rend(); ++iterator) {
			(*iterator)->render();
		}

		Renderer::disableCulling();
		Renderer::enableBlending();
		GraphicsShaders::guiShader.init(orthoMatrix);
		GUI::batch->submit();
	}

	void onClose() {
		// Shaders
		GraphicsShaders::onClose();
		
		// Framebuffers
		guiFrameBuffer->close();
		blurFrameBuffer->close();

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