#pragma once

#include "../geometry/polyhedron.h"

namespace Library {
	extern const Polyhedron icosahedron;
	extern const Polyhedron trianglePyramid;
	extern const Polyhedron house;
	extern const Polyhedron wedge;

	Polyhedron createCube(double side);
	Polyhedron createBox(double width, double height, double length);

	Polyhedron createXPrism(int sides, double radius, double height);
	Polyhedron createYPrism(int sides, double radius, double height);
	Polyhedron createZPrism(int sides, double radius, double height);

	/*
	Creates a prism with the given number of sides
	result.vertices will contain all the corners, grouped 2 by 2 rotating counterclockwise as seen from the top
	result.triangles will contain all the triangles:
	- sides*2 triangles for the sides
	- (sides - 2) triangles for the top cap
	- (sides - 2) triangles for the bottom cap

	sides must be >= 3
	*/
	inline Polyhedron createPrism(int sides, double radius, double height) {
		return createYPrism(sides, radius, height);
	}
	
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
	Polyhedron createPointyPrism(int sides, double radius, double height, double topOffset, double bottomOffset);


	Polyhedron createSphere(double radius, int steps = 1);
	Polyhedron createSpikeBall(double internalRadius, double spikeRadius, int steps, int spikeSteps);
}