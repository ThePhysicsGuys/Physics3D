#pragma once

#include "ray.h"

class Screen;

struct Pickable {
	virtual void onInit() {};
	virtual void onRender(Screen& screen) {};
	virtual void onClose() {};

	virtual float intersect(Screen&, const Ray& ray) = 0;

	virtual void onMousePress(Screen& screen) {};
	virtual void onMouseRelease(Screen& screen) {};
	virtual void onMouseDrag(Screen& screen) {};
};