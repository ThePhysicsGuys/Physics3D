#include "serialization.h"
#include "../geometry/polyhedron.h"
#include "../geometry/polyhedronInternals.h"
#include "../part.h"
#include "../geometry/shape.h"

void serialize(const char* data, size_t size, std::ostream& ostream) {
	ostream.write(data, size);
}

void deserialize(char* buf, size_t size, std::istream& istream) {
	for(size_t i = 0; i < size; i++)
		buf[i] = istream.get();
}

template<>
void serialize<char>(const char& c, std::ostream& ostream) {
	ostream << c;
}
template<>
char deserialize<char>(std::istream& istream) {
	return istream.get();
}

template<>
void serialize<Polyhedron>(const Polyhedron& poly, std::ostream& ostream) {
	poly.serialize(ostream);
}
template<>
Polyhedron deserialize<Polyhedron>(std::istream& istream) {
	return Polyhedron::deserialize(istream);
}

void Polyhedron::serialize(std::ostream& ostream) const {
	::serialize<int>(this->vertexCount, ostream);
	::serialize<int>(this->triangleCount, ostream);

	size_t vertexOffset = getOffset(this->vertexCount);
	float* xPtr = this->vertices.get();
	float* yPtr = xPtr + vertexOffset;
	float* zPtr = yPtr + vertexOffset;

	serializeArray<float>(xPtr, this->vertexCount, ostream);
	serializeArray<float>(yPtr, this->vertexCount, ostream);
	serializeArray<float>(zPtr, this->vertexCount, ostream);

	size_t triangleOffset = getOffset(this->triangleCount);
	int* aPtr = this->triangles.get();
	int* bPtr = aPtr + triangleOffset;
	int* cPtr = bPtr + triangleOffset;

	serializeArray<int>(aPtr, this->triangleCount, ostream);
	serializeArray<int>(bPtr, this->triangleCount, ostream);
	serializeArray<int>(cPtr, this->triangleCount, ostream);
}

Polyhedron Polyhedron::deserialize(std::istream& istream) {
	int vertexCount = ::deserialize<int>(istream);
	int triangleCount = ::deserialize<int>(istream);

	size_t vertexOffset = getOffset(vertexCount);
	UniqueAlignedPointer<float> vecBuf = createParallelVecBuf(vertexCount);
	float* xPtr = vecBuf.get();
	float* yPtr = xPtr + vertexOffset;
	float* zPtr = yPtr + vertexOffset;

	deserializeArray<float>(xPtr, vertexCount, istream);
	deserializeArray<float>(yPtr, vertexCount, istream);
	deserializeArray<float>(zPtr, vertexCount, istream);

	size_t triangleOffset = getOffset(triangleCount);
	UniqueAlignedPointer<int> triangleBuf = createParallelTriangleBuf(triangleCount);
	int* aPtr = triangleBuf.get();
	int* bPtr = aPtr + triangleOffset;
	int* cPtr = bPtr + triangleOffset;

	deserializeArray<int>(aPtr, triangleCount, istream);
	deserializeArray<int>(bPtr, triangleCount, istream);
	deserializeArray<int>(cPtr, triangleCount, istream);

	return Polyhedron(std::move(vecBuf), std::move(triangleBuf), vertexCount, triangleCount);
}



void Shape::serialize(std::ostream& ostream) const {
	::serialize<int>(baseShape->classID, ostream);
	::serialize<DiagonalMat3>(scale, ostream);
}
Shape Shape::deserialize(std::istream& istream) {
	const ShapeClass* shapeClass = ShapeClass::getShapeClassForId(::deserialize<int>(istream));
	DiagonalMat3 scale = ::deserialize<DiagonalMat3>(istream);
	return Shape(shapeClass, scale);
}

template<>
void serialize<Shape>(const Shape& shape, std::ostream& ostream) {
	shape.serialize(ostream);
}
template<>
Shape deserialize<Shape>(std::istream& istream) {
	return Shape::deserialize(istream);
}

template<>
void serialize<Part>(const Part& part, std::ostream& ostream) {
	part.serialize(ostream);
}
template<>
Part deserialize<Part>(std::istream& istream) {
	return Part::deserialize(istream);
}

void Part::serializeCore(std::ostream& ostream) const {
	this->hitbox.serialize(ostream);
	::serialize<PartProperties>(this->properties, ostream);
}

/*
GlobalCFrame cframe;

public:
	bool isTerrainPart = false;
	Physical* parent = nullptr;
	Shape hitbox;
	double maxRadius;
	PartProperties properties;
	BoundingBox localBounds;
	Vec3 conveyorEffect; 
*/
void Part::serialize(std::ostream& ostream) const {
	::serialize<GlobalCFrame>(this->cframe, ostream);
	serializeCore(ostream);
}

void Part::serialize(std::ostream& ostream, const CFrame& attachment) const {
	::serialize<CFrame>(attachment, ostream);
	serializeCore(ostream);
}

Part Part::deserialize(std::istream& istream) {
	GlobalCFrame cframe = ::deserialize<GlobalCFrame>(istream);
	Shape shape = Shape::deserialize(istream);
	PartProperties properties = ::deserialize<PartProperties>(istream);
	return Part(shape, cframe, properties);
}

Part Part::deserialize(std::istream& istream, const GlobalCFrame& parentLocation) {
	GlobalCFrame cframe = parentLocation.localToGlobal(::deserialize<CFrame>(istream));
	Shape shape = Shape::deserialize(istream);
	PartProperties properties = ::deserialize<PartProperties>(istream);
	return Part(shape, cframe, properties);
}
