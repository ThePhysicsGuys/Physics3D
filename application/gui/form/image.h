#pragma once

#include "../texture.h"
#include "component.h"

#include "../engine/math/vec2.h"

class Image : public Component {
public:
	Texture* texture;

	Image(double x, double y, Texture* texture);
	Image(double x, double y, double width, double height, Texture* texture);
	
	Vec2 resize() override;
	void render() override;
};