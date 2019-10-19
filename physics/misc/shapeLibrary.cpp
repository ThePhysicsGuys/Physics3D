#include "shapeLibrary.h"

#include "../math/mathUtil.h"

#include <utility>
#include <map>

#include "../datastructures/buffers.h"

#include "../math/linalg/trigonometry.h"

namespace Library {
	float g = 1.61803398874989484820458683436563811772030917980576286213544862270526046281890f;

	Vec3f icosahedronVertices[]{
		Vec3f(0, 0.5, g / 2), Vec3f(0, 0.5, -g / 2), Vec3f(0, -0.5, -g / 2), Vec3f(0, -0.5, g / 2),
		Vec3f(0.5, g / 2, 0), Vec3f(0.5, -g / 2, 0), Vec3f(-0.5, -g / 2, 0), Vec3f(-0.5, g / 2, 0),
		Vec3f(g / 2, 0, 0.5), Vec3f(-g / 2, 0, 0.5), Vec3f(-g / 2, 0, -0.5), Vec3f(g / 2, 0, -0.5),
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

	const Polyhedron icosahedron(icosahedronVertices, icosahedronTriangles, 12, 20);
	const Polyhedron trianglePyramid(trianglePyramidVertices, trianglePyramidTriangles, 4, 4);
	const Polyhedron house(houseVertices, houseTriangles, 10, 16);
	const Polyhedron wedge(wedgeVertices, wedgeTriangles, 6, 8);

	Polyhedron createCube(double side) {
		return createBox(side, side, side);
	}

	Polyhedron createBox(double width, double height, double length) {
		return BoundingBox(width, height, length).toShape();
	}

	Polyhedron createPrism(int sides, double radius, double height) {
		int vertexCount = sides * 2;
		int triangleCount = sides * 2 + (sides - 2) * 2;
		Vec3f* vecBuf = new Vec3f[vertexCount];
		Triangle* triangleBuf = new Triangle[triangleCount];

		// vertices
		for (int i = 0; i < sides; i++) {
			double angle = i * PI * 2 / sides;
			vecBuf[i*2] = Vec3f(cos(angle) * radius, -height / 2, sin(angle) * radius);
			vecBuf[i*2+1] = Vec3f(cos(angle) * radius, height / 2, sin(angle) * radius);
		}

		// sides
		for (int i = 0; i < sides; i++) {
			int botLeft = i * 2;
			int botRight = ((i+1)%sides) * 2;
			triangleBuf[i*2] = Triangle{ botLeft, botLeft + 1, botRight }; // botLeft, botRight, topLeft
			triangleBuf[i*2 + 1] = Triangle{ botRight + 1, botRight, botLeft + 1 }; // topRight, topLeft, botRight
		}

		Triangle* capOffset = triangleBuf + sides * 2;
		// top and bottom
		for (int i = 0; i < sides - 2; i++) { // common corner is i=0
			capOffset[i] = Triangle{ 0, (i + 1) * 2, (i + 2) * 2 };
			capOffset[i + (sides-2)] = Triangle{ 1, (i + 2) * 2+1, (i + 1) * 2+1 };
		}

		return Polyhedron(vecBuf, triangleBuf, vertexCount, triangleCount);
	}

	Polyhedron createPointyPrism(int sides, double radius, double height, double topOffset, double bottomOffset) {
		int vertexCount = sides * 2 + 2;
		int triangleCount = sides * 4;
		Vec3f* vecBuf = new Vec3f[vertexCount];
		Triangle* triangleBuf = new Triangle[triangleCount];

		// vertices
		for (int i = 0; i < sides; i++) {
			double angle = i * PI * 2 / sides;
			vecBuf[i * 2] = Vec3f(cos(angle) * radius, -height / 2, sin(angle) * radius);
			vecBuf[i * 2 + 1] = Vec3f(cos(angle) * radius, height / 2, sin(angle) * radius);
		}

		int bottomIndex = sides * 2;
		int topIndex = sides * 2 + 1;

		vecBuf[bottomIndex] = Vec3f(0, -height / 2 - bottomOffset, 0);
		vecBuf[topIndex] = Vec3f(0, height / 2 + topOffset, 0);


		// sides
		for (int i = 0; i < sides; i++) {
			int botLeft = i * 2;
			int botRight = ((i + 1) % sides) * 2;
			triangleBuf[i * 2] = Triangle{ botLeft, botLeft + 1, botRight }; // botLeft, botRight, topLeft
			triangleBuf[i * 2 + 1] = Triangle{ botRight + 1, botRight, botLeft + 1 }; // topRight, topLeft, botRight
		}

		Triangle* capOffset = triangleBuf + sides * 2;
		// top and bottom
		for (int i = 0; i < sides; i++) { // common corner is i=0
			capOffset[i] = Triangle{ bottomIndex, i * 2, ((i+1) % sides) * 2 };
			capOffset[i + sides] = Triangle{ topIndex, ((i + 1) % sides) * 2 + 1, i * 2 + 1 };
		}

		return Polyhedron(vecBuf, triangleBuf, vertexCount, triangleCount);
	}

	// divides every triangle into 4 smaller triangles
	static std::pair<Vec3f*, Triangle*> tesselate(Vec3f* vecBuf, Triangle* triangleBuf, size_t vertexCount, size_t triangleCount) {
		assert(triangleCount % 2 == 0);
		size_t newVecBufSize = vertexCount + triangleCount * 3 / 2;
		size_t newTriIndex = triangleCount*4;

		int curMapIndex = vertexCount;

		std::map<std::pair<int, int>, int> newPoints;



		for(size_t i = triangleCount; i-- > 0;) {
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
					vecBuf[index] = (vecBuf[a] + vecBuf[b]) / 2;
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




	Polyhedron createSphere(double radius, int steps) {

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

		for(size_t i = 0; i < vertices; i++) {
			vecBuf[i] = normalize(vecBuf[i]) * radius;
		}

		Polyhedron poly(vecBuf, triBuf, vertices, triangles);



		//Polyhedron poly(vecBuf, triBuf, 12, 20);

		delete[] vecBuf;
		delete[] triBuf;

		return poly;
	}

	Polyhedron createSpikeBall(double internalRadius, double spikeRadius, int steps, int spikeSteps) {

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

		for(size_t i = 0; i < spikeVerts; i++) {
			vecBuf[i] = normalize(vecBuf[i]) * spikeRadius;
		}
		for(size_t i = spikeVerts; i < vertices; i++) {
			vecBuf[i] = normalize(vecBuf[i]) * internalRadius;
		}

		Polyhedron poly(vecBuf, triBuf, vertices, triangles);



		//Polyhedron poly(vecBuf, triBuf, 12, 20);

		delete[] vecBuf;
		delete[] triBuf;

		return poly;
	}
}
