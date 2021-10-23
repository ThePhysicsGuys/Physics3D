#pragma once

#include "../geometry/polyhedron.h"

namespace Library {
extern const Polyhedron icosahedron;
extern const Polyhedron trianglePyramid;
extern const Polyhedron house;
extern const Polyhedron wedge;
extern const Polyhedron corner;
extern Vec3f wedgeVertices[6];
extern Vec3f cornerVertices[4];
extern int wedgeVertexCount;
extern int cornerVertexCount;

Polyhedron createCube(float side);
Polyhedron createBox(float width, float height, float depth);

/*
Creates a prism oriented along the Z axis with the given number of sides
result.vertices will contain all the corners, grouped 2 by 2 rotating counterclockwise as seen from the top
result.triangles will contain all the triangles:
- sides*2 triangles for the sides
- (sides - 2) triangles for the top cap
- (sides - 2) triangles for the bottom cap

sides must be >= 3
*/
Polyhedron createPrism(int sides, float radius, float height);


/*
Creates a pointed prism oriented along the Y axis with the given number of sides
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

/*
	Creates a torus around the z axis. The tube will have a diameter of 2xradiusOfTube
	ringRadius gives the radius of the centerline of the ring
	radialFidelity is the number of segments
	tubeFidelity is the number of vertices that make up each segment
*/
Polyhedron createTorus(float ringRadius, float radiusOfTube, int radialFidelity, int tubeFidelity);

/*
	Creates a swept shape around the z axis, where it meets in two points on the axis. 

	startZ denotes the starting z-coordinate, a triangle fan will be made starting from this index
	endZ denotes the ending z-coordinate

	inbetweenPoints is a list of length inbetweenPointsCount, which contains the 2D outline to be swept around. 
	The x-value denotes the z-position alogn the axis, the y value is the radius at that point

	sweepFidelity denotes the number of steps of rotation. 

	The triangles are structured as follows:
	[0 .. sweepFidelity-1] is the triangleFan for startZ
	[sweepFidelity .. sweepFidelity + 2*(inbetweenPointCount-1)*sweepFidelity-1] are the rings comprising the inbetweenPoints. Each of these rings is 2*sweepFidelity triangles
	[sweepFidelity + 2*(inbetweenPointCount-1)*sweepFidelity .. 2*inbetweenPointCount*sweepFidelity - 1] is the triangleFan for endZ
*/
Polyhedron createRevolvedShape(float startZ, Vec2f* inbetweenPoints, int inbetweenPointCount, float endZ, int sweepFidelity);
};
