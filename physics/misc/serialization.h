#pragma once

#include <typeinfo>
#include <typeindex>
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>

#include "../math/fix.h"
#include "../math/linalg/vec.h"
#include "../math/linalg/mat.h"
#include "../math/cframe.h"
#include "../math/globalCFrame.h"
#include "../geometry/polyhedron.h"
#include "../geometry/shape.h"
#include "../part.h"
#include "../world.h"
#include "../physical.h"
#include "../constraints/hardConstraint.h"
#include "../constraints/fixedConstraint.h"
#include "../constraints/motorConstraint.h"

#include "../misc/gravityForce.h"

#include "../../util/serializeBasicTypes.h"
#include "../../util/sharedObjectSerializer.h"
#include "../../util/dynamicSerialize.h"


void serializePolyhedron(const Polyhedron& poly, std::ostream& ostream);
Polyhedron deserializePolyhedron(std::istream& istream);

void serializeFixedConstraint(const FixedConstraint& object, std::ostream& ostream);
FixedConstraint* deserializeFixedConstraint(std::istream& istream);

void serializeMotorConstraint(const MotorConstraint& constraint, std::ostream& ostream);
MotorConstraint* deserializeMotorConstraint(std::istream& istream);

void serializeDirectionalGravity(const DirectionalGravity& gravity, std::ostream& ostream);
DirectionalGravity* deserializeDirectionalGravity(std::istream& istream);


class ShapeSerializer {
public:
	SharedObjectSerializer<const ShapeClass*> sharedShapeClassSerializer;
	ShapeSerializer() = default;
	template<typename List>
	inline ShapeSerializer(const List& knownShapeClasses) : sharedShapeClassSerializer(knownShapeClasses) {}

	void include(const Shape& shape);
	void serializeShape(const Shape& shape, std::ostream& ostream) const;
};
class ShapeDeserializer {
public:
	SharedObjectDeserializer<const ShapeClass*> sharedShapeClassDeserializer;
	ShapeDeserializer() = default;
	template<typename List>
	inline ShapeDeserializer(const List& knownShapeClasses) : sharedShapeClassDeserializer(knownShapeClasses) {}

	Shape deserializeShape(std::istream& ostream) const;
};


class SerializationSessionPrototype {
protected:
	ShapeSerializer shapeSerializer;

	void serializeRawPartWithoutCFrame(const Part& part, std::ostream& ostream) const;
	void serializeRawPartWithCFrame(const Part& part, std::ostream& ostream) const;

private:
	void collectMotorizedPhysicalInformation(const MotorizedPhysical& motorizedPhys);
	void collectConnectedPhysicalInformation(const ConnectedPhysical& connectedPhys);
	void collectPhysicalInformation(const Physical& phys);

	void serializeMotorizedPhysicalInContext(const MotorizedPhysical& motorizedPhys, std::ostream& ostream);
	void serializePhysicalInContext(const Physical& phys, std::ostream& ostream);
	void serializeRigidBodyInContext(const RigidBody& rigidBody, std::ostream& ostream);

protected:
	virtual void collectPartInformation(const Part& part);
	virtual void serializeCollectedHeaderInformation(std::ostream& ostream);
	virtual void virtualSerializePart(const Part& part, std::ostream& ostream);
public:
	/*initializes the SerializationSession with the given ShapeClasses as "known" at deserialization, making it unneccecary to serialize them. 
	Implicitly the builtin ShapeClasses from the physics engine, such as cubeClass and sphereClass are also included in this list */
	SerializationSessionPrototype(const std::vector<const ShapeClass*>& knownShapeClasses = std::vector<const ShapeClass*>());

	void serializeWorld(const WorldPrototype& world, std::ostream& ostream);
	void serializeParts(const Part* const parts[], size_t partCount, std::ostream& ostream);
};

class DeSerializationSessionPrototype {
private:
	MotorizedPhysical* deserializeMotorizedPhysicalWithContext(std::istream& istream);
	void deserializeConnectionsOfPhysicalWithContext(Physical& physToPopulate, std::istream& istream);
	RigidBody deserializeRigidBodyWithContext(std::istream& istream);
protected:
	ShapeDeserializer shapeDeserializer;

