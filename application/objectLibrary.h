#pragma once

#include "visualShape.h"

extern const Shape icosahedron;
extern const Shape triangleShape;
extern const Shape house;

Shape createCube(double side);
Shape createBox(double width, double height, double length);

/*
Creates a prism with the given number of sides
result.vertices will contain all the corners, grouped 2 by 2 rotating counterclockwise as seen from the top
result.triangles will contain all the triangles:
- sides*2 triangles for the sides
- (sides - 2) triangles for the top cap
- (sides - 2) triangles for the bottom cap

sides must be >= 3
*/
Shape createPrism(unsigned int sides, double radius, double height);

/*
Creates a pointed prism with the given number of sides
result.vertices will contain all the corners:
- vertices grouped 2 by 2 rotating counterclockwise as seen from the top
- top vertex
- bottom vertex

result.triangles will contain all the triangles:
- sides*2 triangles for the sides
- sides triangles for the top point
- sides triangles for the bottom point

sides must be >= 3
*/
Shape createPointyPrism(unsigned int sides, double radius, double height, double topOffset, double bottomOffset);
