#pragma once

#include "shape.h"

class Polyhedron;

Shape sphereShape(double radius);
Shape cylinderShape(double radius, double height);
Shape boxShape(double width, double height, double depth);
Shape polyhedronShape(const Polyhedron& poly);
Shape wedgeShape();
