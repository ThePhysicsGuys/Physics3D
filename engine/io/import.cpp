#include "core.h"

#include "import.h"

#include <fstream>

#include "../util/stringUtil.h"
#include "../physics/physical.h"
#include "../graphics/visualShape.h"
/*
	Import
*/

int Import::parseInt(std::string num) {
	return std::stoi(num);
}

long long Import::parseLong(std::string num) {
	return std::stoll(num);
}

double Import::parseDouble(std::string num) {
	return std::stod(num);
}

float Import::parseFloat(std::string num) {
	return std::stof(num);
}

Vec3 Import::parseVec3(std::string vec) {
	std::vector<std::string> tokens = split(vec, ' ');
	Vec3 vector = Vec3();
	for (int i = 0; i < 3; i++) {
		vector[i] = Import::parseDouble(tokens[i]);
	}
	return vector;
}

Position Import::parsePosition(std::string vec) {
	std::vector<std::string> tokens = split(vec, ' ');
	Position vector;
	for (int i = 0; i < 3; i++) {
		vector[i] = Fix<32>(Import::parseLong(tokens[i]));
	}
	return vector;
}

Vec4 Import::parseVec4(std::string vec) {
	std::vector<std::string> tokens = split(vec, ' ');
	Vec4 vector = Vec4();
	for (int i = 0; i < 4; i++) {
		vector[i] = Import::parseDouble(tokens[i]);
	}
	return vector;
}

Vec4f Import::parseVec4f(std::string vec) {
	std::vector<std::string> tokens = split(vec, ' ');
	Vec4f vector = Vec4f();
	for (int i = 0; i < 4; i++) {
		vector[i] = Import::parseFloat(tokens[i]);
	}
	return vector;
}

Vec3f Import::parseVec3f(std::string vec) {
	std::vector<std::string> tokens = split(vec, ' ');
	Vec3f vector = Vec3f();
	for (int i = 0; i < 3; i++) {
		vector[i] = Import::parseFloat(tokens[i]);
	}
	return vector;
}

DiagonalMat3 Import::parseDiagonalMat3(std::string mat) {
	std::vector<std::string> tokens = split(mat, ' ');
	DiagonalMat3 matrix = DiagonalMat3();
	for (int i = 0; i < 3; i++) {
		matrix[i] = Import::parseDouble(tokens[i]);
	}
	return matrix;
}

Mat3 Import::parseMat3(std::string mat) {
	std::vector<std::string> tokens = split(mat, ' ');
	double data[9];

	for (int i = 0; i < 9; i++) {
		data[i] = Import::parseDouble(tokens[i]);
	}

	return Matrix<double, 3, 3>::fromColMajorData(data);
}

template<typename T>
T Import::read(std::istream& input) {
	char * buffer = (char*) alloca(sizeof(T));
	input.read(buffer, sizeof(T));
	return *reinterpret_cast<T*>(buffer);
}

/*
	End of Import
*/



/*
	OBJImport
*/

struct Group {
	int position;
	int normal;
	int uv;

	Group() {
		position = -1;
		normal = -1;
		uv = -1;
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
			group.uv = uv.size() > 0 ? std::stoi(uv) - 1 : -1;
		}

		// Normals
		if (length > 2) {
			group.normal = std::stoi(tokens[2]) - 1;
		}

		return group;
	}
};

Graphics::VisualShape reorder(const std::vector<Vec3f>& positions, const std::vector<Vec3f>& normals, const std::vector<Vec2f>& uvs, const std::vector<Face>& faces, Flags flags) {
	
	// Positions
	Vec3f* positionArray = new Vec3f[positions.size()];
	for (int i = 0; i < positions.size(); i++) {
		positionArray[i] = positions[i];
	}

	Triangle* triangleArray = new Triangle[faces.size()];

	Vec3f* normalArray = nullptr;
	Vec2f* uvArray = nullptr;
	if (flags.normals) normalArray = new Vec3f[positions.size()];
	if (flags.uvs) uvArray = new Vec2f[positions.size()];

	for (int i = 0; i < faces.size(); i++) {
		const Group* indices = faces[i].groups;

		triangleArray[i] = { indices[0].position, indices[1].position, indices[2].position };

		for (int i = 0; i < 3; i++) {
			const Group& index = indices[i];

			if (flags.normals && indices->normal != -1) {
				normalArray[index.position] = normals[index.normal];
			}

			if (flags.uvs && indices->uv != -1) {
				uvArray[index.position] = Vec2(uvs[index.uv].x, 1.0 - uvs[index.uv].y);
			}
		}
	}

	return Graphics::VisualShape(positionArray, SharedArrayPtr<const Vec3f>(normalArray), SharedArrayPtr<const Vec2f>(uvArray), triangleArray, (int)positions.size(), (int)faces.size());
}

