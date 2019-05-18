#include "loader.h"

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
		int length = tokens.size();

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

	return Shape(positionArray, normalArray, uvArray, triangleArray, positions.size(), faces.size());
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
			Vec3 vertex = Vec3 (
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
				Face face = Face(
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

void saveObj(std::string filename, Shape shape) {
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