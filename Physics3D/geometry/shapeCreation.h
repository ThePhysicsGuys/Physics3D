#pragma once

#include "shape.h"

namespace P3D {
class Polyhedron;

Shape sphereShape(double radius);
Shape cylinderShape(double radius, double height);
Shape boxShape(double width, double height, double depth);
Shape polyhedronShape(const Polyhedron& poly);
};
