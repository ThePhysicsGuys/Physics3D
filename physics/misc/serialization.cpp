#include "serialization.h"

#include <map>
#include <set>
#include <limits.h>

#include "dynamicSerialize.h"
#include "partSerializationInformation.h"

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

void serializeShape(const Shape& shape, std::ostream& ostream, const std::map<const ShapeClass*, ShapeClassID>& registry) {
	auto found = registry.find(shape.baseShape);
	if(found == registry.end()) throw SerializationException("ShapeClass " + std::to_string(reinterpret_cast<size_t>(shape.baseShape)) + " was not registered!");

	::serialize<ShapeClassID>((*found).second, ostream);
	::serialize<double>(shape.getWidth(), ostream);
	::serialize<double>(shape.getHeight(), ostream);
	::serialize<double>(shape.getDepth(), ostream);
}
Shape deserializeShape(std::istream& istream, const std::map<ShapeClassID, const ShapeClass*>& registry) {
	ShapeClassID shapeId = ::deserialize<ShapeClassID>(istream);

	auto found = registry.find(shapeId);
	if(found == registry.end()) throw SerializationException("There was no ShapeClass found for ID " + std::to_string(shapeId));


	const ShapeClass* shapeClass = (*found).second;
	if(shapeClass == nullptr) {
		throw SerializationException("Unknown ShapeClass!" + std::to_string(shapeId));
	}
	double width = ::deserialize<double>(istream);
	double height = ::deserialize<double>(istream);
	double depth = ::deserialize<double>(istream);
	return Shape(shapeClass, width, height, depth);
}

void serializePartWithoutCFrame(const Part& part, std::ostream& ostream, PartSerializationInformation& serializationInformation) {
	::serializeShape(part.hitbox, ostream, serializationInformation.shapeClassRegistry);
	::serialize<PartProperties>(part.properties, ostream);
}

void serializePartWithCFrame(const Part& part, std::ostream& ostream, PartSerializationInformation& serializationInformation) {
	::serialize<GlobalCFrame>(part.getCFrame(), ostream);
	::serializePartWithoutCFrame(part, ostream, serializationInformation);
}

Part deserializePartWithoutCFrame(std::istream& istream, PartDeSerializationInformation& deserializationInformation) {
	Shape shape = ::deserializeShape(istream, deserializationInformation.shapeClassRegistry);
	PartProperties properties = ::deserialize<PartProperties>(istream);
	return Part(shape, GlobalCFrame(), properties);
}

Part deserializePartWithCFrame(std::istream& istream, PartDeSerializationInformation& deserializationInformation) {
	GlobalCFrame cf = ::deserialize<GlobalCFrame>(istream);
	Shape shape = ::deserializeShape(istream, deserializationInformation.shapeClassRegistry);
	PartProperties properties = ::deserialize<PartProperties>(istream);
	return Part(shape, cf, properties);
}

void PartSerializer::serializePart(const Part& part, std::ostream& ostream, PartSerializationInformation& serializationInformation) {
	::serializePartWithoutCFrame(part, ostream, serializationInformation);
}
Part* PartSerializer::deserializePart(std::istream& istream, PartDeSerializationInformation& deserializationInformation) {
	return new Part(::deserializePartWithoutCFrame(istream, deserializationInformation));
}

static void serializeRigidBody(const RigidBody& rigidBody, std::ostream& ostream, PartSerializer& partSerializer, PartSerializationInformation& serializationInformation) {
	partSerializer.serializePart(*rigidBody.mainPart, ostream, serializationInformation);
	::serialize<unsigned int>(rigidBody.parts.size(), ostream);
	for(const AttachedPart& atPart : rigidBody.parts) {
		::serialize<CFrame>(atPart.attachment, ostream);
		partSerializer.serializePart(*atPart.part , ostream, serializationInformation);
	}
}

