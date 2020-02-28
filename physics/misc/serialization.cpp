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
#include "../misc/gravityForce.h"


#define CURRENT_VERSION_ID 0

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
	uint32_t vertexCount = ::deserialize<uint32_t>(istream);
	uint32_t triangleCount = ::deserialize<uint32_t>(istream);

	Vec3f* vertices = new Vec3f[vertexCount];
	Triangle* triangles = new Triangle[triangleCount];

	for(uint32_t i = 0; i < vertexCount; i++) {
		vertices[i] = ::deserialize<Vec3f>(istream);
	}
	for(uint32_t i = 0; i < triangleCount; i++) {
		triangles[i] = ::deserialize<Triangle>(istream);
	}

	Polyhedron result(vertices, triangles, vertexCount, triangleCount);
	delete[] vertices;
	delete[] triangles;
	return result;
}

void ShapeSerializer::include(const Shape& shape) {
	sharedShapeClassSerializer.include(shape.baseShape);
}

void ShapeSerializer::serializeShape(const Shape& shape, std::ostream& ostream) const {
	sharedShapeClassSerializer.serializeIDFor(shape.baseShape, ostream);
	::serialize<double>(shape.getWidth(), ostream);
	::serialize<double>(shape.getHeight(), ostream);
	::serialize<double>(shape.getDepth(), ostream);
}

