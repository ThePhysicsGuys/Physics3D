
#include "component.h"
#include "container.h"
#include "button.h"
#include "label.h"

#include "../texture.h"
#include "../engine/math/vec2.h"
#include "../engine/math/vec4.h"

#include <vector>

class Frame : public Container {
public:
	Button* closeButton;
	Button* minimizeButton;
	double buttonOffset;

	Label* title;

	double titleBarHeight;
	Vec4 titleBarColor;

	Vec4 backgroundColor;

	bool minimized;

	Frame();
	Frame(double x, double y);
	Frame(double x, double y, double width, double height);

	Vec2 resize() override;
	void render() override;

	Component* intersect(Vec2 point) override;

	void hover(Vec2 mouse) override;
	void drag(Vec2 mouse) override;
	void enter() override;
	void exit() override;
};