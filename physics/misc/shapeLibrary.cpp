#include "shapeLibrary.h"

#include <utility>
#include <map>

#include "../datastructures/buffers.h"

#include "../math/linalg/trigonometry.h"
#include "../math/constants.h"

namespace Library {

	Vec3f icosahedronVertices[]{
		Vec3f(0, 0.5, G / 2), Vec3f(0, 0.5, -G / 2), Vec3f(0, -0.5, -G / 2), Vec3f(0, -0.5, G / 2),
		Vec3f(0.5, G / 2, 0), Vec3f(0.5, -G / 2, 0), Vec3f(-0.5, -G / 2, 0), Vec3f(-0.5, G / 2, 0),
		Vec3f(G / 2, 0, 0.5), Vec3f(-G / 2, 0, 0.5), Vec3f(-G / 2, 0, -0.5), Vec3f(G / 2, 0, -0.5),
	};

	Triangle icosahedronTriangles[]{
		{0 , 3 , 8}, {0 , 8, 4 }, {0, 4 , 7}, {0, 7 , 9}, {0, 9, 3 },
		{2 , 10, 1}, {2 , 1, 11}, {2, 11, 5}, {2, 5 , 6}, {2, 6, 10},
		{8 , 3 , 5}, {8 , 5, 11}, {8, 11, 4}, {4, 11, 1}, {4, 1, 7 },
		{10, 7 , 1}, {10, 9, 7 }, {9, 10, 6}, {9, 6 , 3}, {3, 6, 5 },
	};

	Vec3f trianglePyramidVertices[]{
		Vec3f(0.0, 0.0, 0.0),
		Vec3f(1.0, 0.0, 0.0),
		Vec3f(0.0, 1.0, 0.0),
		Vec3f(0.0, 0.0, 1.0)
	};

	Triangle trianglePyramidTriangles[]{
		{0, 2, 1}, {0, 3, 2}, {0, 1, 3}, {1, 2, 3}
	};

	Vec3f houseVertices[]{
		Vec3f(-0.5, 0.0, -0.5), Vec3f(-0.5, 0.0, 0.5), Vec3f(0.5, 0.0, 0.5), Vec3f(0.5, 0.0, -0.5),
		Vec3f(-0.5, 1.0, -0.5), Vec3f(-0.5, 1.0, 0.5), Vec3f(0.5, 1.0, 0.5), Vec3f(0.5, 1.0, -0.5),
		Vec3f(0.0, 1.5, -0.5), Vec3f(0.0, 1.5, 0.5)
	};

	Triangle houseTriangles[]{
		{0, 1, 4}, {5, 4, 1}, // left
		{0, 2, 1}, {0, 3, 2}, // bottom
		{0, 4, 3}, {3, 4, 7}, // front
		{3, 7, 2}, {2, 7, 6}, // right
		{1, 2, 5}, {5, 2, 6}, // back
		{7, 4, 8}, {5, 6, 9}, // roof
		{4, 5, 9}, {4, 9, 8}, {7, 8, 9}, {7, 9, 6} //roof2
	};

	Vec3f wedgeVertices[]{
		Vec3f(-0.5, -0.5, -0.5), Vec3f(-0.5, -0.5, 0.5),Vec3f(0.5, -0.5, 0.5),Vec3f(0.5, -0.5, -0.5),
		Vec3f(-0.5, 0.5, -0.5), Vec3f(-0.5, 0.5, 0.5)
	};

	Triangle wedgeTriangles[]{
		{0, 2, 1}, {0, 3, 2}, // bottom
		{0, 5, 4}, {0, 1, 5}, // back
		{0, 4, 3}, // left side 
		{1, 2, 5}, // right side
		{2, 3, 4}, {2, 4, 5} // diagonalSide 
	};

	Triangle boxTriangles[12] = {
		{1,0,2},{3,2,0}, // BOTTOM
		{1,5,0},{4,0,5}, // FRONT
		{1,2,5},{6,5,2}, // RIGHT
		{6,2,7},{3,7,2}, // BACK
		{3,0,7},{4,7,0}, // LEFT
		{4,5,7},{6,7,5}, // TOP
	};

	const Polyhedron icosahedron(icosahedronVertices, icosahedronTriangles, 12, 20);
	const Polyhedron trianglePyramid(trianglePyramidVertices, trianglePyramidTriangles, 4, 4);
	const Polyhedron house(houseVertices, houseTriangles, 10, 16);
	const Polyhedron wedge(wedgeVertices, wedgeTriangles, 6, 8);

	Polyhedron createCube(float side) {
		return createBox(side, side, side);
	}

