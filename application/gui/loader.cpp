#include "loader.h"

#include <vector>
#include <string>
#include <regex>
#include <iostream>

#include "../engine/math/vec3.h"
#include "../engine/geometry/shape.h"

const int NO_VALUE = -1;
struct IndexGroup {
	int position;

	IndexGroup() {
		position = NO_VALUE;
	}
};

std::vector<std::string> split(const std::string& string, char splitter) {
	std::vector<std::string> elements;
	int length = string.length();
	int start = 0;
	for (int i = 0; i < length; i++) {
		if (string[i] == splitter) {
			if (i != start) elements.push_back(string.substr(start, i - start));
			start = i + 1;
		}
	}
	if (start < length) elements.push_back(string.substr(start, length - start));
	return elements;
}

struct Face {
	IndexGroup indexGroups[3];
	
	Face(std::string v1, std::string v2, std::string v3) {
		indexGroups[0] = parseLine(v1);
		indexGroups[1] = parseLine(v2);
		indexGroups[2] = parseLine(v3);
	}

	IndexGroup parseLine(std::string line) {
		IndexGroup indexGroup = IndexGroup();

		std::vector<std::string> tokens = split(line, '/');
		indexGroup.position = std::stoi(tokens[0]) - 1;
		
		return indexGroup;
	}
};

Shape reorderLists(const std::vector<Vec3>& positions, const std::vector<Face>& faces) {
	std::vector<unsigned int> indicesVector;

	for (Face face : faces) {
		for (IndexGroup group : face.indexGroups) {
			indicesVector.push_back(group.position);
		}
	}

	Vec3* newPositions = new Vec3[positions.size()];
	Triangle* newTriangles = new Triangle[faces.size()];
	for(int i = 0; i < positions.size(); i++)
		newPositions[i] = positions[i];
	
	for(int i = 0; i < faces.size(); i++) {
		const IndexGroup* indexes = faces[i].indexGroups;
		newTriangles[i] = Triangle{static_cast<unsigned int>(indexes[0].position), static_cast<unsigned int>(indexes[1].position), static_cast<unsigned int>(indexes[2].position)};
	}

	return Shape(newPositions, newTriangles, positions.size(), faces.size());
}

Shape loadMesh(std::istream& stream) {
	std::vector<Vec3> vertices;
	std::vector<Face> faces;

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
			Face face = Face(
				tokens[1], 
				tokens[2], 
				tokens[3]
			);
			faces.push_back(face);
		}
	}
	return reorderLists(vertices, faces);
}

