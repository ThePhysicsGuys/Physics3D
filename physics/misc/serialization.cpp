#include "serialization.h"

#include <map>
#include <set>
#include <limits.h>
#include <string>

#include "../geometry/polyhedron.h"
#include "../geometry/normalizedPolyhedron.h"
#include "../geometry/polyhedronInternals.h"
#include "../geometry/shape.h"
#include "../geometry/shapeClass.h"
#include "../part.h"
#include "../world.h"
#include "../constraints/hardConstraint.h"
#include "../constraints/fixedConstraint.h"
#include "../constraints/motorConstraint.h"

#pragma region serializeComponents

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

void serializeFixedConstraint(const FixedConstraint& object, std::ostream& ostream) {}
FixedConstraint* deserializeFixedConstraint(std::istream& istream) { return new FixedConstraint(); }

void serializeMotorConstraint(const MotorConstraint& constraint, std::ostream& ostream) {
	::serialize<Vec3>(constraint.getAngularVelocity(), ostream);
	::serialize<double>(constraint.getCurrentAngle(), ostream);
}
MotorConstraint* deserializeMotorConstraint(std::istream& istream) {
	Vec3 angularVelocity = ::deserialize<Vec3>(istream);
	double currentAngle = ::deserialize<double>(istream);

	return new MotorConstraint(angularVelocity, currentAngle);
}

void serializeNormalizedPolyhedron(const NormalizedPolyhedron& polyhedron, std::ostream& ostream) {
	::serializePolyhedron(polyhedron, ostream);
}
NormalizedPolyhedron* deserializeNormalizedPolyhedron(std::istream& istream) {
	Polyhedron poly = ::deserializePolyhedron(istream);
	NormalizedPolyhedron* result = new NormalizedPolyhedron(poly.normalized());
	return result;
}

#pragma endregion

#pragma region serializePartPhysicalAndRelated

void serializePartWithoutCFrame(const Part& part, std::ostream& ostream, const std::map<const ShapeClass*, ShapeClassID>& classToIDMap) {
	::serializeShape(part.hitbox, ostream, classToIDMap);
	::serialize<PartProperties>(part.properties, ostream);
}

void serializePartWithCFrame(const Part& part, std::ostream& ostream, const std::map<const ShapeClass*, ShapeClassID>& classToIDMap) {
	::serialize<GlobalCFrame>(part.getCFrame(), ostream);
	serializePartWithoutCFrame(part, ostream, classToIDMap);
}

Part deserializePartWithoutCFrame(std::istream& istream, const std::map<ShapeClassID, const ShapeClass*>& IDToClassMap) {
	Shape shape = ::deserializeShape(istream, IDToClassMap);
	PartProperties properties = ::deserialize<PartProperties>(istream);
	return Part(shape, GlobalCFrame(), properties);
}

Part deserializePartWithCFrame(std::istream& istream, const std::map<ShapeClassID, const ShapeClass*>& IDToClassMap) {
	GlobalCFrame cf = ::deserialize<GlobalCFrame>(istream);
	Shape shape = ::deserializeShape(istream, IDToClassMap);
	PartProperties properties = ::deserialize<PartProperties>(istream);
	return Part(shape, cf, properties);
}

void SerializationSessionPrototype::serializePart(const Part& part, std::ostream& ostream) {
	::serializePartWithoutCFrame(part, ostream, this->shapeClassToIDMap);
}
Part* DeSerializationSessionPrototype::deserializePart(std::istream& istream) {
	return new Part(::deserializePartWithoutCFrame(istream, this->IDToShapeClassMap));
}

void SerializationSessionPrototype::serializeRigidBody(const RigidBody& rigidBody, std::ostream& ostream) {
	serializePart(*rigidBody.mainPart, ostream);
	::serialize<unsigned int>(static_cast<unsigned int>(rigidBody.parts.size()), ostream);
	for(const AttachedPart& atPart : rigidBody.parts) {
		::serialize<CFrame>(atPart.attachment, ostream);
		serializePart(*atPart.part, ostream);
	}
}

RigidBody DeSerializationSessionPrototype::deserializeRigidBody(std::istream& istream) {
	Part* mainPart = deserializePart(istream);
	unsigned int size = ::deserialize<unsigned int>(istream);
	RigidBody result(mainPart);
	result.parts.reserve(size);
	for(size_t i = 0; i < size; i++) {
		CFrame attach = ::deserialize<CFrame>(istream);
		Part* newPart = deserializePart(istream);
		result.parts.push_back(AttachedPart{attach, newPart});
	}
	return result;
}

