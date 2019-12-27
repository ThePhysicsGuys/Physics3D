#pragma once

#include "../geometry/polyhedron.h"

namespace Library {
	extern const Polyhedron icosahedron;
	extern const Polyhedron trianglePyramid;
	extern const Polyhedron house;
	extern const Polyhedron wedge;

	Polyhedron createCube(float side);
	Polyhedron createBox(float width, float height, float length);

	Polyhedron createXPrism(int sides, float radius, float height);
	Polyhedron createYPrism(int sides, float radius, float height);
	Polyhedron createZPrism(int sides, float radius, float height);

	/*
	Creates a prism with the given number of sides
	result.vertices will contain all the corners, grouped 2 by 2 rotating counterclockwise as seen from the top
	result.triangles will contain all the triangles:
	- sides*2 triangles for the sides
	- (sides - 2) triangles for the top cap
	- (sides - 2) triangles for the bottom cap

	sides must be >= 3
	*/
	inline Polyhedron createPrism(int sides, float radius, float height) {
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
	Polyhedron createPointyPrism(int sides, float radius, float height, float topOffset, float bottomOffset);


	Polyhedron createSphere(float radius, int steps = 1);
	Polyhedron createSpikeBall(float internalRadius, float spikeRadius, int steps, int spikeSteps);
}