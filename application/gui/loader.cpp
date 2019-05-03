#include "loader.h"

#include <vector>
#include <string>
#include <regex>
#include <iostream>

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

std::vector<std::string> split(const std::string& string, char splitter) {
	std::vector<std::string> elements;
	int length = string.size();
	int start = 0;

	for (int i = 0; i < length; i++) {
		if (string[i] == splitter) {
			if (i != start) 
				elements.push_back(string.substr(start, i - start));
			start = i + 1;
		}
	}

	if (start < length) 
		elements.push_back(string.substr(start, length - start));

	return elements;
}

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
	Vec3* positionArray = nullptr;
	Vec3* normalArray = nullptr;
	Vec2* uvArray = nullptr;

	// Positions
	positionArray = new Vec3[positions.size()];
	for (int i = 0; i < positions.size(); i++) {
		positionArray[i] = positions[i];
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

Shape loadMesh(std::istream& stream) {
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