	Polyhedron createBox(float width, float height, float depth) {
		float dx = float(width / 2.0);
		float dy = float(height / 2.0);
		float dz = float(depth / 2.0);

		Vec3f vertBuf[8]{
			Vec3f(-dx, -dy, -dz),
			Vec3f(dx, -dy, -dz),
			Vec3f(dx, dy, -dz),
			Vec3f(-dx, dy, -dz),
			Vec3f(-dx, -dy, dz),
			Vec3f(dx, -dy, dz),
			Vec3f(dx, dy, dz),
			Vec3f(-dx, dy, dz)
		};
		return Polyhedron(vertBuf, boxTriangles, 8, 12);
	}


	static void createTriangleRing(int ring1Start, int ring2Start, int size, EditableMesh& mesh, int offset) {
		for(int i = 0; i < size - 1; i++) {
			mesh.setTriangle(offset + i * 2, ring1Start + i, ring1Start + i + 1, ring2Start + i);
			mesh.setTriangle(offset + i * 2 + 1, ring1Start + i + 1, ring2Start + i + 1, ring2Start + i);
		}
		int last = size - 1;
		mesh.setTriangle(offset + last * 2, ring1Start + last, ring1Start, ring2Start + last);
		mesh.setTriangle(offset + last * 2 + 1, ring1Start, ring2Start, ring2Start + last);
	}

	static void createTriangleRingReverse(int ring1Start, int ring2Start, int size, EditableMesh& mesh, int offset) {
		for(int i = 0; i < size - 1; i++) {
			mesh.setTriangle(offset + i * 2, ring1Start + i + 1, ring1Start + i, ring2Start + i);
			mesh.setTriangle(offset + i * 2 + 1, ring1Start + i + 1, ring2Start + i, ring2Start + i + 1);
		}
		int last = size - 1;
		mesh.setTriangle(offset + last * 2, ring1Start, ring1Start + last, ring2Start + last);
		mesh.setTriangle(offset + last * 2 + 1, ring1Start, ring2Start + last, ring2Start);
	}

	static void createTriangleFan(int topIndex, int startFan, int size, EditableMesh& mesh, int offset) {
		for(int i = 0; i < size-1; i++) {
			mesh.setTriangle(offset + i, topIndex, startFan + i, startFan + i + 1);
		}
	}
	static void createTriangleFanReverse(int topIndex, int startFan, int size, EditableMesh& mesh, int offset) {
		for(int i = 0; i < size - 1; i++) {
			mesh.setTriangle(offset + i, topIndex, startFan + i + 1, startFan + i);
		}
	}

	static void createTriangleCone(int topIndex, int startFan, int size, EditableMesh& mesh, int offset) {
		createTriangleFan(topIndex, startFan, size, mesh, offset);
		mesh.setTriangle(offset + size - 1, topIndex, startFan + size - 1, startFan);
	}

	static void createTriangleConeReverse(int topIndex, int startFan, int size, EditableMesh& mesh, int offset) {
		createTriangleFanReverse(topIndex, startFan, size, mesh, offset);
		mesh.setTriangle(offset + size - 1, topIndex, startFan, startFan + size - 1);
	}

	static void createFlatSurface(int startFan, int size, EditableMesh& mesh, int offset) {
		createTriangleFan(startFan, startFan + 1, size - 1, mesh, offset);
	}

	static void createFlatSurfaceReverse(int startFan, int size, EditableMesh& mesh, int offset) {
		createTriangleFanReverse(startFan, startFan + 1, size - 1, mesh, offset);
	}

	Polyhedron createPrism(int sides, float radius, float height) {
		EditableMesh result(sides * 2, (sides - 1) * 4);

		// vertices
		for(int i = 0; i < sides; i++) {
			float angle = i * PI * 2 / sides;
			float h = height / 2;
			float cr = cos(angle) * radius;
			float sr = sin(angle) * radius;
			result.setVertex(i * 2, cr, sr, h);
			result.setVertex(i * 2 + 1, cr, sr, -h);
		}

		// triangles
		// sides
		for(int i = 0; i < sides; i++) {
			int botLeft = i * 2;
			int botRight = ((i + 1) % sides) * 2;
			result.setTriangle(i * 2, botLeft, botLeft + 1, botRight); // botLeft, botRight, topLeft
			result.setTriangle(i * 2 + 1, botRight + 1, botRight, botLeft + 1); // topRight, topLeft, botRight
		}

		// top and bottom
		for(int i = 0; i < sides - 2; i++) { // common corner is i=0
			result.setTriangle(i + sides * 2, 0, (i + 1) * 2, (i + 2) * 2);
			result.setTriangle(i + (sides - 2) + sides * 2, 1, (i + 2) * 2 + 1, (i + 1) * 2 + 1);
		}
		return Polyhedron(std::move(result));
	}

