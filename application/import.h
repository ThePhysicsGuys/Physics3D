#pragma once

#include <string>
#include <istream>

#include "../engine/geometry/shape.h"

#include "../engine/math/mat3.h"
#include "../engine/math/vec3.h"

#include "view\camera.h"

#include "extendedPart.h"
#include "worlds.h"

namespace Import {
	int parseInt(std::string num);
	float parseFloat(std::string num);
	double parseDouble(std::string num);
	Vec3 parseVec3(std::string vec);
	DiagonalMat3 parseDiagonalMat3(std::string mat);
	Mat3 parseMat3(std::string mat);

	template<typename T>
	T read(std::istream& input);
};

namespace OBJImport {
	Shape load(std::istream& file, bool binary = false);
	Shape load(std::string file, bool binary = false);
};

namespace WorldImport {
	void load(std::string name, World<ExtendedPart>& world, Camera& camera, bool binary = false);
};