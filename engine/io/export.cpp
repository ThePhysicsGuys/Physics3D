#include "core.h"

#include "export.h"

#include "../graphics/visualShape.h"

#include <fstream>
#include <sstream>

#include "../util/fileUtils.h"

/*
	Export
*/

template<typename T>
void Export::write(std::ostream& output, T const* buffer, int size) {
	for (int i = 0; i < size; i++) {
		output.write((T*)(buffer + i * sizeof(T)), 1);
	}
}

template<typename T>
void Export::write(std::ostream& output, const T& value) {
	char const * v = reinterpret_cast<char const *>(&value);
	write<char>(output, v, sizeof(T));
}

std::string Export::str(Vec3 vector) {
	std::stringstream ss;
	ss << vector.x << " " << vector.y << " " << vector.z;
	return ss.str();
}

std::string Export::str(Vec4 vector) {
	std::stringstream ss;
	ss << vector.x << " " << vector.y << " " << vector.z << " " << vector.w;
	return ss.str();
}

std::string Export::str(Mat3 matrix) {
	std::stringstream ss;

	double data[9];
	matrix.toColMajorData(data);

	ss << data[0];
	for (int i = 1; i < 9; i++) {
		ss << ' ' << data[i];
	}

	return ss.str();
}

std::string Export::str(Position pos) {
	std::stringstream ss;
	ss << pos.x.value << " " << pos.y.value << " " << pos.z.value;
	return ss.str();
}

std::string Export::str(DiagonalMat3 matrix) {
	return Export::str(Vec3(matrix[0], matrix[1], matrix[2]));
}

std::string Export::str(int num) {
	return std::to_string(num);
}

std::string Export::str(double num) {
	return std::to_string(num);
}

/*
	End of Export
*/

/*
	OBJExport
*/

void saveBinaryObj(std::string filename, const Graphics::VisualShape& shape) {
	FileUtils::warnIfFileExists(filename);

	std::ofstream output;
	output.open(filename, std::ios::binary | std::ios::out);

	enum : char {
		V,
		VN,
		VT,
		VNT
	};

	char flag = V;
	if (shape.uvs != nullptr && shape.normals != nullptr) {
		flag = VNT;
	} else if (shape.uvs != nullptr) {
		flag = VT;
	} else if (shape.normals != nullptr) {
		flag = VN;
	}

	Export::write<char>(output, flag);
	Export::write<const int>(output, shape.vertexCount);
	Export::write<const int>(output, shape.triangleCount);

	for (Vec3f vertex : shape.iterVertices()) {
		Export::write<Vec3f>(output, vertex);
	}

	if (shape.normals != nullptr) {
		for (int i = 0; i < shape.vertexCount; i++) {
			Vec3f normal = shape.normals.get()[i];
			Export::write<Vec3f>(output, normal);
		}
	}

	if (shape.uvs != nullptr) {
		for (int i = 0; i < shape.vertexCount; i++) {
			Vec2f uv = shape.uvs.get()[i];
			Export::write<Vec2f>(output, uv);
		}
	}

	for (Triangle triangle : shape.iterTriangles()) {
		for (int index : triangle.indexes) {
			Export::write<int>(output, index);
		}
	}

	output.close();
}

void saveNonBinaryObj(const std::string& filename, const Graphics::VisualShape& shape) {
	FileUtils::warnIfFileExists(filename);

	std::ofstream output;
	output.open(filename);

	for (Vec3f vertex : shape.iterVertices()) {
		output << "v " << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
	}

	if (shape.normals != nullptr) {
		for (int i = 0; i < shape.vertexCount; i++) {
			Vec3 normal = shape.normals.get()[i];
			output << "vn " << normal.x << " " << normal.y << " " << normal.z << std::endl;
		}
	}

	if (shape.uvs != nullptr) {
		for (int i = 0; i < shape.vertexCount; i++) {
			Vec2 uv = shape.uvs.get()[i];
			output << "vt " << uv.x << " " << uv.y << std::endl;
		}
	}

	for (Triangle triangle : shape.iterTriangles()) {
		output << "f";

		for (int index : triangle.indexes) {
			index++;
			if (shape.normals != nullptr && shape.uvs != nullptr) {
				output << " " << index << "/" << index << "/" << index;
			} else if (shape.uvs != nullptr) {
				output << " " << index << "//" << index;
			} else if (shape.normals != nullptr) {
				output << " " << index << "/" << index;
			} else {
				output << " " << index;
			}
		}

		output << std::endl;
	}

	output.close();
}

void OBJExport::save(const std::string& filename, const Graphics::VisualShape& shape, bool binary) {
	if (binary)
		saveBinaryObj(filename, shape);
	else
		saveNonBinaryObj(filename, shape);
}

/*
	End of OBJExport
*/