	Polyhedron createPointyPrism(int sides, float radius, float height, float topOffset, float bottomOffset) {
		EditableMesh result(sides * 2 + 2, sides * 4);

		// vertices
		for (int i = 0; i < sides; i++) {
			float angle = i * PI * 2 / sides;
			result.setVertex(i * 2, cos(angle) * radius, -height / 2, sin(angle) * radius);
			result.setVertex(i * 2 + 1, cos(angle) * radius, height / 2, sin(angle) * radius);
		}

		int bottomIndex = sides * 2;
		int topIndex = sides * 2 + 1;

		result.setVertex(bottomIndex, 0, -height / 2 - bottomOffset, 0);
		result.setVertex(topIndex, 0, height / 2 + topOffset, 0);

		// triangles
		// sides
		for(int i = 0; i < sides; i++) {
			int botLeft = i * 2;
			int botRight = ((i + 1) % sides) * 2;
			result.setTriangle(i * 2, botLeft, botLeft + 1, botRight); // botLeft, botRight, topLeft
			result.setTriangle(i * 2 + 1, botRight + 1, botRight, botLeft + 1); // topRight, topLeft, botRight
		}

		// top and bottom
		for(int i = 0; i < sides; i++) { // common corner is i=0
			result.setTriangle(i + sides * 2, bottomIndex, i * 2, ((i + 1) % sides) * 2);
			result.setTriangle(i + sides + sides * 2, topIndex, ((i + 1) % sides) * 2 + 1, i * 2 + 1);
		}

		return Polyhedron(std::move(result));
	}

	// divides every triangle into 4 smaller triangles
	static std::pair<Vec3f*, Triangle*> tesselate(Vec3f* vecBuf, Triangle* triangleBuf, int vertexCount, int triangleCount) {
		assert(triangleCount % 2 == 0);
		int newVecBufSize = vertexCount + triangleCount * 3 / 2;
		int newTriIndex = triangleCount*4;

		int curMapIndex = vertexCount;

		std::map<std::pair<int, int>, int> newPoints;



		for(int i = triangleCount; i-- > 0;) {
			// first collect the 6 new points

			Triangle& curT = triangleBuf[i];

			Triangle newCenterTriangle;

			for(int j = 0; j < 3; j++) {
				int a = curT[j];
				int b = curT[(j + 1) % 3];

				if(b > a) { std::swap(a, b); }

				std::pair<int, int> edge(a, b);

				auto result = newPoints.find(edge);

				int index;

				if(result == newPoints.end()) {
					index = curMapIndex++;
					newPoints[edge] = index;
					vecBuf[index] = (vecBuf[a] + vecBuf[b]) * 0.5f;
				} else {
					index = newPoints[edge];
				}

				newCenterTriangle[j] = index;
			}

			triangleBuf[--newTriIndex] = Triangle{newCenterTriangle[1], newCenterTriangle[0], curT[1]};
			triangleBuf[--newTriIndex] = Triangle{newCenterTriangle[2], newCenterTriangle[1], curT[2]};
			triangleBuf[--newTriIndex] = Triangle{newCenterTriangle[0], newCenterTriangle[2], curT[0]};
			triangleBuf[--newTriIndex] = newCenterTriangle;
		}
		assert(curMapIndex == newVecBufSize);
		return std::pair<Vec3f*, Triangle*>(vecBuf, triangleBuf);
	}

	Polyhedron createSphere(float radius, int steps) {

		int vertices = 12;
		int triangles = 20;

		for(int i = 0; i < steps; i++) {
			vertices = vertices + triangles * 3 / 2;
			triangles = triangles * 4;
		}

		Polyhedron curSphere = icosahedron;
		Vec3f* vecBuf = new Vec3f[vertices];
		Triangle* triBuf = new Triangle[triangles];

		curSphere.getVertices(vecBuf);
		curSphere.getTriangles(triBuf);

		vertices = 12;
		triangles = 20;
		for(int i = 0; i < steps; i++) {
			tesselate(vecBuf, triBuf, vertices, triangles);
			vertices = vertices + triangles * 3 / 2;
			triangles = triangles * 4;
		}

		// size 42 x 80
		// size 162 x 320
		// size 642 x 1280

		for(int i = 0; i < vertices; i++) {
			vecBuf[i] = normalize(vecBuf[i]) * radius;
		}

		Polyhedron poly(vecBuf, triBuf, vertices, triangles);



		//Polyhedron poly(vecBuf, triBuf, 12, 20);

		delete[] vecBuf;
		delete[] triBuf;

		return poly;
	}