Graphics::VisualShape loadBinaryObj(std::istream& input) {
	char flag = Import::read<char>(input);
	int vertexCount = Import::read<int>(input);
	int triangleCount = Import::read<int>(input);

	char V = 0;
	char VN = 1;
	char VT = 2;
	char VNT = 3;

	Vec3f* vertices = new Vec3f[vertexCount];
	for (int i = 0; i < vertexCount; i++) {
		Vec3 t = Import::read<Vec3f>(input);

		vertices[i] = t;
	}

	Vec3f* normals = nullptr;
	if (flag == VN || flag == VNT) {
		normals = new Vec3f[vertexCount];
		for (int i = 0; i < vertexCount; i++) {
			normals[i] = Import::read<Vec3f>(input);
		}
	}

	Vec2f* uvs = nullptr;
	if (flag == VT || flag == VNT) {
		uvs = new Vec2f[vertexCount];
		for (int i = 0; i < vertexCount; i++) {
			uvs[i] = Import::read<Vec2f>(input);
		}
	}

	Triangle* triangles = new Triangle[triangleCount];
	for (int i = 0; i < triangleCount; i++) {
		triangles[i] = Import::read<Triangle>(input);
	}

	return Graphics::VisualShape(vertices, SharedArrayPtr<const Vec3f>(normals), SharedArrayPtr<const Vec2f>(uvs), triangles, vertexCount, triangleCount);
}

Graphics::VisualShape loadNonBinaryObj(std::istream& input) {
	std::vector<Vec3f> vertices;
	std::vector<Vec3f> normals;
	std::vector<Vec2f> uvs;
	std::vector<Face> faces;
	Flags flags = { false, false };

	std::string line;
	while (getline(input, line)) {
		std::vector<std::string> tokens = split(line, ' ');

		if (tokens.size() == 0)
			continue;

		if (tokens[0] == "v") {
			Vec3f vertex = Vec3f(stof(tokens[1]), stof(tokens[2]), stof(tokens[3]));
			vertices.push_back(vertex);
		} else if (tokens[0] == "f") {
			Face face = Face(tokens[1], tokens[2], tokens[3]);
			faces.push_back(face);

			if (tokens.size() > 4) {
				Face face = Face(tokens[1], tokens[3], tokens[4]);
				faces.push_back(face);
			}
		} else if (tokens[0] == "vt") {
			flags.uvs = true;
			Vec2f uv = Vec2f(stof(tokens[1]), stof(tokens[2]));
			uvs.push_back(uv);
		} else if (tokens[0] == "vn") {
			flags.normals = true;
			Vec3f normal = Vec3f(stof(tokens[1]), stof(tokens[2]), stof(tokens[3]));
			normals.push_back(normal);
		}
	}

	return reorder(vertices, normals, uvs, faces, flags);
}

Graphics::VisualShape OBJImport::load(std::istream& file, bool binary) {
	if (binary)
		return loadBinaryObj(file);
	else
		return loadNonBinaryObj(file);
}

Graphics::VisualShape OBJImport::load(std::string file, bool binary) {
	struct stat buffer;

	if (stat(file.c_str(), &buffer) == -1) {
		Log::subject(file.c_str());
		Log::error("File not found: %s", file.c_str());

		return Graphics::VisualShape();
	}

	std::ifstream input;

	if (binary)
		input.open(file, std::ios::binary);
	else
		input.open(file);

	Graphics::VisualShape shape = load(input, binary);

	input.close();

	return shape;
}

/*
	End of OBJImport
*/