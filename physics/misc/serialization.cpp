#include "serialization.h"

#include "dynamicSerialize.h"

#include "../geometry/polyhedron.h"
#include "../geometry/polyhedronInternals.h"
#include "../geometry/shape.h"
#include "../geometry/shapeClass.h"
#include "../part.h"
#include "../world.h"

#include "../constraints/hardConstraint.h"
#include "../constraints/fixedConstraint.h"
#include "../constraints/motorConstraint.h"

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

void serializePolyhedron(const Polyhedron& poly, std::ostream& ostream) {
	::serialize<int>(poly.vertexCount, ostream);
	::serialize<int>(poly.triangleCount, ostream);

	for(int i = 0; i < poly.vertexCount; i++) {
		::serialize<Vec3f>(poly[i], ostream);
	}
	for(int i = 0; i < poly.triangleCount; i++) {
		::serialize<Triangle>(poly.getTriangle(i), ostream);
	}
}
Polyhedron deserializePolyhedron(std::istream& istream) {
	int vertexCount = ::deserialize<int>(istream);
	int triangleCount = ::deserialize<int>(istream);

	Vec3f* vertices = new Vec3f[vertexCount];
	Triangle* triangles = new Triangle[triangleCount];

	for(int i = 0; i < vertexCount; i++) {
		vertices[i] = ::deserialize<Vec3f>(istream);
	}
	for(int i = 0; i < triangleCount; i++) {
		triangles[i] = ::deserialize<Triangle>(istream);
	}

	Polyhedron result(vertices, triangles, vertexCount, triangleCount);
	delete[] vertices;
	delete[] triangles;
	return result;
}

void serializeShape(const Shape& shape, std::ostream& ostream) {
	::serialize<int>(shape.baseShape->classID, ostream);
	::serialize<double>(shape.getWidth(), ostream);
	::serialize<double>(shape.getHeight(), ostream);
	::serialize<double>(shape.getDepth(), ostream);
}
Shape deserializeShape(std::istream& istream) {
	int shapeId = ::deserialize<int>(istream);
	const ShapeClass* shapeClass = ShapeClass::getShapeClassForId(shapeId);
	if(shapeClass == nullptr) {
		throw SerializationException("Unknown ShapeClass!");
	}
	double width = ::deserialize<double>(istream);
	double height = ::deserialize<double>(istream);
	double depth = ::deserialize<double>(istream);
	return Shape(shapeClass, width, height, depth);
}

void serializePart(const Part& part, std::ostream& ostream) {
	::serializeShape(part.hitbox, ostream);
	::serialize<PartProperties>(part.properties, ostream);
}

Part deserializePart(std::istream& istream) {
	Shape shape = ::deserializeShape(istream);
	PartProperties properties = ::deserialize<PartProperties>(istream);
	return Part(shape, GlobalCFrame(), properties);
}

void PartSerializer::serializePart(const Part& part, std::ostream& ostream) {
	::serializePart(part, ostream);
}
Part* PartSerializer::deserializePart(std::istream& istream) {
	return new Part(::deserializePart(istream));
}

static void serializeRigidBody(const RigidBody& rigidBody, std::ostream& ostream, PartSerializer& partSerializer) {
	partSerializer.serializePart(*rigidBody.mainPart, ostream);
	::serialize<unsigned int>(rigidBody.parts.size(), ostream);
	for(const AttachedPart& atPart : rigidBody.parts) {
		::serialize<CFrame>(atPart.attachment, ostream);
		partSerializer.serializePart(*atPart.part , ostream);
	}
}

static RigidBody deserializeRigidBody(std::istream& istream, PartSerializer& partSerializer) {
	Part* mainPart = partSerializer.deserializePart(istream);
	unsigned int size = ::deserialize<unsigned int>(istream);
	RigidBody result(mainPart);
	result.parts.reserve(size);
	for(size_t i = 0; i < size; i++) {
		CFrame attach = ::deserialize<CFrame>(istream);
		Part* newPart = partSerializer.deserializePart(istream);
		result.parts.push_back(AttachedPart{attach, newPart});
	}
	return result;
}

static void serializeHardPhysicalConnection(const HardPhysicalConnection& connection, std::ostream& ostream) {
	::serialize<CFrame>(connection.attachOnChild, ostream);
	::serialize<CFrame>(connection.attachOnParent, ostream);

	hardConstraintRegistry.serialize(*connection.constraintWithParent, ostream);
}