static void serializeHardPhysicalConnection(const HardPhysicalConnection& connection, std::ostream& ostream) {
	::serialize<CFrame>(connection.attachOnChild, ostream);
	::serialize<CFrame>(connection.attachOnParent, ostream);

	dynamicHardConstraintSerializer.serialize(*connection.constraintWithParent, ostream);
}

static HardPhysicalConnection deserializeHardPhysicalConnection(std::istream& istream) {
	CFrame attachOnChild = ::deserialize<CFrame>(istream);
	CFrame attachOnParent = ::deserialize<CFrame>(istream);

	HardConstraint* constraint = dynamicHardConstraintSerializer.deserialize(istream);

	return HardPhysicalConnection(attachOnChild, attachOnParent, std::unique_ptr<HardConstraint>(constraint));
}



void SerializationSessionPrototype::serializePhysical(const Physical& phys, std::ostream& ostream) {
	serializeRigidBody(phys.rigidBody, ostream);
	::serialize<unsigned int>(static_cast<unsigned int>(phys.childPhysicals.size()), ostream);
	for(const ConnectedPhysical& p : phys.childPhysicals) {
		serializeHardPhysicalConnection(p.connectionToParent, ostream);
		serializePhysical(p, ostream);
	}
}

void SerializationSessionPrototype::serializeMotorizedPhysical(const MotorizedPhysical& phys, std::ostream& ostream) {
	::serialize<Motion>(phys.motionOfCenterOfMass, ostream);
	::serialize<GlobalCFrame>(phys.getMainPart()->getCFrame(), ostream);

	serializePhysical(phys, ostream);
}

void DeSerializationSessionPrototype::deserializeConnectionsOfPhysical(Physical& physToPopulate, std::istream& istream) {
	unsigned int childrenCount = ::deserialize<unsigned int>(istream);
	physToPopulate.childPhysicals.reserve(childrenCount);
	for(size_t i = 0; i < childrenCount; i++) {
		HardPhysicalConnection connection = deserializeHardPhysicalConnection(istream);
		RigidBody b = deserializeRigidBody(istream);
		physToPopulate.childPhysicals.push_back(ConnectedPhysical(std::move(b), &physToPopulate, std::move(connection)));
		ConnectedPhysical& currentlyWorkingOn = physToPopulate.childPhysicals.back();
		deserializeConnectionsOfPhysical(currentlyWorkingOn, istream);
	}
}

MotorizedPhysical* DeSerializationSessionPrototype::deserializeMotorizedPhysical(std::istream& istream) {
	Motion motion = ::deserialize<Motion>(istream);
	GlobalCFrame cf = ::deserialize<GlobalCFrame>(istream);
	RigidBody r = deserializeRigidBody(istream);
	r.setCFrame(cf);
	MotorizedPhysical* mainPhys = new MotorizedPhysical(std::move(r));
	mainPhys->motionOfCenterOfMass = motion;

	deserializeConnectionsOfPhysical(*mainPhys, istream);

	mainPhys->fullRefreshOfConnectedPhysicals();
	mainPhys->refreshPhysicalProperties();
	return mainPhys;
}

#pragma endregion

#pragma region serializeWorld

#pragma region information collection

void SerializationSessionPrototype::collectPartInformation(const Part& part) {
	const ShapeClass* shape = part.hitbox.baseShape;
	if(shapeClassToIDMap.find(shape) == shapeClassToIDMap.end()) {
		shapeClassesToSerialize.emplace(shape);
	}
}

void SerializationSessionPrototype::collectPhysicalInformation(const Physical& phys) {
	for(const Part& p : phys.rigidBody) {
		collectPartInformation(p);
	}

	for(const ConnectedPhysical& p : phys.childPhysicals) {
		collectConnectedPhysicalInformation(p);
	}
}

void SerializationSessionPrototype::collectMotorizedPhysicalInformation(const MotorizedPhysical& motorizedPhys) {
	collectPhysicalInformation(motorizedPhys);
}
void SerializationSessionPrototype::collectConnectedPhysicalInformation(const ConnectedPhysical& connectedPhys) {
	collectPhysicalInformation(connectedPhys);
}

#pragma endregion

void SerializationSessionPrototype::serializeWorld(const WorldPrototype& world, std::ostream& ostream) {
	
	for(const MotorizedPhysical* p : world.physicals) {
		collectMotorizedPhysicalInformation(*p);
	}
	for(const Part& p : world.iterParts(TERRAIN_PARTS)) {
		collectPartInformation(p);
	}

	serializeCollectedHeaderInformation(ostream);


	// actually serialize the world
	size_t physicalCount = world.physicals.size();
	::serialize<size_t>(physicalCount, ostream);

	size_t partCount = 0;
	for(const Part& p : world.iterParts(TERRAIN_PARTS)) {
		partCount++;
	}

	::serialize<size_t>(partCount, ostream);

	for(const MotorizedPhysical* p : world.physicals) {
		serializeMotorizedPhysical(*p, ostream);
	}

	for(const Part& p : world.iterParts(TERRAIN_PARTS)) {
		::serialize<GlobalCFrame>(p.getCFrame(), ostream);
		serializePart(p, ostream);
	}
}

