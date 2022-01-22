#pragma once

#include <istream>

namespace P3D::Graphics {
struct ExtendedTriangleMesh;
};

namespace P3D {

namespace Import {
	int parseInt(const std::string& num);
	long long parseLong(const std::string& num);
	Fix<32> parseFix(const std::string& num);
	float parseFloat(const std::string& num);
	double parseDouble(const std::string& num);
	Vec3 parseVec3(const std::string& vec);
	Vec3f parseVec3f(const std::string& vec);
	Vec4 parseVec4(const std::string& vec);
	Vec4f parseVec4f(const std::string& vec);
	Position parsePosition(const std::string& pos);
	DiagonalMat3 parseDiagonalMat3(const std::string& mat);
	Mat3 parseMat3(const std::string& mat);

	template<typename T>
	T read(std::istream& input);
};

namespace OBJImport {
	Graphics::ExtendedTriangleMesh load(std::istream& file, bool binary = false);
	Graphics::ExtendedTriangleMesh load(const std::string& file, bool binary);
	Graphics::ExtendedTriangleMesh load(const std::string& file);
};

};