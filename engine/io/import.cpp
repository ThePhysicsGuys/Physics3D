#include "core.h"

#include "import.h"

#include <fstream>
#include <optional>

#include "../util/stringUtil.h"
#include <Physics3D/physical.h>
#include "../graphics/extendedTriangleMesh.h"

namespace P3D {

/*
	Import
*/

int Import::parseInt(const std::string& num) {
	return std::stoi(num);
}

long long Import::parseLong(const std::string& num) {
	return std::stoll(num);
}

Fix<32> Import::parseFix(const std::string& num) {
	long long v = std::stoll(num);
	return Fix<32>(static_cast<int64_t>(v));
}

double Import::parseDouble(const std::string& num) {
	return std::stod(num);
}

float Import::parseFloat(const std::string& num) {
	return std::stof(num);
}

Vec3 Import::parseVec3(const std::string& vec) {
	std::vector<std::string> tokens = Util::split(vec, ' ');
	Vec3 vector = Vec3();
	for (int i = 0; i < 3; i++)
		vector[i] = Import::parseDouble(tokens[i]);
	
	return vector;
}

Position Import::parsePosition(const std::string& vec) {
	std::vector<std::string> tokens = Util::split(vec, ' ');
	return Position(Import::parseFix(tokens[0]), Import::parseFix(tokens[1]), Import::parseFix(tokens[2]));
}

Vec4 Import::parseVec4(const std::string& vec) {
	std::vector<std::string> tokens = Util::split(vec, ' ');
	Vec4 vector;
	for (int i = 0; i < 4; i++)
		vector[i] = Import::parseDouble(tokens[i]);
	
	return vector;
}

Vec4f Import::parseVec4f(const std::string& vec) {
	std::vector<std::string> tokens = Util::split(vec, ' ');
	Vec4f vector;
	for (int i = 0; i < 4; i++)
		vector[i] = Import::parseFloat(tokens[i]);
	
	return vector;
}

Vec3f Import::parseVec3f(const std::string& vec) {
	std::vector<std::string> tokens = Util::split(vec, ' ');
	Vec3f vector = Vec3f();
	for (int i = 0; i < 3; i++)
		vector[i] = Import::parseFloat(tokens[i]);
	
	return vector;
}

DiagonalMat3 Import::parseDiagonalMat3(const std::string& mat) {
	std::vector<std::string> tokens = Util::split(mat, ' ');
	DiagonalMat3 matrix = DiagonalMat3();
	for (int i = 0; i < 3; i++)
		matrix[i] = Import::parseDouble(tokens[i]);
	
	return matrix;
}

Mat3 Import::parseMat3(const std::string& mat) {
	std::vector<std::string> tokens = Util::split(mat, ' ');
	double data[9];

	for (int i = 0; i < 9; i++)
		data[i] = Import::parseDouble(tokens[i]);

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

struct Vertex {
	int position;
	std::optional<int> normal;
	std::optional<int> uv;

	Vertex() : position(0), normal(std::nullopt), uv(std::nullopt) {}

	Vertex(const std::string_view& line) {
		std::vector<std::string_view> tokens = Util::split_view(line, '/');
		size_t length = tokens.size();

		// Positions
		position = std::stoi(std::string(tokens[0])) - 1;

		// Uvs
		if (length > 1 && !tokens[1].empty())
			uv = std::stoi(std::string(tokens[1])) - 1;

		// Normals
		if (length > 2 && !tokens[2].empty())
			normal = std::stoi(std::string(tokens[2])) - 1;
	}

	bool operator==(const Vertex& other) const {
		return position == other.position && normal == other.normal && uv == other.uv;
	}
};

struct VertexHasher {
	size_t operator()(const P3D::Vertex& vertex) const {

		size_t result = std::hash<int>()(vertex.position);

		if (vertex.normal.has_value())
			result = result >> 1 ^ std::hash<int>()(vertex.normal.value()) << 1;

		if (vertex.uv.has_value())
			result = result >> 1 ^ std::hash<int>()(vertex.uv.value()) << 1;

		return result;
	}
};

struct Face {
	Vertex vertices[3];

	Face(const Vertex& v1, const Vertex& v2, const Vertex& v3) {
		vertices[0] = v1;
		vertices[1] = v2;
		vertices[2] = v3;
	}

	Vertex& operator[](std::size_t index) {
		assert(index < 3);

		return vertices[index];
	}

	const Vertex& operator[](int index) const {
		assert(index < 3);

		return vertices[index];
	}
};

Graphics::ExtendedTriangleMesh reorderWithoutSharedVerticesSupport(const std::vector<Vec3f>& positions, const std::vector<Vec3f>& normals, const std::vector<Vec2f>& uvs, const std::vector<Face>& faces) {
	// Positions
	Vec3f* positionArray = new Vec3f[positions.size()];
	for (std::size_t i = 0; i < positions.size(); i++)
		positionArray[i] = positions[i];

	// Normals
	Vec3f* normalArray = nullptr;
	if (!normals.empty())
		normalArray = new Vec3f[positions.size()];

	// UVs, tangents and bitangents
	Vec2f* uvArray = nullptr;
	Vec3f* tangentArray = nullptr;
	Vec3f* bitangentArray = nullptr;
	if (!uvs.empty()) {
		uvArray = new Vec2f[positions.size()];
		tangentArray = new Vec3f[positions.size()];
		bitangentArray = new Vec3f[positions.size()];
	}

	// Triangles
	Triangle* triangleArray = new Triangle[faces.size()];
	for (std::size_t faceIndex = 0; faceIndex < faces.size(); faceIndex++) {
		const Face& face = faces[faceIndex];

		// Save triangle
		triangleArray[faceIndex] = Triangle { face.vertices[0].position, face.vertices[1].position, face.vertices[2].position };

		// Calculate tangent and bitangent
		Vec3f tangent;
		Vec3f bitangent;
		if (uvArray) {
			Vec3f edge1 = positions[face.vertices[1].position] - positions[face.vertices[0].position];
			Vec3f edge2 = positions[face.vertices[2].position] - positions[face.vertices[0].position];
			Vec2f dUV1 = uvs[*face.vertices[1].uv] - uvs[*face.vertices[0].uv];
			Vec2f dUV2 = uvs[*face.vertices[2].uv] - uvs[*face.vertices[0].uv];

			float f = 1.0f / (dUV1.x * dUV2.y - dUV2.x * dUV1.y);

			tangent.x = f * (dUV2.y * edge1.x - dUV1.y * edge2.x);
			tangent.y = f * (dUV2.y * edge1.y - dUV1.y * edge2.y);
			tangent.z = f * (dUV2.y * edge1.z - dUV1.y * edge2.z);
			tangent = normalize(tangent);

			bitangent.x = f * (-dUV2.x * edge1.x + dUV1.x * edge2.x);
			bitangent.y = f * (-dUV2.x * edge1.y + dUV1.x * edge2.y);
			bitangent.z = f * (-dUV2.x * edge1.z + dUV1.x * edge2.z);
			bitangent = normalize(bitangent);
		}

		for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++) {
			const Vertex& vertex = face[vertexIndex];

			// Save normal
			if (normalArray && vertex.normal.has_value())
				normalArray[vertex.position] = normals[*vertex.normal];

			// Save uv, tangent and bitangent
			if (uvArray && vertex.uv.has_value()) {
				uvArray[vertex.position] = uvs[*vertex.uv];
				tangentArray[vertex.position] = tangent;
				bitangentArray[vertex.position] = bitangent;
			}
		}
	}

	Graphics::ExtendedTriangleMesh result(positionArray, static_cast<int>(positions.size()), triangleArray, static_cast<int>(faces.size()));
	result.setNormalBuffer(SRef<const Vec3f[]>(normalArray));
	result.setUVBuffer(SRef<const Vec2f[]>(uvArray));
	result.setTangentBuffer(SRef<const Vec3f[]>(tangentArray));
	result.setBitangentBuffer(SRef<const Vec3f[]>(bitangentArray));

	return result;
}

Graphics::ExtendedTriangleMesh reorderWithSharedVerticesSupport(const std::vector<Vec3f>& positions, const std::vector<Vec3f>& normals, const std::vector<Vec2f>& uvs, const std::vector<Face>& faces) {
	std::unordered_map<Vertex, int, VertexHasher> mapping;

	// Get index of each vertex - uv - normal tuple
	auto getIndex = [&mapping] (const Vertex& vertex) -> int {
		auto iterator = mapping.find(vertex);
		if (iterator == mapping.end()) {
			int index = static_cast<int>(mapping.size());
			mapping.emplace(vertex, index);

			return index;
		}

		return iterator->second;
	};

	// Fill triangle array
	Triangle* triangleArray = new Triangle[faces.size()];
	for (std::size_t faceIndex = 0; faceIndex < faces.size(); faceIndex++) {
		const Face& face = faces[faceIndex];

		const Vertex& v0 = face.vertices[0];
		const Vertex& v1 = face.vertices[1];
		const Vertex& v2 = face.vertices[2];

		int i0 = getIndex(v0);
		int i1 = getIndex(v1);
		int i2 = getIndex(v2);

		// Save triangle
		triangleArray[faceIndex] = Triangle { i0, i1, i2 };
	}

	// Array size
	std::size_t size = mapping.size();

	// Positions
	Vec3f* positionArray = new Vec3f[size];

	// Normals
	Vec3f* normalArray = nullptr;
	if (!normals.empty())
		normalArray = new Vec3f[size];

	// UV
	Vec2f* uvArray = nullptr;
	if (!uvs.empty()) 
		uvArray = new Vec2f[size];

	// Fill arrays
	for (const auto& [vertex, index] : mapping) {
		// Store position
		positionArray[index] = positions[vertex.position];
		
		// Store normal
		if (normalArray && vertex.normal.has_value())
			normalArray[index] = normals[*vertex.normal];

		// Store uv
		if (uvArray && vertex.uv.has_value()) 
			uvArray[index] = uvs[*vertex.uv];
	}

	Graphics::ExtendedTriangleMesh result(positionArray, size, triangleArray, static_cast<int>(faces.size()));
	result.setNormalBuffer(SRef<const Vec3f[]>(normalArray));
	result.setUVBuffer(SRef<const Vec2f[]>(uvArray));

	return result;
}

Graphics::ExtendedTriangleMesh loadBinaryObj(std::istream& input) {
	char flag = Import::read<char>(input);
	int vertexCount = Import::read<int>(input);
	int triangleCount = Import::read<int>(input);

	char V = 0;
	char VN = 1;
	char VT = 2;
	char VNT = 3;

	Vec3f* vertices = new Vec3f[vertexCount];
	for (int i = 0; i < vertexCount; i++) {
		Vec3 vertex = Import::read<Vec3f>(input);

		vertices[i] = vertex;
	}

	Vec3f* normals = nullptr;
	if (flag == VN || flag == VNT) {
		normals = new Vec3f[vertexCount];

		for (int i = 0; i < vertexCount; i++) {
			normals[i] = Import::read<Vec3f>(input);
		}
	}

	Vec2f* uvs = nullptr;
	Vec3f* tangents = nullptr;
	Vec3f* bitangents = nullptr;
	if (flag == VT || flag == VNT) {
		uvs = new Vec2f[vertexCount];
		tangents = new Vec3f[vertexCount];
		bitangents = new Vec3f[vertexCount];

		for (int i = 0; i < vertexCount; i++) {
			uvs[i] = Import::read<Vec2f>(input);
		}
	}

	Triangle* triangles = new Triangle[triangleCount];
	for (int i = 0; i < triangleCount; i++) {
		Triangle triangle = Import::read<Triangle>(input);

		triangles[i] = triangle;

		// Calculate (bi)tangents
		if (flag == VT || flag == VNT) {
			Vec3f tangent;
			Vec3f bitangent;

			Vec3f edge1 = vertices[triangle.secondIndex] - vertices[triangle.firstIndex];
			Vec3f edge2 = vertices[triangle.thirdIndex] - vertices[triangle.firstIndex];
			Vec2f dUV1 = uvs[triangle.secondIndex] - uvs[triangle.firstIndex];
			Vec2f dUV2 = uvs[triangle.thirdIndex] - uvs[triangle.firstIndex];

			float f = 1.0f / (dUV1.x * dUV2.y - dUV2.x * dUV1.y);

			tangent.x = f * (dUV2.y * edge1.x - dUV1.y * edge2.x);
			tangent.y = f * (dUV2.y * edge1.y - dUV1.y * edge2.y);
			tangent.z = f * (dUV2.y * edge1.z - dUV1.y * edge2.z);
			tangents[triangle.firstIndex] = normalize(tangent);
			tangents[triangle.secondIndex] = normalize(tangent);
			tangents[triangle.thirdIndex] = normalize(tangent);

			bitangent.x = f * (-dUV2.x * edge1.x + dUV1.x * edge2.x);
			bitangent.y = f * (-dUV2.x * edge1.y + dUV1.x * edge2.y);
			bitangent.z = f * (-dUV2.x * edge1.z + dUV1.x * edge2.z);
			bitangents[triangle.firstIndex] = normalize(bitangent);
			bitangents[triangle.secondIndex] = normalize(bitangent);
			bitangents[triangle.thirdIndex] = normalize(bitangent);
		}
	}

	Graphics::ExtendedTriangleMesh result(vertices, vertexCount, triangles, triangleCount);
	result.setNormalBuffer(SRef<const Vec3f[]>(normals));
	result.setUVBuffer(SRef<const Vec2f[]>(uvs));
	result.setTangentBuffer(SRef<const Vec3f[]>(tangents));
	result.setBitangentBuffer(SRef<const Vec3f[]>(bitangents));

	return result;
}

Graphics::ExtendedTriangleMesh loadNonBinaryObj(std::istream& input) {
	std::vector<Vec3f> vertices;
	std::vector<Vec3f> normals;
	std::vector<Vec2f> uvs;
	std::vector<Face> faces;

	std::string line;
	while (getline(input, line)) {
		std::vector<std::string_view> tokens = Util::split_view(line, ' ');

		if (tokens.empty())
			continue;

		if (tokens[0] == "v") {
			float x = std::stof(std::string(tokens[1]));
			float y = std::stof(std::string(tokens[2]));
			float z = std::stof(std::string(tokens[3]));

			vertices.emplace_back(x, y, z);
		} else if (tokens[0] == "f") {
			Vertex v1(tokens[1]);
			Vertex v2(tokens[2]);
			Vertex v3(tokens[3]);

			faces.emplace_back(v1, v2, v3);

			if (tokens.size() > 4)
				faces.emplace_back(v1, v3, Vertex(tokens[4]));
		} else if (tokens[0] == "vt") {
			float u = std::stof(std::string(tokens[1]));
			float v = std::stof(std::string(tokens[2]));

			uvs.emplace_back(u, v);
		} else if (tokens[0] == "vn") {
			float x = std::stof(std::string(tokens[1]));
			float y = std::stof(std::string(tokens[2]));
			float z = std::stof(std::string(tokens[3]));

			normals.emplace_back(x, y, z);
		}
	}

	return reorderWithSharedVerticesSupport(vertices, normals, uvs, faces);
}

Graphics::ExtendedTriangleMesh OBJImport::load(std::istream& file, bool binary) {
	if (binary)
		return loadBinaryObj(file);
	else
		return loadNonBinaryObj(file);
}

Graphics::ExtendedTriangleMesh OBJImport::load(const std::string& file) {
	bool binary;
	if (Util::endsWith(file, ".bobj"))
		binary = true;
	else if (Util::endsWith(file, ".obj"))
		binary = false;
	else
		return Graphics::ExtendedTriangleMesh();

	return OBJImport::load(file, binary);
}

Graphics::ExtendedTriangleMesh OBJImport::load(const std::string& file, bool binary) {
	std::ifstream input;

	if (binary)
		input.open(file, std::ios::binary);
	else
		input.open(file);

	Graphics::ExtendedTriangleMesh shape = load(input, binary);

	input.close();

	return shape;
}

/*
	End of OBJImport
*/

}