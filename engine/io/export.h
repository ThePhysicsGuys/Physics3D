#pragma once

namespace Graphics {
struct VisualShape;
};

namespace P3D {

namespace Export {
	template<typename T>
	void write(std::ostream& output, T const * buffer, int size);

	template<typename T>
	void write(std::ostream& output, const T& value);

	std::string str(Vec4 vector);
	std::string str(Vec3 vector);
	std::string str(Position pos);
	std::string str(Mat3 matrix);
	std::string str(DiagonalMat3 matrix);
	std::string str(int num);
	std::string str(double num);
};

namespace OBJExport {
	void save(const std::string& file, const Graphics::VisualShape&, bool binary = false);
};

};