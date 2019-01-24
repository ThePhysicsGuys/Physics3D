#pragma once

#include "../engine/geometry/shape.h"

extern const NormalizedShape icosahedron;
extern const Shape triangleShape;
extern const Shape house;

NormalizedShape createCube(double side);
NormalizedShape createBox(double width, double height, double length);
