#pragma once

#include <string>
#include <istream>

#include "../engine/geometry/shape.h"

#include "../engine/math/mat3.h"
#include "../engine/math/vec3.h"
#include "../engine/math/vec4.h"

#include "../view/screen.h"

#include "../extendedPart.h"
#include "../worlds.h"

namespace Import {
	int parseInt(std::string num);
	long long parseLong(std::string num);
	float parseFloat(std::string num);
	double parseDouble(std::string num);
	Vec3 parseVec3(std::string vec);
	Vec3f parseVec3f(std::string vec);
	Vec4 parseVec4(std::string vec);
	Vec4f parseVec4f(std::string vec);
	Position parsePosition(std::string pos);
	DiagonalMat3 parseDiagonalMat3(std::string mat);
	Mat3 parseMat3(std::string mat);

	template<typename T>
	T read(std::istream& input);
};

namespace OBJImport {
	VisualShape load(std::istream& file, bool binary = false);
	VisualShape load(std::string file, bool binary = false);
};

namespace WorldImport {
	void load(std::string name, World<ExtendedPart>& world, Screen& screen, bool binary = false);
};