Shape ShapeDeserializer::deserializeShape(std::istream& istream) const {
	const ShapeClass* baseShape = sharedShapeClassDeserializer.deserializeObject(istream);
	double width = ::deserialize<double>(istream);
	double height = ::deserialize<double>(istream);
	double depth = ::deserialize<double>(istream);
	return Shape(baseShape, width, height, depth);
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

void serializeDirectionalGravity(const DirectionalGravity& gravity, std::ostream& ostream) {
	::serialize<Vec3>(gravity.gravity, ostream);
}
DirectionalGravity* deserializeDirectionalGravity(std::istream& istream) {
	Vec3 g = ::deserialize<Vec3>(istream);
	return new DirectionalGravity(g);
}

#pragma endregion

#pragma region serializePartPhysicalAndRelated


void SerializationSessionPrototype::serializeRawPartWithCFrame(const Part& part, std::ostream& ostream) const {
	::serialize<GlobalCFrame>(part.getCFrame(), ostream);
	this->serializeRawPartWithoutCFrame(part, ostream);
}
void SerializationSessionPrototype::serializeRawPartWithoutCFrame(const Part& part, std::ostream& ostream) const {
	shapeSerializer.serializeShape(part.hitbox, ostream);
	::serialize<PartProperties>(part.properties, ostream);
}
Part DeSerializationSessionPrototype::deserializeRawPart(const GlobalCFrame& cframe, std::istream& istream) const {
	Shape shape = shapeDeserializer.deserializeShape(istream);
	PartProperties properties = ::deserialize<PartProperties>(istream);
	return Part(shape, cframe, properties);
}

Part DeSerializationSessionPrototype::deserializeRawPartWithCFrame(std::istream& istream) const {
	GlobalCFrame cframe = ::deserialize<GlobalCFrame>(istream);
	return deserializeRawPart(cframe, istream);
}

void SerializationSessionPrototype::virtualSerializePart(const Part& part, std::ostream& ostream) {
	this->serializeRawPartWithoutCFrame(part, ostream);
}
Part* DeSerializationSessionPrototype::virtualDeserializePart(Part&& partPhysicalData, std::istream& istream) {
	return new Part(std::move(partPhysicalData));
}

void SerializationSessionPrototype::serializeRigidBodyInContext(const RigidBody& rigidBody, std::ostream& ostream) {
	virtualSerializePart(*rigidBody.mainPart, ostream);
	::serialize<uint32_t>(static_cast<uint32_t>(rigidBody.parts.size()), ostream);
	for(const AttachedPart& atPart : rigidBody.parts) {
		::serialize<CFrame>(atPart.attachment, ostream);
		virtualSerializePart(*atPart.part, ostream);
	}
}

RigidBody DeSerializationSessionPrototype::deserializeRigidBodyWithContext(std::istream& istream) {
	Part* mainPart = virtualDeserializePart(deserializeRawPart(GlobalCFrame(), istream), istream);
	uint32_t size = ::deserialize<uint32_t>(istream);
	RigidBody result(mainPart);
	result.parts.reserve(size);
	for(uint32_t i = 0; i < size; i++) {
		CFrame attach = ::deserialize<CFrame>(istream);
		Part* newPart = virtualDeserializePart(deserializeRawPart(GlobalCFrame(), istream), istream);
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

	return HardPhysicalConnection(std::unique_ptr<HardConstraint>(constraint), attachOnChild, attachOnParent);
}



void SerializationSessionPrototype::serializePhysicalInContext(const Physical& phys, std::ostream& ostream) {
	serializeRigidBodyInContext(phys.rigidBody, ostream);
	::serialize<uint32_t>(static_cast<uint32_t>(phys.childPhysicals.size()), ostream);
	for(const ConnectedPhysical& p : phys.childPhysicals) {
		serializeHardPhysicalConnection(p.connectionToParent, ostream);
		serializePhysicalInContext(p, ostream);
	}
}

void SerializationSessionPrototype::serializeMotorizedPhysicalInContext(const MotorizedPhysical& phys, std::ostream& ostream) {
	::serialize<Motion>(phys.motionOfCenterOfMass, ostream);
	::serialize<GlobalCFrame>(phys.getMainPart()->getCFrame(), ostream);

	serializePhysicalInContext(phys, ostream);
}

void DeSerializationSessionPrototype::deserializeConnectionsOfPhysicalWithContext(Physical& physToPopulate, std::istream& istream) {
	uint32_t childrenCount = ::deserialize<uint32_t>(istream);
	physToPopulate.childPhysicals.reserve(childrenCount);
	for(uint32_t i = 0; i < childrenCount; i++) {
		HardPhysicalConnection connection = deserializeHardPhysicalConnection(istream);
		RigidBody b = deserializeRigidBodyWithContext(istream);
		physToPopulate.childPhysicals.push_back(ConnectedPhysical(std::move(b), &physToPopulate, std::move(connection)));
		ConnectedPhysical& currentlyWorkingOn = physToPopulate.childPhysicals.back();
		deserializeConnectionsOfPhysicalWithContext(currentlyWorkingOn, istream);
	}
}

MotorizedPhysical* DeSerializationSessionPrototype::deserializeMotorizedPhysicalWithContext(std::istream& istream) {
	Motion motion = ::deserialize<Motion>(istream);
	GlobalCFrame cf = ::deserialize<GlobalCFrame>(istream);
	RigidBody r = deserializeRigidBodyWithContext(istream);
	r.setCFrame(cf);
	MotorizedPhysical* mainPhys = new MotorizedPhysical(std::move(r));
	mainPhys->motionOfCenterOfMass = motion;

	deserializeConnectionsOfPhysicalWithContext(*mainPhys, istream);

	mainPhys->fullRefreshOfConnectedPhysicals();
	mainPhys->refreshPhysicalProperties();
	return mainPhys;
}

#pragma endregion

#pragma region serializeWorld

#pragma region information collection

void SerializationSessionPrototype::collectPartInformation(const Part& part) {
	this->shapeSerializer.include(part.hitbox);
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
	
	::serialize<uint64_t>(world.externalForces.size(), ostream);
	for(ExternalForce* force : world.externalForces) {
		dynamicExternalForceSerializer.serialize(*force, ostream);
	}
	::serialize<uint64_t>(world.age, ostream);

	for(const MotorizedPhysical* p : world.physicals) {
		collectMotorizedPhysicalInformation(*p);
	}
	for(const Part& p : world.iterParts(TERRAIN_PARTS)) {
		collectPartInformation(p);
	}

	serializeCollectedHeaderInformation(ostream);


	// actually serialize the world
	size_t physicalCount = world.physicals.size();
	::serialize<uint64_t>(physicalCount, ostream);

	size_t partCount = 0;
	for(const Part& p : world.iterParts(TERRAIN_PARTS)) {
		partCount++;
	}

	::serialize<uint64_t>(partCount, ostream);

	for(const MotorizedPhysical* p : world.physicals) {
		serializeMotorizedPhysicalInContext(*p, ostream);
	}

	for(const Part& p : world.iterParts(TERRAIN_PARTS)) {
		::serialize<GlobalCFrame>(p.getCFrame(), ostream);
		virtualSerializePart(p, ostream);
	}
}
void DeSerializationSessionPrototype::deserializeWorld(WorldPrototype& world, std::istream& istream) {
	uint64_t forceCount = ::deserialize<uint64_t>(istream);
	world.externalForces.reserve(forceCount);
	for(uint64_t i = 0; i < forceCount; i++) {
		ExternalForce* force = dynamicExternalForceSerializer.deserialize(istream);
		world.externalForces.push_back(force);
	}
	world.age = ::deserialize<uint64_t>(istream);

	this->deserializeAndCollectHeaderInformation(istream);

	uint64_t numberOfPhysicals = ::deserialize<uint64_t>(istream);
	uint64_t numberOfTerrainParts = ::deserialize<uint64_t>(istream);
	world.physicals.reserve(numberOfPhysicals);

	for(uint64_t i = 0; i < numberOfPhysicals; i++) {
		MotorizedPhysical* p = deserializeMotorizedPhysicalWithContext(istream);
		world.addPart(p->getMainPart());
	}

	for(uint64_t i = 0; i < numberOfTerrainParts; i++) {
		GlobalCFrame cf = ::deserialize<GlobalCFrame>(istream);
		Part* p = virtualDeserializePart(deserializeRawPart(GlobalCFrame(), istream), istream);
		p->setCFrame(cf);
		world.addTerrainPart(p);
	}
}

void SerializationSessionPrototype::serializeParts(const Part* const parts[], size_t partCount, std::ostream& ostream) {
	for(size_t i = 0; i < partCount; i++) {
		collectPartInformation(*(parts[i]));
	}
	serializeCollectedHeaderInformation(ostream);
	::serialize<uint64_t>(static_cast<uint64_t>(partCount), ostream);
	for(size_t i = 0; i < partCount; i++) {
		::serialize<GlobalCFrame>(parts[i]->getCFrame(), ostream);
		virtualSerializePart(*(parts[i]), ostream);
	}
}

std::vector<Part*> DeSerializationSessionPrototype::deserializeParts(std::istream& istream) {
	deserializeAndCollectHeaderInformation(istream);
	size_t numberOfParts = ::deserialize<uint64_t>(istream);
	std::vector<Part*> result;
	result.reserve(numberOfParts);
	for(size_t i = 0; i < numberOfParts; i++) {
		Part* newPart = virtualDeserializePart(deserializeRawPartWithCFrame(istream), istream);
		result.push_back(newPart);
	}
	return result;
}


void SerializationSessionPrototype::serializeCollectedHeaderInformation(std::ostream& ostream) {
	::serialize<uint32_t>(CURRENT_VERSION_ID, ostream);
	this->shapeSerializer.sharedShapeClassSerializer.serializeRegistry([](const ShapeClass* sc, std::ostream& ostream) {dynamicShapeClassSerializer.serialize(*sc, ostream); }, ostream);
}

void DeSerializationSessionPrototype::deserializeAndCollectHeaderInformation(std::istream& istream) {
	uint32_t readVersionID = ::deserialize<uint32_t>(istream);
	if(readVersionID != CURRENT_VERSION_ID) {
		throw SerializationException(
			"This serialization version is outdated and cannot be read! Current " + 
			std::to_string(CURRENT_VERSION_ID) + 
			" version from stream: " + 
			std::to_string(readVersionID)
		);
	}
	shapeDeserializer.sharedShapeClassDeserializer.deserializeRegistry([](std::istream& istream) {return dynamicShapeClassSerializer.deserialize(istream); }, istream);
}

static const ShapeClass* builtinKnownShapeClasses[]{boxClass, sphereClass, cylinderClass};
SerializationSessionPrototype::SerializationSessionPrototype(const std::vector<const ShapeClass*>& knownShapeClasses) : shapeSerializer(builtinKnownShapeClasses) {
	for(const ShapeClass* sc : knownShapeClasses) {
		shapeSerializer.sharedShapeClassSerializer.addPredefined(sc);
	}
}

DeSerializationSessionPrototype::DeSerializationSessionPrototype(const std::vector<const ShapeClass*>& knownShapeClasses) : shapeDeserializer(builtinKnownShapeClasses) {
	for(const ShapeClass* sc : knownShapeClasses) {
		shapeDeserializer.sharedShapeClassDeserializer.addPredefined(sc);
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

static DynamicSerializerRegistry<ExternalForce>::ConcreteDynamicSerializer<DirectionalGravity> gravitySerializer
(serializeDirectionalGravity, deserializeDirectionalGravity, 0);



DynamicSerializerRegistry<HardConstraint> dynamicHardConstraintSerializer{
	{typeid(FixedConstraint), &fixedConstraintSerializer},
	{typeid(MotorConstraint), &motorConstraintSerializer}
};
DynamicSerializerRegistry<ShapeClass> dynamicShapeClassSerializer{
	{typeid(NormalizedPolyhedron), &polyhedronSerializer},
};
DynamicSerializerRegistry<ExternalForce> dynamicExternalForceSerializer{
	{typeid(DirectionalGravity), &gravitySerializer},
};

#pragma endregion
