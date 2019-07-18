#pragma once

#include "ray.h"

#include "../screen.h"

struct Pickable {
	virtual void init() {};
	virtual void render(Screen& screen) {};

	virtual float intersect(Screen&, const Ray& ray) = 0;

	virtual void press(Screen& screen) {};
	virtual void release(Screen& screen) {};
	virtual void drag(Screen& screen) {};
};