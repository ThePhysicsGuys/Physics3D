#pragma once

#include "screen.h"

#include "../../engine/math/mat4.h"
#include "../../engine/math/vec4.h"
#include "../../engine/math/vec3.h"
#include "../../engine/math/vec2.h"

#include <vector>
#include "../../engine/world.h"

Vec2f getNormalizedDeviceSpacePosition(Vec2f viewportSpacePosition, Vec2f screenSize);
Vec3f calcRay(Screen& screen, Vec2f mousePosition);
void updateIntersectedPhysical(Screen& screen, Vec2 mousePosition);
void moveGrabbedPhysicalTransversal(Screen& screen, double dz);
void moveGrabbedPhysicalLateral(Screen& screen);