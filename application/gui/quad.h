#pragma once

#include "arrayMesh.h"
#include "../../engine/math/vec2.h"

class Quad {
public:

	Vec3 position;
	Vec3 rotation;
	ArrayMesh* quad = nullptr;
	
	Quad() {
		const double vertices[8] = { -1, 1, -1, -1, 1, 1, 1, -1 };
		quad = new ArrayMesh(vertices, 8, 3);
	}

	void render() {
		quad->render();
	}
};