static RigidBody deserializeRigidBody(std::istream& istream, PartSerializer& partSerializer, PartDeSerializationInformation& deserializationInformation) {
	Part* mainPart = partSerializer.deserializePart(istream, deserializationInformation);
	unsigned int size = ::deserialize<unsigned int>(istream);
	RigidBody result(mainPart);
	result.parts.reserve(size);
	for(size_t i = 0; i < size; i++) {
		CFrame attach = ::deserialize<CFrame>(istream);
		Part* newPart = partSerializer.deserializePart(istream, deserializationInformation);
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



void serializeConnectionsFromPhysical(const Physical& phys, std::ostream& ostream, PartSerializer& partSerializer, PartSerializationInformation& serializationInformation) {
	serializeRigidBody(phys.rigidBody, ostream, partSerializer, serializationInformation);
	::serialize<unsigned int>(phys.childPhysicals.size(), ostream);
	for(const ConnectedPhysical& p : phys.childPhysicals) {
		serializeHardPhysicalConnection(p.connectionToParent, ostream);
		serializeConnectionsFromPhysical(p, ostream, partSerializer, serializationInformation);
	}
}

void serializeMotorizedPhysical(const MotorizedPhysical& phys, std::ostream& ostream, PartSerializer& partSerializer, PartSerializationInformation& serializationInformation) {
	::serialize<Motion>(phys.motionOfCenterOfMass, ostream);
	::serialize<GlobalCFrame>(phys.getMainPart()->getCFrame(), ostream);

	serializeConnectionsFromPhysical(phys, ostream, partSerializer, serializationInformation);
}

static void deserializeConnectionsOfPhysical(Physical& physToPopulate, std::istream& istream, PartSerializer& partSerializer, PartDeSerializationInformation& deserializationInformation) {
	unsigned int childrenCount = ::deserialize<unsigned int>(istream);
	physToPopulate.childPhysicals.reserve(childrenCount);
	for(size_t i = 0; i < childrenCount; i++) {
		HardPhysicalConnection connection = deserializeHardPhysicalConnection(istream);
		RigidBody b = deserializeRigidBody(istream, partSerializer, deserializationInformation);
		physToPopulate.childPhysicals.push_back(ConnectedPhysical(std::move(b), &physToPopulate, std::move(connection)));
		ConnectedPhysical& currentlyWorkingOn = physToPopulate.childPhysicals.back();
		deserializeConnectionsOfPhysical(currentlyWorkingOn, istream, partSerializer, deserializationInformation);
	}
}

MotorizedPhysical* deserializeMotorizedPhysical(std::istream& istream, PartSerializer& partSerializer, PartDeSerializationInformation& deserializationInformation) {
	Motion motion = ::deserialize<Motion>(istream);
	GlobalCFrame cf = ::deserialize<GlobalCFrame>(istream);
	RigidBody r = deserializeRigidBody(istream, partSerializer, deserializationInformation);
	r.setCFrame(cf);
	MotorizedPhysical* mainPhys = new MotorizedPhysical(std::move(r));
	mainPhys->motionOfCenterOfMass = motion;

	deserializeConnectionsOfPhysical(*mainPhys, istream, partSerializer, deserializationInformation);

	mainPhys->fullRefreshOfConnectedPhysicals();
	mainPhys->refreshPhysicalProperties();
	return mainPhys;
}

static void findShapeClass(const Part& part, std::set<const ShapeClass*>& found, const std::map<const ShapeClass*, ShapeClassID>& ignoredShapeClasses) {
	const ShapeClass* shape = part.hitbox.baseShape;
	if(ignoredShapeClasses.find(shape) == ignoredShapeClasses.end()) {
		found.emplace(shape);
	}
}

static void findShapeClasses(const RigidBody& rb, std::set<const ShapeClass*>& found, const std::map<const ShapeClass*, ShapeClassID>& ignoredShapeClasses) {
	for(const Part& p : rb) {
		findShapeClass(p, found, ignoredShapeClasses);
	}
}

static void findShapeClasses(const Physical& phys, std::set<const ShapeClass*>& found, const std::map<const ShapeClass*, ShapeClassID>& ignoredShapeClasses) {
	findShapeClasses(phys.rigidBody, found, ignoredShapeClasses);

	for(const ConnectedPhysical& p : phys.childPhysicals) {
		findShapeClasses(p, found, ignoredShapeClasses);
	}
}

static std::set<const ShapeClass*> findShapeClasses(const WorldPrototype& world, const std::map<const ShapeClass*, ShapeClassID>& ignoredShapeClasses) {
	std::set<const ShapeClass*> result;

	for(const MotorizedPhysical* p : world.physicals) {
		findShapeClasses(*p, result, ignoredShapeClasses);
	}
	for(const Part& p : world.iterParts(TERRAIN_PARTS)) {
		findShapeClass(p, result, ignoredShapeClasses);
	}

	return result;
}

static void serializeWorldPrototype(const WorldPrototype& world, std::ostream& ostream, PartSerializer& partSerializer, PartSerializationInformation& serializationInfo) {
	size_t numberOfPhysicals = world.physicals.size();
	::serialize<size_t>(numberOfPhysicals, ostream);
	
	size_t partCount = 0;
	for(const Part& p : world.iterParts(TERRAIN_PARTS)) {
		partCount++;
	}

	::serialize<size_t>(partCount, ostream);

	for(const MotorizedPhysical* p : world.physicals) {
		::serializeMotorizedPhysical(*p, ostream, partSerializer, serializationInfo);
	}

	for(const Part& p : world.iterParts(TERRAIN_PARTS)) {
		::serialize<GlobalCFrame>(p.getCFrame(), ostream);
		partSerializer.serializePart(p, ostream, serializationInfo);
	}
}

void deserializeWorldPrototype(WorldPrototype& world, std::istream& istream, PartSerializer& partSerializer, PartDeSerializationInformation& deserializationInfo) {
	size_t numberOfPhysicals = ::deserialize<size_t>(istream);
	size_t numberOfTerrainParts = ::deserialize<size_t>(istream);
	world.physicals.reserve(numberOfPhysicals);

	for(size_t i = 0; i < numberOfPhysicals; i++) {
		MotorizedPhysical* p = ::deserializeMotorizedPhysical(istream, partSerializer, deserializationInfo);
		world.addPart(p->getMainPart());
	}

	for(size_t i = 0; i < numberOfTerrainParts; i++) {
		GlobalCFrame cf = ::deserialize<GlobalCFrame>(istream);
		Part* p = partSerializer.deserializePart(istream, deserializationInfo);
		p->setCFrame(cf);
		world.addTerrainPart(p);
	}
}


void serializeWorldPrototype(const WorldPrototype& world, std::ostream& ostream, PartSerializer& partSerializer) {

	std::map<const ShapeClass*, ShapeClassID> knownShapeClasses;
	for(ShapeClassID i = 0; i < builtinShapeClasses.size(); i++) {
		knownShapeClasses.emplace(builtinShapeClasses[i], i);
	}

	std::set<const ShapeClass*> shapeClassesToSerialize = findShapeClasses(world, knownShapeClasses);

	PartSerializationInformation info{knownShapeClasses};

	::serialize<ShapeClassID>(shapeClassesToSerialize.size(), ostream);

	//throw "TODO extract reading ShapeClasses and collecting information into it's own segment, to share for looseParts and singlePhysical";

	ShapeClassID currentID = UINT_MAX;
	for(const ShapeClass* sc : shapeClassesToSerialize) {
		info.shapeClassRegistry.emplace(sc, currentID);

		::serialize<ShapeClassID>(currentID, ostream);
		currentID--;
		shapeClassRegistry.serialize(*sc, ostream);
	}

	serializeWorldPrototype(world, ostream, partSerializer, info);
}

void deserializeWorldPrototype(WorldPrototype& world, std::istream& istream, PartSerializer& partSerializer) {
	
	std::map<ShapeClassID, const ShapeClass*> knownShapeClasses;

	for(ShapeClassID i = 0; i < builtinShapeClasses.size(); i++) {
		knownShapeClasses.emplace(i, builtinShapeClasses[i]);
	}


	ShapeClassID shapeClassCount = ::deserialize<ShapeClassID>(istream);

	for(ShapeClassID i = 0; i < shapeClassCount; i++) {
		ShapeClassID shapeClassID = ::deserialize<ShapeClassID>(istream);
		const ShapeClass* newShapeClass = shapeClassRegistry.deserialize(istream);

		if(knownShapeClasses.emplace(shapeClassID, newShapeClass).second == false) {
			throw SerializationException("This shapeClass serialization ID is already in use! " + std::to_string(shapeClassID));
		}
	}

	PartDeSerializationInformation info{knownShapeClasses};

	deserializeWorldPrototype(world, istream, partSerializer, info);
}

std::vector<const ShapeClass*> builtinShapeClasses{boxClass, sphereClass, cylinderClass};
