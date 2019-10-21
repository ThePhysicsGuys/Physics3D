#pragma once

#include <istream>

struct VisualShape;

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