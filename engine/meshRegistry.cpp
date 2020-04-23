#include "core.h"
#include "meshRegistry.h"

#include "../physics/misc/shapeLibrary.h"
#include "../physics/geometry/basicShapes.h"

#include "../graphics/visualShape.h"
#include "../graphics/buffers/bufferLayout.h"
#include "../graphics/buffers/vertexBuffer.h"
#include "../engine/visualData.h"
#include "../graphics/mesh/indexedMesh.h"
#include "../physics/geometry/shapeClass.h"

#include <stdexcept>

#define PI 3.14159265359

namespace Engine::MeshRegistry {

std::vector<Graphics::IndexedMesh*> meshes;
std::map<const ShapeClass*, VisualData> shapeClassMeshIds;
VisualData box;
VisualData sphere;
VisualData cylinder;

// Generates a cylinder with 
Graphics::VisualShape createCylinder(int sides, double radius, double height) {
	if(sides < 2) { throw std::logic_error("Cannot create cylinder with <2 sides"); }
	int vertexCount = sides * 4;
	Vec3f* vertexBuffer = new Vec3f[vertexCount];


	// vertices
	for(int i = 0; i < sides; i++) {
		double angle = i * PI * 2 / sides;
		Vec3f bottom(cos(angle) * radius, sin(angle) * radius, height / 2);
		Vec3f top(cos(angle) * radius, sin(angle) * radius, -height / 2);
		vertexBuffer[i * 2] = bottom;
		vertexBuffer[i * 2 + 1] = top;
		vertexBuffer[i * 2 + sides * 2] = bottom;
		vertexBuffer[i * 2 + 1 + sides * 2] = top;
	}

	int triangleCount = sides * 2 + (sides - 2) * 2;
	Triangle* triangleBuffer = new Triangle[triangleCount];

	// sides
	for(int i = 0; i < sides; i++) {
		int bottomLeft = i * 2;
		int bottomRight = ((i + 1) % sides) * 2;
		triangleBuffer[i * 2] = Triangle{bottomLeft, bottomLeft + 1, bottomRight}; // botLeft, botRight, topLeft
		triangleBuffer[i * 2 + 1] = Triangle{bottomRight + 1, bottomRight, bottomLeft + 1}; // topRight, topLeft, botRight
	}

	Triangle* capOffset = triangleBuffer + sides * 2;
	// top and bottom
	for(int i = 0; i < sides - 2; i++) { // common corner is i=0
		capOffset[i] = Triangle{sides * 2 + 0, sides * 2 + (i + 1) * 2, sides * 2 + (i + 2) * 2};
		capOffset[i + (sides - 2)] = Triangle{sides * 2 + 1, sides * 2 + (i + 2) * 2 + 1, sides * 2 + (i + 1) * 2 + 1};
	}

	Vec3f* normalBuffer = new Vec3f[vertexCount];
	for (int i = 0; i < sides * 2; i++) {
		Vec3f vertex = vertexBuffer[i];
		normalBuffer[i] = normalize(Vec3(vertex.x, vertex.y, 0));
	}

	for(int i = 0; i < sides; i++) {
		normalBuffer[i * 2 + sides * 2] = Vec3f(0, 0, 1);
		normalBuffer[i * 2 + sides * 2 + 1] = Vec3f(0, 0, -1);
	}

	Vec2f* uvBuffer = new Vec2f[vertexCount];
	for (int i = 0; i < vertexCount; i++) {
		Vec3f& p = vertexBuffer[i];

		float u = 0;
		float v = p.y / height;
		if (p.x >= 0) {
			if (p.x == 0)
				u = asin(p.y / sqrt(p.x * p.x + p.y * p.y));
			else
				u = atan(p.y / p.x);
		} else
			u = -asin(p.y / sqrt(p.x * p.x + p.y * p.y)) + PI;

		uvBuffer[i] = Vec2f(u, v);
	}

	return Graphics::VisualShape(vertexBuffer, vertexCount, triangleBuffer, triangleCount, SharedArrayPtr<const Vec3f>(normalBuffer), SharedArrayPtr<const Vec2f>(uvBuffer));
}

Graphics::VisualShape createSphere(double radius, int steps) {
	Polyhedron sphere(Library::createSphere(radius, steps));
	Graphics::VisualShape sphereShape = Graphics::VisualShape(sphere);

	Vec3f* normalBuffer = new Vec3f[sphereShape.vertexCount];
	Vec2f* uvBuffer = new Vec2f[sphereShape.vertexCount];
	int i = 0;
	for (Vec3f vertex : sphereShape.iterVertices()) {
		Vec3f normal = normalize(vertex);
		Vec2f uv = Vec2f(atan2(normal.x, normal.z) / (2 * PI) + 0.5, normal.y * 0.5 + 0.5);

		normalBuffer[i] = normal;
		uvBuffer[i] = uv;

		i++;
	}

	sphereShape.normals = SharedArrayPtr<const Vec3f>(normalBuffer);
	sphereShape.uvs = SharedArrayPtr<const Vec2f>(uvBuffer);
	return sphereShape;
}

Graphics::VisualShape createBox(float width, float height, float depth) {
	Polyhedron box(Library::createBox(width, height, depth));
	Graphics::VisualShape boxShape = Graphics::VisualShape(box);

	Vec3f* normalBuffer = new Vec3f[boxShape.vertexCount];
	boxShape.computeNormals(normalBuffer);

	boxShape.normals = SharedArrayPtr<const Vec3f>(normalBuffer);
	return boxShape;
}

Graphics::VisualShape createCube(float size) {
	return createBox(size, size, size);
}

void init() {
	sphere = registerMeshFor(sphereClass, Graphics::VisualShape::generateSmoothNormalsShape(sphereClass->asPolyhedron()));
	box = registerMeshFor(boxClass);
	Graphics::VisualShape cylinderShape = createCylinder(64, 1.0, 2.0);
	cylinder = registerMeshFor(cylinderClass, cylinderShape);
}


VisualData addMeshShape(const Graphics::VisualShape& shape) {
	using namespace Graphics;

	int size = (int) meshes.size();
	
	IndexedMesh* mesh = new IndexedMesh(shape);

	// Todo move somewhere else
	VertexBuffer* uniformBuffer = new VertexBuffer(nullptr, 0, Graphics::Renderer::STREAM_DRAW);
	BufferLayout uniformLayout = BufferLayout(
		{
			BufferElement("vModelMatrix", BufferDataType::MAT4, true),
			BufferElement("vAlbedo", BufferDataType::FLOAT4, true),
			BufferElement("vMRAo", BufferDataType::FLOAT3, true),
		}
	);
	mesh->addUniformBuffer(uniformBuffer, uniformLayout);

	meshes.push_back(mesh);

	return VisualData { size, shape.uvs != nullptr, shape.normals != nullptr };
}

VisualData registerMeshFor(const ShapeClass* shapeClass, const Graphics::VisualShape& mesh) {
	if(shapeClassMeshIds.find(shapeClass) != shapeClassMeshIds.end()) throw "Attempting to re-register existing ShapeClass!";

	VisualData meshData = addMeshShape(mesh);

	//Log::error("Mesh %d registered!", meshData);

	shapeClassMeshIds.insert(std::pair<const ShapeClass*, VisualData>(shapeClass, meshData));
	return meshData;
}

VisualData registerMeshFor(const ShapeClass* shapeClass) {
	Graphics::VisualShape shape = Graphics::VisualShape::generateSplitNormalsShape(shapeClass->asPolyhedron());
	return registerMeshFor(shapeClass, shape);
}

VisualData getOrCreateMeshFor(const ShapeClass* shapeClass) {
	auto found = shapeClassMeshIds.find(shapeClass);

	if(found != shapeClassMeshIds.end()) {
		// mesh found!
		VisualData meshData = (*found).second;
		//Log::error("Mesh %d reused!", meshData);
		return meshData;
	} else {
		// mesh not found :(
		return registerMeshFor(shapeClass);
	}
}


};
