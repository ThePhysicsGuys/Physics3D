#pragma once

#include "screen.h"

#include "../../engine/math/mat4.h"
#include "../../engine/math/vec4.h"
#include "../../engine/math/vec3.h"
#include "../../engine/math/vec2.h"

#include <vector>
#include "../../engine/world.h"

Vec2 getNormalizedDeviceSpacePosition(Vec2 viewportSpacePosition, Vec2 screenSize);
Vec3 calcRay(Vec2 mousePosition, Vec2 screenSize, Mat4f viewMatrix, Mat4f projectionMatrix);
void updateIntersectedPhysical(Screen& screen, Vec2 mousePosition, Mat4f rotatedViewMatrix, Mat4f projectionMatrix);
void moveGrabbedPhysicalTransversal(Screen& screen, double dz);
void moveGrabbedPhysicalLateral(Screen& screen);