	Part deserializeRawPart(const GlobalCFrame& knownCFrame, std::istream& istream) const;
	Part deserializeRawPartWithCFrame(std::istream& istream) const;

	virtual void deserializeAndCollectHeaderInformation(std::istream& istream);

	virtual Part* virtualDeserializePart(Part&& partPhysicalData, std::istream& istream);

public:
	/*initializes the DeSerializationSession with the given ShapeClasses as "known" at deserialization, these are used along with the deserialized ShapeClasses
	Implicitly the builtin ShapeClasses from the physics engine, such as cubeClass and sphereClass are also included in this list */
	DeSerializationSessionPrototype(const std::vector<const ShapeClass*>& knownShapeClasses = std::vector<const ShapeClass*>());


	void deserializeWorld(WorldPrototype& world, std::istream& istream);
	std::vector<Part*> deserializeParts(std::istream& istream);
};


template<typename ExtendedPartType>
class SerializationSession : private SerializationSessionPrototype {
protected:
	using SerializationSessionPrototype::shapeSerializer;
	using SerializationSessionPrototype::serializeCollectedHeaderInformation;

	virtual void collectExtendedPartInformation(const ExtendedPartType& part) {
		SerializationSessionPrototype::collectPartInformation(part);
	}

	virtual void serializeExtendedPart(const ExtendedPartType& part, std::ostream& ostream) = 0;

private:
	virtual void collectPartInformation(const Part& part) final override {
		const ExtendedPartType& p = static_cast<const ExtendedPartType&>(part);
		collectExtendedPartInformation(p);
	}

	virtual void virtualSerializePart(const Part& part, std::ostream& ostream) final override {
		this->serializeRawPartWithoutCFrame(part, ostream);
		const ExtendedPartType& p = static_cast<const ExtendedPartType&>(part);
		serializeExtendedPart(p, ostream);
	}
public:
	using SerializationSessionPrototype::SerializationSessionPrototype;


	void serializeWorld(const World<ExtendedPartType>& world, std::ostream& ostream) {
		SerializationSessionPrototype::serializeWorld(world, ostream);
	}

	void serializeParts(const ExtendedPartType* const parts[], size_t partCount, std::ostream& ostream) {
		for(size_t i = 0; i < partCount; i++) {
			collectPartInformation(*(parts[i]));
		}
		serializeCollectedHeaderInformation(ostream);
		::serialize<size_t>(partCount, ostream);
		for(size_t i = 0; i < partCount; i++) {
			::serialize<GlobalCFrame>(parts[i]->getCFrame(), ostream);
			virtualSerializePart(*(parts[i]), ostream);
		}
	}
};

template<typename ExtendedPartType>
class DeSerializationSession : private DeSerializationSessionPrototype {
protected:
	using DeSerializationSessionPrototype::shapeDeserializer;
	using DeSerializationSessionPrototype::deserializeAndCollectHeaderInformation;
	
	virtual ExtendedPartType* deserializeExtendedPart(Part&& partPhysicalData, std::istream& istream) = 0;

private:
	virtual Part* virtualDeserializePart(Part&& partPhysicalData, std::istream& istream) final override {
		return deserializeExtendedPart(std::move(partPhysicalData), istream);
	}

public:
	using DeSerializationSessionPrototype::DeSerializationSessionPrototype;

	void deserializeWorld(World<ExtendedPartType>& world, std::istream& istream) {
		DeSerializationSessionPrototype::deserializeWorld(world, istream);
	}
	std::vector<ExtendedPartType*> deserializeParts(std::istream& istream) {
		deserializeAndCollectHeaderInformation(istream);
		size_t numberOfParts = ::deserialize<size_t>(istream);
		std::vector<ExtendedPartType*> result;
		result.reserve(numberOfParts);
		for(size_t i = 0; i < numberOfParts; i++) {
			ExtendedPartType* newPart = static_cast<ExtendedPartType*>(virtualDeserializePart(deserializeRawPartWithCFrame(istream), istream));
			result.push_back(newPart);
		}
		return result;
	}
};


extern DynamicSerializerRegistry<HardConstraint> dynamicHardConstraintSerializer;
extern DynamicSerializerRegistry<ShapeClass> dynamicShapeClassSerializer;
extern DynamicSerializerRegistry<ExternalForce> dynamicExternalForceSerializer;
