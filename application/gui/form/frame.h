
#include "component.h"
#include "container.h"
#include "../texture.h"
#include "../engine/math/vec2.h"
#include "../engine/math/vec4.h"

#include <vector>

class Frame : public Container {
public:
	double titleBarHeight;
	Vec4 titleBarColor;
	Vec4 backgroundColor;
	Texture* closeTexture;
	double closeButtonOffset;

	Frame(double x, double y);
	Frame(double x, double y, double width, double height);

	Vec2 resize() override;
	void render() override;

	Component* intersect(Vec2 point) override;
	void hover(Vec2 mouse) override;
};