static HardPhysicalConnection deserializeHardPhysicalConnection(std::istream& istream) {
	CFrame attachOnChild = ::deserialize<CFrame>(istream);
	CFrame attachOnParent = ::deserialize<CFrame>(istream);

	HardConstraint* constraint = hardConstraintRegistry.deserialize(istream);

	return HardPhysicalConnection(attachOnChild, attachOnParent, std::unique_ptr<HardConstraint>(constraint));
}



void serializeConnectionsFromPhysical(const Physical& phys, std::ostream& ostream, PartSerializer& partSerializer) {
	serializeRigidBody(phys.rigidBody, ostream, partSerializer);
	::serialize<unsigned int>(phys.childPhysicals.size(), ostream);
	for(const ConnectedPhysical& p : phys.childPhysicals) {
		serializeHardPhysicalConnection(p.connectionToParent, ostream);
		serializeConnectionsFromPhysical(p, ostream, partSerializer);
	}
}

void serializeMotorizedPhysical(const MotorizedPhysical& phys, std::ostream& ostream, PartSerializer& partSerializer) {
	::serialize<Motion>(phys.motionOfCenterOfMass, ostream);
	::serialize<GlobalCFrame>(phys.getMainPart()->getCFrame(), ostream);

	serializeConnectionsFromPhysical(phys, ostream, partSerializer);
}

static void deserializeConnectionsOfPhysical(Physical& physToPopulate, std::istream& istream, PartSerializer& partSerializer) {
	unsigned int childrenCount = ::deserialize<unsigned int>(istream);
	physToPopulate.childPhysicals.reserve(childrenCount);
	for(size_t i = 0; i < childrenCount; i++) {
		HardPhysicalConnection connection = deserializeHardPhysicalConnection(istream);
		RigidBody b = deserializeRigidBody(istream, partSerializer);
		physToPopulate.childPhysicals.push_back(ConnectedPhysical(std::move(b), &physToPopulate, std::move(connection)));
		ConnectedPhysical& currentlyWorkingOn = physToPopulate.childPhysicals.back();
		deserializeConnectionsOfPhysical(currentlyWorkingOn, istream, partSerializer);
	}
}

MotorizedPhysical* deserializeMotorizedPhysical(std::istream& istream, PartSerializer& partSerializer) {
	Motion motion = ::deserialize<Motion>(istream);
	GlobalCFrame cf = ::deserialize<GlobalCFrame>(istream);
	RigidBody r = deserializeRigidBody(istream, partSerializer);
	r.setCFrame(cf);
	MotorizedPhysical* mainPhys = new MotorizedPhysical(std::move(r));
	mainPhys->motionOfCenterOfMass = motion;

	deserializeConnectionsOfPhysical(*mainPhys, istream, partSerializer);

	mainPhys->fullRefreshOfConnectedPhysicals();
	mainPhys->refreshPhysicalProperties();
	return mainPhys;
}


void serializeWorldPrototype(const WorldPrototype& world, std::ostream& ostream, PartSerializer& partSerializer) {
	size_t numberOfPhysicals = world.physicals.size();
	::serialize<size_t>(numberOfPhysicals, ostream);
	
	size_t partCount = 0;
	for(const Part& p : world.iterParts(TERRAIN_PARTS)) {
		partCount++;
	}

	::serialize<size_t>(partCount, ostream);

	for(const MotorizedPhysical* p : world.physicals) {
		::serializeMotorizedPhysical(*p, ostream, partSerializer);
	}

	for(const Part& p : world.iterParts(TERRAIN_PARTS)) {
		::serialize<GlobalCFrame>(p.getCFrame(), ostream);
		partSerializer.serializePart(p, ostream);
	}
}

void deserializeWorldPrototype(WorldPrototype& world, std::istream& istream, PartSerializer& partSerializer) {
	size_t numberOfPhysicals = ::deserialize<size_t>(istream);
	size_t numberOfTerrainParts = ::deserialize<size_t>(istream);
	world.physicals.reserve(numberOfPhysicals);

	for(size_t i = 0; i < numberOfPhysicals; i++) {
		MotorizedPhysical* p = ::deserializeMotorizedPhysical(istream, partSerializer);
		world.addPart(p->getMainPart());
	}

	for(size_t i = 0; i < numberOfTerrainParts; i++) {
		GlobalCFrame cf = ::deserialize<GlobalCFrame>(istream);
		Part* p = partSerializer.deserializePart(istream);
		p->setCFrame(cf);
		world.addTerrainPart(p);
	}
}

