#pragma once

#include "arrayMesh.h"
#include "../../engine/math/vec2.h"

class Quad {
public:
	ArrayMesh* quad = nullptr;
	
	Quad() {
		double quadVertices[] { -1,  1, -1, -1, 1, -1, -1,  1, 1, -1, 1,  1 };
		double quadUV[] { 0,  1, 0,  0, 1,  0, 0,  1, 1,  0, 1,  1 };

		quad = new ArrayMesh(quadVertices, quadUV, 6, 2);
	}

	void render() {
		quad->render();
	}
};