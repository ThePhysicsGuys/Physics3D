#pragma once

#include "../engine/geometry/shape.h"

extern const Shape icosahedron;
extern const Shape triangleShape;
extern const Shape house;

Shape createCube(double side);
Shape createBox(double width, double height, double length);