	Polyhedron createSpikeBall(float internalRadius, float spikeRadius, int steps, int spikeSteps) {

		int vertices = 12;
		int triangles = 20;

		for(int i = 0; i < steps; i++) {
			vertices = vertices + triangles * 3 / 2;
			triangles = triangles * 4;
		}

		Polyhedron curSphere = icosahedron;
		Vec3f* vecBuf = new Vec3f[vertices];
		Triangle* triBuf = new Triangle[triangles];

		curSphere.getVertices(vecBuf);
		curSphere.getTriangles(triBuf);

		vertices = 12;
		triangles = 20;
		for(int i = 0; i < steps; i++) {
			tesselate(vecBuf, triBuf, vertices, triangles);
			vertices = vertices + triangles * 3 / 2;
			triangles = triangles * 4;
		}

		// size 42 x 80
		// size 162 x 320
		// size 642 x 1280

		int spikeVerts = 12;
		int spikeTris = 20;
		for(int i = 0; i < spikeSteps; i++) {
			spikeVerts = spikeVerts + spikeTris * 3 / 2;
			spikeTris = spikeTris * 4;
		}

		for(int i = 0; i < spikeVerts; i++) {
			vecBuf[i] = normalize(vecBuf[i]) * spikeRadius;
		}
		for(int i = spikeVerts; i < vertices; i++) {
			vecBuf[i] = normalize(vecBuf[i]) * internalRadius;
		}

		Polyhedron poly(vecBuf, triBuf, vertices, triangles);



		//Polyhedron poly(vecBuf, triBuf, 12, 20);

		delete[] vecBuf;
		delete[] triBuf;

		return poly;
	}

	Polyhedron createTorus(float ringRadius, float radiusOfTube, int radialFidelity, int tubeFidelity) {
		EditableMesh result(radialFidelity * tubeFidelity, 2 * radialFidelity * tubeFidelity);

		for(int segment = 0; segment < radialFidelity; segment++) {
			float angle = (2 * PI * segment) / radialFidelity;
			float s = sin(angle);
			float c = cos(angle);
			
			for(int partInSegment = 0; partInSegment < tubeFidelity; partInSegment++) {
				float tubeAngle = (2 * PI * partInSegment) / tubeFidelity;
				
				float height = radiusOfTube * sin(tubeAngle);
				float radius = ringRadius + radiusOfTube * cos(tubeAngle);

				result.setVertex(segment * tubeFidelity + partInSegment, Vec3f(s * radius, c * radius, height));
			}
		}

		// triangles
		for(int segment = 0; segment < radialFidelity - 1; segment++) {
			createTriangleRing(segment * tubeFidelity, (segment + 1) * tubeFidelity, tubeFidelity, result, 2 * segment * tubeFidelity);
		}
		createTriangleRing((radialFidelity - 1) * tubeFidelity, 0, tubeFidelity, result, 2 * (radialFidelity - 1) * tubeFidelity);

		return Polyhedron(std::move(result));
	}

	Polyhedron createRevolvedShape(float startZ, Vec2f* inbetweenPoints, int inbetweenPointCount, float endZ, int segmentCount) {
		EditableMesh result(segmentCount * inbetweenPointCount + 2, segmentCount * 2 * (inbetweenPointCount));
		
		result.setVertex(0, 0.0f, 0.0f, startZ);
		int lastVertex = segmentCount * inbetweenPointCount + 1;
		result.setVertex(lastVertex, 0.0f, 0.0f, endZ);
		for(int segmentI = 0; segmentI < segmentCount; segmentI++) {
			float angle = (2 * PI * segmentI) / segmentCount;
			float s = sin(angle);
			float c = cos(angle);

			for(int inbetweenI = 0; inbetweenI < inbetweenPointCount; inbetweenI++) {
				float zValue = inbetweenPoints[inbetweenI].x;
				float radius = inbetweenPoints[inbetweenI].y;
				result.setVertex(inbetweenI * segmentCount + segmentI + 1, -s * radius, c * radius, zValue);
			}
		}

		// triangles
		createTriangleConeReverse(0, 1, segmentCount, result, 0);
		for(int inbetweenI = 0; inbetweenI < inbetweenPointCount - 1; inbetweenI++) {
			createTriangleRing(1 + segmentCount * inbetweenI, 1 + segmentCount * (inbetweenI + 1), segmentCount, result, segmentCount + 2 * segmentCount * inbetweenI);
		}
		createTriangleCone(lastVertex, 1 + segmentCount * (inbetweenPointCount-1), segmentCount, result, segmentCount + 2 * segmentCount * (inbetweenPointCount-1));

		return Polyhedron(std::move(result));
	}
}
