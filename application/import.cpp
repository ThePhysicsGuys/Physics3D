#include "import.h"

#include <vector>
#include <fstream>
#include <map>

#include "../util/stringUtil.h"
#include "../util/log.h"

#include "view\material.h"

#include "../engine/physical.h"

/*
	Import
*/

int Import::parseInt(std::string num) {
	return std::stoi(num);
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
		vector.v[i] = parseDouble(tokens[i]);
	}
	return vector;
}

DiagonalMat3 Import::parseDiagonalMat3(std::string mat) {
	std::vector<std::string> tokens = split(mat, ' ');
	DiagonalMat3 matrix = DiagonalMat3();
	for (int i = 0; i < 3; i++) {
		matrix.m[i] = Import::parseDouble(tokens[i]);
	}
	return matrix;
}

Mat3 Import::parseMat3(std::string mat) {
	std::vector<std::string> tokens = split(mat, ' ');
	Mat3 matrix = Mat3();
	for (int i = 0; i < 9; i++) {
		matrix.m[i] = Import::parseDouble(tokens[i]);
	}
	return matrix;
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
	unsigned int position;
	unsigned int normal;
	unsigned int uv;

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

Shape reorder(const std::vector<Vec3>& positions, const std::vector<Vec3>& normals, const std::vector<Vec2>& uvs, const std::vector<Face>& faces, Flags flags) {
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

	return Shape(positionArray, normalArray, uvArray, triangleArray, (int)positions.size(), (int)faces.size());
}

Shape loadBinaryObj(std::istream& input) {
	char flag = Import::read<char>(input);
	int vertexCount = Import::read<int>(input);
	int triangleCount = Import::read<int>(input);

	char V = 0;
	char VN = 1;
	char VT = 2;
	char VNT = 3;

	Vec3* vertices = new Vec3[vertexCount];
	for (int i = 0; i < vertexCount; i++) {
		Vec3 t = Import::read<Vec3f>(input);

		vertices[i] = t;
	}

	Vec3* normals = nullptr;
	if (flag == VN || flag == VNT) {
		normals = new Vec3[vertexCount];
		for (int i = 0; i < vertexCount; i++) {
			normals[i] = Import::read<Vec3f>(input);
		}
	}

	Vec2* uvs = nullptr;
	if (flag == VT || flag == VNT) {
		uvs = new Vec2[vertexCount];
		for (int i = 0; i < vertexCount; i++) {
			uvs[i] = Import::read<Vec2f>(input);
		}
	}

	Triangle* triangles = new Triangle[triangleCount];
	for (int i = 0; i < triangleCount; i++) {
		triangles[i] = Import::read<Triangle>(input);
	}

	return Shape(vertices, normals, uvs, triangles, vertexCount, triangleCount);
}

Shape loadNonBinaryObj(std::istream& input) {
	std::vector<Vec3> vertices;
	std::vector<Vec3> normals;
	std::vector<Vec2> uvs;
	std::vector<Face> faces;
	Flags flags = { false, false };

	std::string line;
	while (getline(input, line)) {
		std::vector<std::string> tokens = split(line, ' ');

		if (tokens[0] == "v") {
			Vec3 vertex = Vec3(stod(tokens[1]), stod(tokens[2]), stod(tokens[3]));
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
			Vec2 uv = Vec2(stod(tokens[1]), stod(tokens[2]));
			uvs.push_back(uv);
		} else if (tokens[0] == "vn") {
			flags.normals = true;
			Vec3 normal = Vec3(stod(tokens[1]), stod(tokens[2]), stod(tokens[3]));
			normals.push_back(normal);
		}
	}

	return reorder(vertices, normals, uvs, faces, flags);
}

Shape OBJImport::load(std::istream& file, bool binary) {
	if (binary)
		return loadBinaryObj(file);
	else
		return loadNonBinaryObj(file);
}

Shape OBJImport::load(std::string file, bool binary) {
	std::ifstream input;

	if (binary)
		input.open(file, std::ios::binary);
	else
		input.open(file);

	Shape shape = load(input, binary);

	input.close();

	return shape;
}

/*
	End of OBJImport
*/



/*
	WorldImport
*/

enum class Subject {
	PART = 2,
	CAMERA = 1,
	GLOBAL = 0,
	NONE = -1
};

void parseSubject(Subject subject, std::string path, std::map<std::string, std::string>& fields, World<ExtendedPart>& world, Camera& camera) {
	if (subject == Subject::PART) {
		Material material = Material();
		material.ambient = Import::parseVec3(fields.at("ambient"));
		material.diffuse = Import::parseVec3(fields.at("diffuse"));
		material.specular = Import::parseVec3(fields.at("specular"));
		material.reflectance = Import::parseFloat(fields.at("reflectance"));

		if (!fields.at("texture").empty())
			material.texture = load(fields.at("texture"));

		if (!fields.at("normals").empty())
			material.normal = load(fields.at("normals"));

		double density = Import::parseDouble(fields.at("density"));
		double friction = Import::parseDouble(fields.at("friction"));
		double mass = Import::parseDouble(fields.at("mass"));

		Vec3 pos = Import::parseVec3(fields.at("position"));
		Mat3 rot = Import::parseMat3(fields.at("rotation"));

		std::string name = fields.at("name");
		int mode = Import::parseInt(fields.at("mode"));

		Vec3 velocity = Import::parseVec3(fields.at("velocity"));
		Vec3 angularvelocity = Import::parseVec3(fields.at("angularvelocity"));
		DiagonalMat3 inertia = Import::parseDiagonalMat3(fields.at("inertia"));

		bool anchored = Import::parseInt(fields.at("anchored"));

		std::string shapeReference = fields.at("shape");

		Shape shape = OBJImport::load(path + shapeReference);
		CFrame cframe = CFrame(pos, rot);
		ExtendedPart part = ExtendedPart(shape, cframe, density, friction, 0, name);

		Physical physical = Physical(&part, mass, inertia);
		physical.angularVelocity = angularvelocity;
		physical.velocity = velocity;

		part.parent = &physical;
		part.material = material;
		part.renderMode = mode;

		world.addObject(&part, anchored);
	} else if (subject == Subject::CAMERA) {
		Vec3 pos = Import::parseVec3(fields.at("position"));
		Mat3 rot = Import::parseMat3(fields.at("rotation"));
		double speed = Import::parseDouble(fields.at("speed"));
		double rspeed = Import::parseDouble(fields.at("rspeed"));
		bool fly = Import::parseInt(fields.at("flying"));

		camera.cframe = CFrame(pos, rot);
		camera.speed = speed;
		camera.rspeed = rspeed;
		camera.flying = fly;
	} else if (subject == Subject::GLOBAL) {

	}

	fields.clear();
}

void loadBinaryWorld(std::string name, World<ExtendedPart>& world, Camera& camera) {

}

void loadNonBinaryWorld(std::string name, World<ExtendedPart>& world, Camera& camera) {
	Subject subject = Subject::NONE;
	std::map<std::string, std::string> fields;
	
	std::string path = std::string("./") + name + "/";
	std::string filename = path + "world.txt";
	std::ifstream input(filename.c_str());

	std::string line;
	while (getline(input, line)) {
		line = trim(line);
		if (line.empty() || line.at(0) == '#') {
			// next line
		} else {
			if (startWith(line, "part:")) {
				parseSubject(subject, path, fields, world, camera);
				subject = Subject::PART;
				continue;
			}

			if (startWith(line, "camera:")) {
				parseSubject(subject, path, fields, world, camera);
				subject = Subject::CAMERA;
				continue;
			}

			if (startWith(line, "global:")) {
				parseSubject(subject, path, fields, world, camera);
				subject = Subject::GLOBAL;
				continue;
			}

			size_t pos = line.find("=");
			std::string property = rtrim(line.substr(0, pos));
			std::string value = ltrim(line.substr(pos + 1, line.length()));

			fields[property] = value;
		}
	}

	parseSubject(subject, path, fields, world, camera);
}

void WorldImport::load(std::string name, World<ExtendedPart>& world, Camera& camera, bool binary) {
	if (binary)
		loadBinaryWorld(name, world, camera);
	else
		loadNonBinaryWorld(name, world, camera);
}

/*
	End of WorldImport
*/