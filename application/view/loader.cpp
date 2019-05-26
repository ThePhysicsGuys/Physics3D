/*#include "loader.h"

#include <vector>
#include <string>
#include <regex>
#include <fstream>
#include <sys/stat.h>

#include "../util/stringUtil.h"

#include "../engine/math/vec3.h"
#include "../engine/math/vec2.h"
#include "../engine/geometry/shape.h"

const int NO_VALUE = -1;
struct Group {
	unsigned int position;
	unsigned int normal;
	unsigned int uv;

	Group() {
		position = NO_VALUE;
		normal = NO_VALUE;
		uv = NO_VALUE;
	}
};

struct Flags {
	bool normals;
	bool uvs;
};

struct Face {
	Group groups[3];
	
	Face(std::string v1, std::string v2, std::string v3) {
		groups[0] = parseLine(v1);
		groups[1] = parseLine(v2);
		groups[2] = parseLine(v3);
	}

	Group parseLine(std::string line) {
		Group group = Group();

		std::vector<std::string> tokens = split(line, '/');
		size_t length = tokens.size();

		// Positions
		group.position = std::stoi(tokens[0]) - 1;

		// Uvs
		if (length > 1) {
			std::string uv = tokens[1];
			group.uv = uv.size() > 0 ? std::stoi(uv) - 1 : NO_VALUE;
		}
		
		// Normals
		if (length > 2) {
			group.normal = std::stoi(tokens[2]) - 1;
		}

		return group;
	}
};

Shape reorderLists(const std::vector<Vec3>& positions, const std::vector<Vec3>& normals, const std::vector<Vec2>& uvs, const std::vector<Face>& faces, Flags flags)	{
	Triangle* triangleArray = nullptr;
	Vec3f* positionArray = nullptr;
	Vec3* normalArray = nullptr;
	Vec2* uvArray = nullptr;

	// Positions
	positionArray = new Vec3f[positions.size()];
	for (int i = 0; i < positions.size(); i++) {
		positionArray[i] = Vec3f(positions[i]);
	}

	triangleArray = new Triangle[faces.size()];

	if (flags.normals) normalArray = new Vec3[positions.size()];
	if (flags.uvs) uvArray = new Vec2[positions.size()];

	for (int i = 0; i < faces.size(); i++) {
		const Group* indices = faces[i].groups;

		// Triangles
		triangleArray[i] = Triangle {
			indices[0].position,
			indices[1].position,
			indices[2].position
		};

		for (int i = 0; i < 3; i++) {
			const Group& index = indices[i];
			
			if (flags.normals && indices->normal != NO_VALUE) {
				normalArray[index.position] = normals[index.normal];
			}

			if (flags.uvs && indices->uv != NO_VALUE) {
				uvArray[index.position] = Vec2(uvs[index.uv].x, 1.0 - uvs[index.uv].y);
			}
		}
	}

	return Shape(positionArray, normalArray, uvArray, triangleArray, (int) positions.size(), (int) faces.size());
}

Shape loadObj(std::string filename) {
	std::ifstream input;
	input.open(filename);

	Shape shape = loadObj(input);

	input.close();

	return shape;
}

Shape loadObj(std::istream& stream) {
	std::vector<Vec3> vertices;
	std::vector<Vec3> normals;
	std::vector<Vec2> uvs;
	std::vector<Face> faces;
	Flags flags = { false, false };

	std::string line;
	while (getline(stream, line)) {
		std::vector<std::string> tokens = split(line, ' ');

		if (tokens[0] == "v") {
			Vec3 vertex = Vec3(
				stod(tokens[1]),
				stod(tokens[2]),
				stod(tokens[3])
			);
			vertices.push_back(vertex);
		} else if (tokens[0] == "f") {
			Face face = Face (
				tokens[1], 
				tokens[2], 
				tokens[3]
			);

			faces.push_back(face);

			if (tokens.size() > 4) {
				Face face = Face (
					tokens[1],
					tokens[3],
					tokens[4]
				);

				faces.push_back(face);
			}
		} else if (tokens[0] == "vt") {
			flags.uvs = true;
			Vec2 uv = Vec2 (
				stod(tokens[1]),
				stod(tokens[2])
			);
			uvs.push_back(uv);
		} else if (tokens[0] == "vn") {
			flags.normals = true;
			Vec3 normal = Vec3 (
				stod(tokens[1]),
				stod(tokens[2]),
				stod(tokens[3])
			);
			normals.push_back(normal);
		}
	}

	return reorderLists(vertices, normals, uvs, faces, flags);
}

void saveObj(std::string filename, const Shape& shape) {
	struct stat buffer;

	if (stat(filename.c_str(), &buffer) != -1) {
		Log::warn("File already exists: %s", filename.c_str());
	}

	std::ofstream output;
	output.open(filename);

	for (Vec3f vertex : shape.iterVertices()) {
		output << "v " << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
	}

	if (shape.normals) {
		for (int i = 0; i < shape.vertexCount; i++) {
			Vec3 normal = shape.normals.get()[i];
			output << "vn " << normal.x << " " << normal.y << " " << normal.z << std::endl;
		}
	}

	if (shape.uvs) {
		for (int i = 0; i < shape.vertexCount; i++) {
			Vec2 uv = shape.uvs.get()[i];
			output << "vt " << uv.x << " " << uv.y << std::endl;
		}
	}

	for (Triangle triangle : shape.iterTriangles()) {
		output << "f";
		
		for (int index : triangle.indexes) {
			index++;
			if (shape.normals && shape.uvs) {
				output << " " << index << "/" << index << "/" << index;
			} else if (shape.uvs) {
				output << " " << index << "//" << index;
			} else if (shape.normals) {
				output << " " << index << "/" << index;
			} else {
				output << " " << index;
			}
		}
		
		output << std::endl;
	}

	output.close();
}

void writeCharBuffer(std::ostream& output, char const * buffer, int size) {
	for (int i = 0; i < size; i++) {
		output.write((char*)(buffer + i), 1);
	}
}

template<typename T>
void write(std::ostream& output, T& value) {
	char const * v = reinterpret_cast<char const *>(&value);
	writeCharBuffer(output, v, sizeof(T));
}

template<typename T>
T read(std::istream& input) {
	char* buffer = (char*) alloca(sizeof(T));
	input.read(buffer, sizeof(T));
	return *reinterpret_cast<T*>(buffer);
}

Shape loadBinaryObj(std::istream& input) {
	char flag = read<char>(input);
	int vertexCount = read<int>(input);
	int triangleCount = read<int>(input);

	char V = 0;
	char VN = 1;
	char VT = 2;
	char VNT = 3;

	Vec3* vertices = new Vec3[vertexCount];
	for (int i = 0; i < vertexCount; i++) {
		vertices[i] = read<Vec3>(input);
	}

	Vec3* normals = nullptr;
	if (flag == VN || flag == VNT) {
		normals = new Vec3[vertexCount];
		for (int i = 0; i < vertexCount; i++) {
			normals[i] = read<Vec3>(input);
		}
	}

	Vec2* uvs = nullptr;
	if (flag == VT || flag == VNT) {
		uvs = new Vec2[vertexCount];
		for (int i = 0; i < vertexCount; i++) {
			uvs[i] = read<Vec2>(input);
		}
	}

	Triangle* triangles = new Triangle[triangleCount];
	for (int i = 0; i < triangleCount; i++) {
		triangles[i] = read<Triangle>(input);
	}

	return Shape(vertices, normals, uvs, triangles, vertexCount, triangleCount);
}

Shape loadBinaryObj(std::string filename) {
	std::ifstream input;
	input.open(filename);

	Shape shape = loadBinaryObj(input);

	input.close();

	return shape;
}


void saveBinaryObj(std::string filename, const Shape& shape) {
	struct stat buffer;

	if (stat(filename.c_str(), &buffer) != -1) {
		Log::warn("File already exists: %s", filename.c_str());
	}

	std::ofstream output;

	output.open(filename, std::ios::binary | std::ios::out);

	char V = 0;
	char VN = 1;
	char VT = 2;
	char VNT = 3;

	char flag = V;

	if (shape.uvs && shape.normals) {
		flag = VNT;
	} else if (shape.uvs) {
		flag = VT;
	} else if (shape.normals) {
		flag = VN;
	}
	
	write<char>(output, flag);
	write<const int>(output, shape.vertexCount);
	write<const int>(output, shape.triangleCount);

	for (Vec3f vertex : shape.iterVertices()) {
		write<float>(output, vertex.x);
		write<float>(output, vertex.y);
		write<float>(output, vertex.z);
	}
	
	if (shape.normals) {
		for (int i = 0; i < shape.vertexCount; i++) {
			Vec3f normal = shape.normals.get()[i];
			write<float>(output, normal.x);
			write<float>(output, normal.y);
			write<float>(output, normal.z);
		}
	}

	if (shape.uvs) {
		for (int i = 0; i < shape.vertexCount; i++) {
			Vec2f uv = shape.uvs.get()[i];
			write<float>(output, uv.x);
			write<float>(output, uv.y);
		}
	}

	for (Triangle triangle : shape.iterTriangles()) {
		for (int index : triangle.indexes) {
			write<int>(output, index);
		}
	}

	output.close();
}*/