void DeSerializationSessionPrototype::deserializeWorld(WorldPrototype& world, std::istream& istream) {
	this->deserializeAndCollectHeaderInformation(istream);



	size_t numberOfPhysicals = ::deserialize<size_t>(istream);
	size_t numberOfTerrainParts = ::deserialize<size_t>(istream);
	world.physicals.reserve(numberOfPhysicals);

	for(size_t i = 0; i < numberOfPhysicals; i++) {
		MotorizedPhysical* p = deserializeMotorizedPhysical(istream);
		world.addPart(p->getMainPart());
	}

	for(size_t i = 0; i < numberOfTerrainParts; i++) {
		GlobalCFrame cf = ::deserialize<GlobalCFrame>(istream);
		Part* p = deserializePart(istream);
		p->setCFrame(cf);
		world.addTerrainPart(p);
	}
}



void SerializationSessionPrototype::serializeCollectedHeaderInformation(std::ostream& ostream) {
	::serialize<ShapeClassID>(static_cast<unsigned int>(this->shapeClassesToSerialize.size()), ostream);

	ShapeClassID currentID = UINT_MAX;
	for(const ShapeClass* sc : shapeClassesToSerialize) {
		this->shapeClassToIDMap.emplace(sc, currentID);

		::serialize<ShapeClassID>(currentID, ostream);
		currentID--;
		dynamicShapeClassSerializer.serialize(*sc, ostream);
	}
}

void DeSerializationSessionPrototype::deserializeAndCollectHeaderInformation(std::istream& istream) {
	ShapeClassID shapeClassCount = ::deserialize<ShapeClassID>(istream);

	for(ShapeClassID i = 0; i < shapeClassCount; i++) {
		ShapeClassID shapeClassID = ::deserialize<ShapeClassID>(istream);
		const ShapeClass* newShapeClass = dynamicShapeClassSerializer.deserialize(istream);

		if(this->IDToShapeClassMap.emplace(shapeClassID, newShapeClass).second == false) {
			throw SerializationException("This shapeClass serialization ID is already in use! " + std::to_string(shapeClassID));
		}
	}
}

static const ShapeClass* builtinKnownShapeClasses[]{boxClass, sphereClass, cylinderClass};
SerializationSessionPrototype::SerializationSessionPrototype(const std::vector<const ShapeClass*>& knownShapeClasses) {
	unsigned int i = 0;
	for(const ShapeClass* s : builtinKnownShapeClasses) {
		shapeClassToIDMap.emplace(s, i);
		i++;
	}
	for(const ShapeClass* s : knownShapeClasses) {
		shapeClassToIDMap.emplace(s, i);
		i++;
	}
}

DeSerializationSessionPrototype::DeSerializationSessionPrototype(const std::vector<const ShapeClass*>& knownShapeClasses) {
	unsigned int i = 0;
	for(const ShapeClass* s : builtinKnownShapeClasses) {
		IDToShapeClassMap.emplace(i, s);
		i++;
	}
	for(const ShapeClass* s : knownShapeClasses) {
		IDToShapeClassMap.emplace(i, s);
		i++;
	}
}

#pragma endregion

#pragma region dynamic serializers

static DynamicSerializerRegistry<HardConstraint>::ConcreteDynamicSerializer<FixedConstraint> fixedConstraintSerializer
(serializeFixedConstraint, deserializeFixedConstraint, 0);
static DynamicSerializerRegistry<HardConstraint>::ConcreteDynamicSerializer<MotorConstraint> motorConstraintSerializer
(serializeMotorConstraint, deserializeMotorConstraint, 1);

static DynamicSerializerRegistry<ShapeClass>::ConcreteDynamicSerializer<NormalizedPolyhedron> polyhedronSerializer
(serializeNormalizedPolyhedron, deserializeNormalizedPolyhedron, 0);

DynamicSerializerRegistry<HardConstraint> dynamicHardConstraintSerializer{
	{typeid(FixedConstraint), &fixedConstraintSerializer},
	{typeid(MotorConstraint), &motorConstraintSerializer}
};
DynamicSerializerRegistry<ShapeClass> dynamicShapeClassSerializer{
	{typeid(NormalizedPolyhedron), &polyhedronSerializer},
};

#pragma endregion
