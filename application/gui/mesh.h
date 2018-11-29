#pragma once

#include "abstractMesh.h"

class Mesh : public AbstractMesh {
public:
	unsigned int vbo;

	Mesh(double* positions, int size);

	void render() override;
	void close() override;
};

