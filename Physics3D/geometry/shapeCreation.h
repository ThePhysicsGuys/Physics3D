#pragma once

#include "shape.h"

namespace P3D {
class Polyhedron;

Shape boxShape(double width, double height, double depth);
Shape wedgeShape(double width, double height, double depth);
Shape cornerShape(double width, double height, double depth);
Shape sphereShape(double radius);
Shape cylinderShape(double radius, double height);
Shape polyhedronShape(const Polyhedron& poly);
}