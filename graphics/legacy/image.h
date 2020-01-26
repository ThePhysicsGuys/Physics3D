#pragma once

#include "component.h"

class Texture;

class Image : public Component {
private:
	bool fixedHeight;
	bool fixedWidth;
public:
	Texture* texture;

	Image(double x, double y, double width, double height);
	Image(double x, double y, Texture* texture);
	Image(double x, double y, double width, double height, Texture* texture);
	
	Image* fixHeight(float height);
	Image* fixWidth(float width);

	Vec2 resize() override;
	void render() override;
};