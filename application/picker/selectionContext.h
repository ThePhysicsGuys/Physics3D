#pragma once

#include "selection.h"
#include "../physics/math/ray.h"
#include "../physics/math/linalg/vec.h"

namespace P3D::Application {

struct SelectionContext {
	Ray ray;
	Vec2 mouse;
	Selection selection;
};
	
}
